#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define DELAY 2
#define ROUNDS 100

#define LED_PORTB 1
#define LED_PORTC 2
#define LED_PORTD 3

struct LED {
    uint8_t port;
    uint8_t pin;
};

typedef struct LED LED;

volatile uint8_t brightness;
volatile LED activeLed;

inline uint32_t ramSeed(void) {
    uint32_t seed = 0xCAFEBABE;
    for (uint32_t *i = (uint32_t *) RAMSTART; i < (uint32_t *) RAMEND; ++i) {
        if (*i != seed) seed ^= (*i);
    } // If we xor a variable with itself, we get zero
    return seed;
}

static inline void startTimer0(void) {
    TCCR0B |= (1 << CS01) | (1 << CS00); // divide by 64
    // TIMSK0 |= ((1 << OCIE0A) | (1 << OCIE1B));
    TIMSK0 |= ((1 << OCIE0A) | (1 << TOIE0));
}

static inline void stopTimer0(void) {
    TIMSK0 &= ~((1 << OCIE0A) | (1 << TOIE0));
}

inline void activateLed(LED led) {
    switch(led.port) {
        case LED_PORTB:
            PORTB |= (1 << led.pin);
            break;
        case LED_PORTC:
            PORTC |= (1 << led.pin);
            break;
        case LED_PORTD:
            PORTD |= (1 << led.pin);
            break;
    }
}

inline void deactivateLed(LED led) {
    switch(led.port) {
        case LED_PORTB:
            PORTB &= ~(1 << led.pin);
            break;
        case LED_PORTC:
            PORTC &= ~(1 << led.pin);
            break;
        case LED_PORTD:
            PORTD &= ~(1 << led.pin);
            break;
    }
}

ISR(TIMER0_OVF_vect) {
    activateLed(activeLed);
    OCR0A = brightness;
}

ISR(TIMER0_COMPA_vect) {
    deactivateLed(activeLed);
}

ISR(INT0_vect) {

}

const LED LEDS[] = { {LED_PORTB, PB1}, {LED_PORTB, PB2}, {LED_PORTB, PB3}, {LED_PORTB, PB4}, {LED_PORTB, PB5}, {LED_PORTB, PB6}, {LED_PORTB, PB7}};

static inline LED chooseLed(void) {
    return LEDS[random() % (sizeof(LEDS)/sizeof(LEDS[0]))];
}

void randomSleep(void) {
    uint8_t delayCount = random() % 32;
    for (uint8_t i = 0; i < delayCount; ++i) {
        _delay_ms(100);
    }
}

inline void disableAnalogConverter(void) {
    ACSR |= (1 << ACD); //disable analog converter
}

inline void enableTriggerInterrupt(void) {
    EICRA |= (1 << ISC01) | (1 << ISC00); // interrupt on level
    EIMSK |= (1 << INT0); // enable INT0 interrupt
}

void chooseAndEnableLed(void) {
    DDRB = 0;
    activeLed = chooseLed();
    activateLed(activeLed);
}

void testAllLeds(void) {
    PORTB = 0xff;
    DDRB = 0;

    for (uint8_t i=0;i<(sizeof(LEDS)/sizeof(LEDS[0]));++i) {
        activateLed(LEDS[i]);
        _delay_ms(200);
        deactivateLed(LEDS[i]);
    }
    PORTB = 0;
}

void rampLedUp(void) {
    for (uint8_t i = 0; i < 255; ++i) {
        brightness = i;
        _delay_ms(DELAY);
    }
}

void rampLedDown(void) {
    for (uint8_t i = 255; i > 0; --i) {
        brightness = i;
        _delay_ms(DELAY);
    }
}

inline void switchOffLeds(void) {
    DDRB = 0; // all on PORTB are input
    PORTB = 0; // all pullups disable because LEDs pull to low
}

inline void switchOffUnnnecessaryIO(void) {
    DDRD = 0; // all on PORTD are input
    PORTD = 0xff; // all pullups enable against oscillation
    DDRC = 0;
    PORTC = 0xff;
}

static void main_loop (void) __attribute__((noreturn));
void main_loop (void) {
    srandom(ramSeed());
    disableAnalogConverter();
    switchOffUnnnecessaryIO();
    enableTriggerInterrupt();
    testAllLeds();
    sei();

    brightness = 0;
    while (1) {
        chooseAndEnableLed();
        for (uint8_t r = 0; r < ROUNDS; ++r) {
            startTimer0();
            rampLedUp();
            rampLedDown();
            stopTimer0();
            switchOffLeds();
            randomSleep();
            chooseAndEnableLed();
        }
        switchOffLeds();
        sleep_bod_disable(); // no brownout detection while sleeping - safes some energy 
        set_sleep_mode(SLEEP_MODE_PWR_DOWN); // we go in the deepest sleep
        sleep_mode();
    }
}

int main (void)
{
    main_loop();
    return 0;
}
