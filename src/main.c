#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#define DELAY 2
#define ROUNDS 20

#define SRAM_START	0x100 // ATmega328p specific.
#define SRAM_END	0x900 // ATmega328p specific.

volatile uint8_t brightness;
volatile uint8_t activeLed;


uint8_t ramSeed(void) {
    uint8_t seed = 0xAA;
    for (uint8_t * i = (uint8_t *)SRAM_START; i < (uint8_t *)SRAM_END; i++)
        if (*i != seed) seed ^= (*i); // If we xor a variable with itself, we get zero
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

ISR(INT0_vect){
}

//uint8_t LEDS[] = {PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7};
uint8_t LEDS[] = {PB4, PB5};

int main(void) {
    srand(ramSeed());
    ACSR |= (1 << ACD); //disable analog converter
    DDRD = 0; // all on PORTD are input
    PORTD = 0xff;
    EICRA |= (1<<ISC01)|(1<<ISC00); // interrupt on level
    EIMSK |= (1<<INT0); // enable INT0 interrupt
    sei();

    brightness = 0;
    while (1) {
        activeLed = PB5;
        DDRB |= (1 << activeLed);
        for (uint8_t r=0;r<ROUNDS;++r) {
            startTimer0();
            for (uint8_t i = 0; i < 255; ++i) {
                _delay_ms(DELAY);
                brightness = i;
            }
            for (uint8_t i = 255; i > 0; --i) {
                _delay_ms(DELAY);
                brightness = i;
            }

            uint8_t delayCount = rand() & 0xf;
            for (uint8_t i = 0; i < delayCount; ++i) {
                _delay_ms(200);
            }
            stopTimer0();
            PORTB = 0;
            DDRB &= ~(1 << activeLed);
            activeLed = LEDS[rand() & 0x1];
            DDRB |= (1 << activeLed);
        }
        sleep_bod_disable();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN); // choose power down mode
        sleep_mode();
    }
}
