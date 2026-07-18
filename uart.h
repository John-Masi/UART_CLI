#include <stdlib.h> 
#include <stdio.h> 

#pragma once

// PIN 13 the LED pin on the board
#define DDRB (*(volatile uint8_t *) 0x24)
#define PORTB (*(volatile uint8_t *) 0x25)
#define PB13 7 

// Timer/Counter 0 
#define OCR0B (*(volatile uint8_t *) 0x48)
#define OCR0A (*(volatile uint8_t *) 0x47) 
#define TCNT0 (*(volatile uint8_t *) 0x46)
#define TCCR0B (*(volatile uint8_t *) 0x45) 
#define TCCR0A (*(volatile uint8_t *) 0x44)
#define TIMSK0 (*(volatile uint8_t *) 0x6E)
#define TIFR0 (*(volatile uint8_t*) 0x35)

#define WGM02 3
#define CS00 0
#define CS01 1
#define CS02 2
#define OCIE0B 2
#define OCF0A 1

volatile uint8_t count;
void timer_init(void) {
  OCR0B = 249;
  TCCR0B |= (1 << WGM02);
  TCCR0B |= (1 << CS02) | (1 << CS00);

}

void poll_timer(void) {
  if(TIFR0 & (1 << OCF0A)) {
    count++;

    TIFR0 |= (1 << OCF0A);
  }
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


