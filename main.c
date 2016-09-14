#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif


#include <avr/io.h>  // libAVR
#include <avr/interrupt.h> //for interruptions
#include <util/delay.h>                // for _delay_ms()
#include "uart.h" // includes library for use of AVR


int main(void) {
    DDRB = 0b10010000;                  // initialize port with bit7 as output
    
    
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


    EICRB = 0b11; // define interrupção no INT4 na rampa de SUBIDA
    EIMSK = 0b00010000; // seta interrupção no INT4 (PE4 = Digital Pin 2)

    uartInit();

    sei(); // ativa interrupções
    

    while(1) {
        // uartSendString("spam", 4);
        _delay_ms(100);                // wait x milliseconds
        
    }
}

ISR(TIMER1_COMPA_vect) {
    PORTB ^= 0b00010000; // pino 10
    // PORTB ^= 0b10000000; // pino 13
}

ISR(INT4_vect) {
    PORTB ^= 0b10000000;
}

/*
ISR(USART0_RX_vect) {
    //PORTB ^= 0b10000000;
    //tempo = UDR0;   
}*/