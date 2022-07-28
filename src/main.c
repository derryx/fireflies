#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY 4
volatile uint8_t brightness;
volatile uint8_t activeLed;
uint16_t rand = 0x1234;

static inline uint16_t nextRand(void) {
    return rand = 2053 * rand + 13849;
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

int main(void) {
    activeLed = PB5;
    DDRB |= (1 << activeLed);
    sei();

    brightness = 0;
    while (1) {
        startTimer0();
        for (uint8_t i = 0; i <= 255; ++i) {
            _delay_ms(DELAY);
            brightness = i;
        }
        for (uint8_t i = 255; i >= 0; --i) {
            _delay_ms(DELAY);
            brightness = i;
        }

        uint8_t delayCount = nextRand() & 0xf;
        for (uint8_t i = 0; i < delayCount; ++i) {
            _delay_ms(200);
        }
        stopTimer0();
    }
}
