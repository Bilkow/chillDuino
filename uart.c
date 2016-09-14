#include <avr/io.h>  // libAVR

#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif

#define BAUD 9600
#include <util/setbaud.h> // sets UBRR constants, USE_2X, U2X0

#include "uart.h"

void uartInit(void) { //initialize uart
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= 1 << U2X0;
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    UCSR0C = 1 << UCSZ01 | 1 << UCSZ00;
    UCSR0B = 1 << RXEN0 | 1 << TXEN0;

}

void uartSendString(char *string, int size) { // sends String via uart
    int charIndex;
    for (charIndex=0; charIndex < size; charIndex++) {
    while (! (UCSR0A & (1 << UDRE0) ) );
    UDR0 = string[charIndex];
    

    }
}