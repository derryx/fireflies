#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY 4
volatile uint8_t brightness;

static inline void initTimer0(void) {
    TCCR0B |= (1 << CS01) | (1 << CS00); // divide by 64
   // TIMSK0 |= ((1 << OCIE0A) | (1 << OCIE1B));
    TIMSK0 |= ((1 << OCIE0A) );
    TIMSK0 |= (1 << TOIE0);
    sei();
}

ISR(TIMER0_OVF_vect) {
    PORTB |= (1 << PORTB5);
    OCR0A = brightness;
}

ISR(TIMER0_COMPA_vect) {
    PORTB &= ~ (1 << PORTB5);
}

int main(void) {
    DDRB |= (1 << DDB5);
    initTimer0();
    uint16_t rand=0x1234;
    while (1) {
        for (uint8_t i=0;i<255;++i) {
            _delay_ms(DELAY);
            brightness=i;
        }
        for (uint8_t i=255;i>0;--i) {
            _delay_ms(DELAY);
            brightness=i;
        }
        rand = 2053 * rand + 13849;
        uint8_t delayCount = rand & 0xf;
        for (uint8_t i =0; i< delayCount; ++i) {
            _delay_ms(200);
        }
/*        PORTB |= (1 << PORTB5);
        _delay_ms(1000);
        PORTB &= ~(1 << PORTB5);
        _delay_ms(1000);*/
    }
}
