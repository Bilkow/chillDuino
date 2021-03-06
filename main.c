#ifndef F_CPU
#define F_CPU 16000000UL // defines frequency of the AVR to 16 MHz
#endif



#include <avr/io.h>  // libAVR
#include <avr/interrupt.h> // for interruptions
#include <util/delay.h>                // for _delay_ms()
#include "uart.h" // UART functions
#include <stdio.h> // sprintf()
#include <stdlib.h>

#define TEMPO_AMOSTRAGEM 30000 // 0.120/0.000004
#define KP 0.01
#define KI 0.02
#define KD 0.3141592
char divisao_anterior;

unsigned char numero_interrupt;
char passou;
char subindo;
int freq_esperada;
int erro_somatorio;
int freq_anterior;

void envia_valor(int valor);

int main(void) {
    char string_recebida[8];
    int string_position = 0;
    passou = 0;
    subindo = 1;
    freq_esperada = 1500;
    freq_anterior = 1500;
    erro_somatorio = 0;
    
    divisao_anterior = 1;
    numero_interrupt = 0;

    DDRB |= (1 << 7);  // initialize port with bit7 as output (led)
    PORTE |= (1 << 4); // seta o pullup do pino 2;

    cli(); // clear interruptions
    TCCR1A = 0;
    TCCR1B = 0;

    // milis = 0;

    // config do fast pwm no pino digital 10!!
    DDRB |= (1 << 4);
    TCCR2A |= (1 << COM2A1) + (1 << WGM21) + (1 << WGM20); // tabelas 20-3, 20-8

    TCCR2B |= (1 << CS20); // seção 20.10.2
    OCR2A = 127; // seção 20.10.4

    
    // new clock: no interruptions!!
    // select clock with prescaler = 64 (12 bits de resolucao)!!! (tabela 17-6)
    TCCR1B |= (1 << CS11) + (1 << CS10);

    // TCNT1H e TCNT1L -> direct I/O with R/W in the 16-bit counter!!


    
    // interrupcao PORTB INT4 (PINO 2) -- entrada do sensor!
    EICRB = 0b11; // define interrupção no INT4 na rampa de SUBIDA
    EIMSK = 0b00010000; // seta interrupção no INT4 (PE4 = Digital Pin 2)


    
    // configuracao do "PWM" (gerador de onda quadrada) para o periodo de amostragem
    // configura o pino OC3A como saída, que é o PE3 e digital pin 5
    DDRE |= (1 << 3);
    // pela tabela 17-2, seta WGM32, ativar o modo CTC com topo no OCR3A
    //TCCR3B |= (1 << 3);
    TCCR3B = (1 << WGM32) + (1 << CS32) + (1 << CS30);
    // configura pra toggle on compare match, segundo a tabela 17-3, COM3A1 = 0, COM3A0 = 1.
    //TCCR3A |= (1 << COM3A0);
    // configura a fonte do clock para o clock com um prescaler de 1024 (tabela 17-6, CS32 = 1, CS31 = 0, CS30 = 1)
    //TCCR3B |= 0b101;
    // seta o topo OCR3A como 3750 para obter um periodo de 240 ms.
    OCR3AH = 0b1110;
    OCR3AL = 0b10100110;

    uartInit();

    sei(); // set interruptions


    while(1) {
        if (UCSR0A & (1 << RXC0)) {
            //OCR2A = UDR0;
            char char_recebido = UDR0;
            //uartSendString(&char_recebido, 1);
            
            string_recebida[string_position] = char_recebido;
            if (string_position >= 7) {
              uartSendString("String Position >= 7", 20);
              string_position = -1;
            }
            else if (char_recebido == 'm') {
              freq_esperada = strtol(string_recebida, NULL, 0);
	      passou = 0;
	      subindo = freq_esperada > freq_anterior ? 1 : 0;
              //uartSendString(string_recebida, 8);
              envia_valor(freq_esperada);
              string_position = -1;
            }
            else if (char_recebido == 'c')
              string_position = -1;
            string_position++;
            
        }
    }
}


void envia_valor(int valor) {
    char tamanho, texto_valor[10];
    tamanho = sprintf(texto_valor, "%u ", valor);
    uartSendString(texto_valor, tamanho);
}



// interrupt ventilador
ISR(INT4_vect) {
    PORTB ^= 0b10000000;

    numero_interrupt++;
    if (numero_interrupt == 1)
        _delay_us(100);
    else if (numero_interrupt >= 2) {
        unsigned int tempo_atual;
        char divisao_atual;
        unsigned int periodo;
        int freq_atual;

        numero_interrupt = 0;
        periodo = TCNT1L + (TCNT1H << 8);
        TCNT1H = 0;
        TCNT1L = 0;
        
        tempo_atual = (unsigned int)TCNT3L + (unsigned int)(TCNT3H << 8);
        divisao_atual = tempo_atual > 1875;
        if (divisao_atual == divisao_anterior)
            return;
        else {
            int freq_dif;
            int erro;
            int PWM;
            divisao_anterior = divisao_atual;
            freq_atual = 15000000/periodo; // 60/(tempo/250000), frequencia em rpm

            //controlador PID
            erro = freq_esperada - freq_atual;
	    erro_somatorio += erro;
	    if (erro_somatorio > 20000)
	      erro_somatorio = 20000;
	    else if (erro_somatorio < 350)
	      erro_somatorio = 350;
	    if (!passou && ((subindo && freq_atual > freq_esperada) || (!subindo && freq_atual < freq_esperada))) {
	      if (subindo)
	        erro_somatorio /= 2;
	      else
		erro_somatorio *= 1.6;
	      passou = 1;
	    }
	    //else if (erro_somatorio < 350)
	    //  erro_somatorio = 350;
	    
	    freq_dif = freq_atual - freq_anterior;
            freq_anterior = freq_atual;
            PWM = erro * KP + erro_somatorio * KI - freq_dif * KD;
            OCR2A = PWM > 255 ? 255 : PWM < 0x07 ? 0x07 : PWM;

	    //OCR2A = 0x7f;
	    
            envia_valor(freq_atual);
          
        }
    }
}
