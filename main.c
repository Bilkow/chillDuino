#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif

 
#include <avr/io.h>  // libAVR
#include <util/delay.h>                // for _delay_ms()

#include "uart.h"

int main(void)
{
    DDRB = 0b10000000;                  // initialize port with bit7 as output
    while(1)
    {
        // turn LED on
        //PORTB |= 0b10000000;            // makes PB7 = 1
        uartInit();
        uartSendString("spam", 4);
        _delay_ms(100);                // wait 500 milliseconds
        
        //LED off
        //PORTB &= ~0b10000000;            // makes PB7 = 0
        //_delay_ms(500);                // wait 500 milliseconds
    }
}


