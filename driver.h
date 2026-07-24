#include <stdlib.h> 
#include <stdio.h> 
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <string.h>
#include <util/atomic.h>

#pragma once

// PIN 13 the LED pin on the board
#define DDRB (*(volatile uint8_t *) 0x24)
#define PORTB (*(volatile uint8_t *) 0x25)
#define PB13 7 

// Timer/Counter 1 
#define TCN10 (*(volatile uint8_t *) 0x46)
#define TCCR1B (*(volatile uint8_t *) 0x81) 
#define TCCR1A (*(volatile uint8_t *) 0x80)
#define TIMSK1 (*(volatile uint8_t *) 0x6F)
#define TIFR1 (*(volatile uint8_t*) 0x35)

#define WGM02 3
#define CS00 0
#define CS01 1
#define CS02 2
#define OCIE0B 2
#define OCF0A 1

volatile uint8_t count;
volatile uint16_t ocr1 = 782;
volatile uint16_t tick;
volatile uint16_t tc;
volatile uint8_t flag;
volatile uint16_t value;

void timer_init(void) {
  OCR1AH = (ocr1 >> 8) & 0xFF;
  OCR1AL = (ocr1 & 0xFF);

  TCCR1B |= (1 << WGM02);
  TIMSK1 |= (1 << 1);

}

// UART0 Registers and bit locations 
#define UDR0 (*(volatile uint8_t *) 0xC6)
#define UBRR0H (*(volatile uint8_t *) 0xC5)
#define UBRR0L (*(volatile uint8_t *) 0xC4)
#define UCSR0C (*(volatile uint8_t *) 0xC2)
#define UCSR0B (*(volatile uint8_t *) 0xC1)
#define UCSR0A (*(volatile uint8_t *) 0xC0)

#define RXEN0 4 
#define TXEN0 3 
#define RXCIE0 7
#define TXCIE0 6

#define UCSZ01 2 
#define UCSZ00 1

#define UDRE0 5
#define RXCN0 7 
#define TXCN0 6

volatile char buffer[32];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile uint8_t ready;

volatile char tx_buffer[32];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;

void usart_init(void) {
  UBRR0H = 0x00;
  UBRR0L = 0x67;
  UCSR0C = 0;
  UCSR0B |= (1 << RXCIE0);
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

  // charcter size of data being sent
  UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

}

void x_char(char c) {
  tx_buffer[tx_head] = c;
  tx_head++;
  if(tx_head >= sizeof(tx_buffer)) {
    tx_head = 0;
  }

    UCSR0B |= (1 << 5);

}

void tx_string(const char* str) {
  while(*str) {
    x_char(*str);
    str++;
  }

}

// ADC Registers 
#define ADMUX (*(volatile uint8_t *)0x7C)
#define ADCSRB (*(volatile uint8_t *)0x7B)
#define ADCSRA (*(volatile uint8_t *)0x7A)
#define ADCH (*(volatile uint8_t *)0x79)
#define ADCL (*(volatile uint8_t *)0x78)


#define REFS1 7
#define REFS0 6 

void adc_init(void) {
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADIE);
    ADMUX |= (1 << REFS0);

}

void adc_poll(void) {
    ADCSRA |= (1 << ADSC);
    if(flag == 1) {
      flag = 0;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        tick = count;
        tc++;
      }
      
      uint16_t read = (value * 5.0) / 1023;
      itoa(read,buffer,10);
      tx_string(buffer);
      tx_string(" Volts");
      x_char('\n');
      x_char('\r');


      if(tc >= 7281) {
        TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
          count = 0;
          tick = 0;
        }
        tc = 0;

      }
    }
  }
