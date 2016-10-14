#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif



#include <avr/io.h>  // libAVR
#include <avr/interrupt.h> //for interruptions
#include <util/delay.h>                // for _delay_ms()
#include "uart.h" // includes library for use of AVR
#include <stdio.h> // sprintf()

#define TEMPO_AMOSTRAGEM 30000 // 0.120/0.000004
int desvio_acumulado;

// unsigned long int tempo_inicial;
// unsigned long int delta_t[5];
unsigned char numero_interrupt;
// unsigned long int milis;
unsigned char PWM_val;

int main(void) {
    desvio_acumulado = 0;
    numero_interrupt = 0;
    PWM_val = 255;

    DDRB = 0b10010000;                  // initialize port with bit7 and bit4 as output
    PORTE |= 1 << 4; // seta o pullup do pino 2;

    cli(); // clear interruptions
    TCCR1A = 0;
    TCCR1B = 0;

    // milis = 0;

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
    // select clock with prescaler = 64 (12 bits de resolucao)!!! (tabela 17-6)
    TCCR1B |= (1 << CS11) + (1 << CS10);

    // TCNT1H e TCNT1L -> direct I/O with R/W in the 16-bit counter!!


    // interrupcaoPORTB INT4 (PINO 2)
    EICRB = 0b11; // define interrupção no INT4 na rampa de SUBIDA
    EIMSK = 0b00010000; // seta interrupção no INT4 (PE4 = Digital Pin 2)

    uartInit();

    sei(); // ativa interrupções


    while(1) {
        // uartSendString("spam", 4);
        if (UCSR0A & (1 << RXC0)) {
            PWM_val = UDR0;
            // char recv_byte = UDR0, time_read[2];
            // time_read[0] = TCNT1L;
            // time_read[1] = TCNT1H;
            // uartSendString(&recv_byte, 1);
            // uartSendString(time_read, 2);
        }
    }
}


void envia_valor(int valor) {
    char tamanho, texto_valor[5];
    tamanho = sprintf(texto_valor, "%u ", valor);
    uartSendString(texto_valor, tamanho);
}

//  --- calculo de frequência ---
// unsigned long int f_rpm;
// f_rpm = (long int)600000*5/(delta_t[0] + delta_t[1] + delta_t[2] + delta_t[3] + delta_t[4]);
//
// sprintf(mensagem, "%8lu", f_rpm);
// uartSendString(mensagem, 8);


// interrupt ventilador
ISR(INT4_vect) {
    PORTB ^= 0b10000000;

    numero_interrupt++;
    if (numero_interrupt == 2) {
        unsigned int periodo;
        unsigned int f_rpm;
        numero_interrupt = 0;
        periodo = TCNT1L + (TCNT1H << 8);
        TCNT1H = 0;
        TCNT1L = 0;
        f_rpm = 15000000/periodo; // 60/(tempo/250000)
        
        desvio_acumulado += TEMPO_AMOSTRAGEM - periodo;
        
#ifdef DEBUG 
        char tamanho, texto_valor[5];
        tamanho = sprintf(texto_valor, " desvio(%d) periodo(%d) ", desvio_acumulado, periodo);
        uartSendString(texto_valor, tamanho);
#endif
        
        while (desvio_acumulado < 0) { // caso o periodo seja maior que o tempo de amostragem!
            envia_valor(f_rpm);
            desvio_acumulado += TEMPO_AMOSTRAGEM;
        }
        
        if (desvio_acumulado < TEMPO_AMOSTRAGEM)
            envia_valor(f_rpm);
        else
            desvio_acumulado -= TEMPO_AMOSTRAGEM;
    }
}
