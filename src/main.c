#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define DELAY 2
#define ROUNDS 20

volatile uint8_t brightness;
volatile uint8_t activeLed;

inline uint16_t ramSeed(void) {
    uint16_t seed = 0xCAFE;
    for (uint16_t *i = (uint16_t *) RAMSTART; i < (uint16_t *) RAMEND; ++i) {
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

ISR(TIMER0_OVF_vect) {
    PORTB |= (1 << activeLed);
    OCR0A = brightness;
}

ISR(TIMER0_COMPA_vect) {
    PORTB &= ~(1 << activeLed);
}

ISR(INT0_vect) {

}

//uint8_t LEDS[] = {PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7};
const uint8_t LEDS[] = {PB2, PB3, PB4, PB5};

static inline uint8_t chooseLed(void) {
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

void enableAndChooseLed(void) {
    activeLed = chooseLed();
    DDRB |= (1 << activeLed);
}

void rampLedUp(void) {
    for (uint8_t i = 0; i < 255; ++i) {
        _delay_ms(DELAY);
        brightness = i;
    }
}

void rampLedDown(void) {
    for (uint8_t i = 255; i > 0; --i) {
        _delay_ms(DELAY);
        brightness = i;
    }
}

inline void switchOffLeds(void) {
    PORTB = 0;
    DDRB = 0;
}

inline void switchOffUnnnecessaryIO(void) {
    DDRD = 0; // all on PORTD are input
    PORTD = 0xff;
}

int main(void) {
    srandom(ramSeed());
    disableAnalogConverter();
    switchOffUnnnecessaryIO();
    enableTriggerInterrupt();
    sei();

    brightness = 0;
    while (1) {
        enableAndChooseLed();
        for (uint8_t r = 0; r < ROUNDS; ++r) {
            startTimer0();
            rampLedUp();
            rampLedDown();
            stopTimer0();
            switchOffLeds();
            randomSleep();
            enableAndChooseLed();
        }
        switchOffLeds();
        sleep_bod_disable(); // no brownout detection while sleeping - safes some energy
        set_sleep_mode(SLEEP_MODE_PWR_DOWN); // we go in the deepest sleep
        sleep_mode();
    }
}
