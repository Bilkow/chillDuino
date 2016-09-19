#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif



#include <avr/io.h>  // libAVR
#include <avr/interrupt.h> //for interruptions
#include <util/delay.h>                // for _delay_ms()
#include "uart.h" // includes library for use of AVR
#include <stdio.h> // sprintf()

unsigned long int tempo_inicial;
unsigned long int delta_t[5];
unsigned char numero_interrupt;
unsigned long int milis;

int main(void) {
    DDRB = 0b10010000;                  // initialize port with bit7 and bit4 as output    

    cli(); // clear interruptions
    TCCR1A = 0;
    TCCR1B = 0;

    milis = 0;
    numero_interrupt = 0;
    // = 1000;

    // set compare match register to desired timer count:
    OCR1A = 1599; // interrompe a cada 0.1 ms
    // turn on CTC modPORTBe:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    // TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);


    // interrupcaoPORTB INT4 (PINO 2)
    EICRB = 0b11; // define interrupção no INT4 na rampa de SUBIDA
    EIMSK = 0b00010000; // seta interrupção no INT4 (PE4 = Digital Pin 2)

    uartInit();

    sei(); // ativa interrupções

    PORTF |= 1 << 2; // seta o pullup do pino 2;    

    while(1) {
        // uartSendString("spam", 4);
        _delay_ms(1000);                // wait x milliseconds
        
    }
}

ISR(TIMER1_COMPA_vect) {

    
    milis++;
    
    
    // PORTB ^= 0b00010000; // pino 10
    if (!(milis % 10000)) {
        char mensagem[8];
        unsigned long int f_rpm;
        f_rpm = (long int)600000*5/(delta_t[0] + delta_t[1] + delta_t[2] + delta_t[3] + delta_t[4]);
    
        sprintf(mensagem, "%8lu", f_rpm);
        uartSendString(mensagem, 8);
        

        PORTB ^= 0b10000000; // pino 13 
    }
}


// interrupt ventilador
ISR(INT4_vect) {
    int i;
    PORTB ^= 0b10000000;
    
    numero_interrupt++;
    if (numero_interrupt == 1)
        tempo_inicial = milis;
    else if (numero_interrupt == 3) {
        for (i = 0; i < 4; i++)  {
            delta_t[i+1] = delta_t[i];
        }
        delta_t[0] = milis - tempo_inicial;
        numero_interrupt = 0;
    }
}

/*
ISR(USART0_RX_vect) {
    //PORTB ^= 0b10000000;
    //tempo = UDR0;   
}*/
