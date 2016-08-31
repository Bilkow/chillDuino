#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif


#include <avr/io.h>  // libAVR
#include <avr/interrupt.h> //for interruptions
#include <util/delay.h>                // for _delay_ms()
#include "uart.h" // includes library for use of AVR

int main(void)
{
    DDRB = 0b10000000;                  // initialize port with bit7 as output
    
    
    cli();
    TCCR1A = 0;
    TCCR1B = 0;

    // set compare match register to desired timer count:
    OCR1A = 15624;
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);

    sei();
    
    while(1)
    {
        // turn LED on
        //PORTB |= 0b10000000;            // makes PB7 = 1
        //uartInit();
        //uartSendString("spam", 4);
        _delay_ms(1000);                // wait x milliseconds
        
        //LED off
        //PORTB &= ~0b10000000;            // makes PB7 = 0
        //_delay_ms(500);                // wait 500 milliseconds
    }
}

ISR(TIMER1_COMPA_vect)
{
    PORTB ^= 0b10000000;
}
