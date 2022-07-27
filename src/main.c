#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    DDRB |= (1 << DDB1);
    while (1) {
        PORTB |= (1 << PORTB1);
        _delay_ms(1000);
        PORTB &= ~(1 << PORTB1);
        _delay_ms(1000);
    }
}
