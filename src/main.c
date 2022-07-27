#include <avr/io.h>

int main(void)
{
    /* test the GNU __extension__ with -pedantic settings */
    uint8_t someBinVar = __extension__ 0b01011010;

    DDRB |= someBinVar;

    for(;;)
    {
        //
    }
}
