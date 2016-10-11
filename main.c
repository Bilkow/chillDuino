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
unsigned char PWM_val;

int main(void) {
    PWM_val = 255;
    DDRB = 0b10010000;                  // initialize port with bit7 and bit4 as output
    PORTE |= 1 << 4; // seta o pullup do pino 2;

    cli(); // clear interruptions
    TCCR1A = 0;
    TCCR1B = 0;

    milis = 0;
    numero_interrupt = 0;
    // = 1000;

    // set compare match register to desired timer count:
    // OCR1A = 1599; // interrompe a cada 0.1 ms
    // turn on CTC modPORTBe:
    // TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    // TCCR1B |= (1 << CS10);
    // TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    // TIMSK1 |= (1 << OCIE1A);


    // new clock: no interruptions!!
    // select clock with prescaler = 8!!! (tabela 17-6)
    TCCR1B |= (1 << CS11);

    // TCNT1H e TCNT1L -> direct I/O with R/W in the 16-bit counter!!


    // interrupcaoPORTB INT4 (PINO 2)
    EICRB = 0b11; // define interrupção no INT4 na rampa de SUBIDA
    EIMSK = 0b00010000; // seta interrupção no INT4 (PE4 = Digital Pin 2)

    uartInit();

    sei(); // ativa interrupções


    while(1) {
        // uartSendString("spam", 4);
        if (UCSR0A & (1 << RXC0)) {
            char recv_byte = UDR0, time_read[2];
            time_read[0] = TCNT1L;
            time_read[1] = TCNT1H;
            uartSendString(&recv_byte, 1);
            uartSendString(time_read, 2);
        }
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
