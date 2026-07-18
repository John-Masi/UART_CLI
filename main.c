#include <avr/interrupt.h> 
#include <string.h>
#include <util/atomic.h>
#include "uart.h"

// Our reponse from RX line that goes to the microcontroller tx line 
// and the computer gets it back on the rx line 
ISR(USART0_UDRE_vect) {
  UDR0 = tx_buffer[tx_tail++];
  if(tx_tail >= sizeof(tx_buffer)) {
    tx_tail = 0;
  }

  if(tx_head == tx_tail) {
    UCSR0B &= ~(1 << 5);
  }


}

ISR(USART0_RX_vect){
  // Reiceiving bytes from the computer TX Line 
  char c = UDR0;

  if(c == '\b' || c == 0x7F) {
    if(head > 0) {
      head--;
      buffer[head] = '\0';
      x_char('\b');
      x_char(' ');
      x_char('\b');
    }
  }
  else if(c == '\r' || c == '\n'){
    buffer[head] = '\0';
    x_char('\r');
    x_char('\n');
    ready = 1;
  }
  else { 
      buffer[head++] = c;
      x_char(c);
  }

  PORTB ^= (1 << 7);

}

int main() {
  DDRB |= (1 << 7);
  timer_init();
  usart_init();
  sei();

  while(1) {
    poll_timer();
    if(ready) {
      ready = 0;
      if(strcmp(buffer,"On") == 0) {
        PORTB |= (1 << 7);
        tx_string("LED ON\n\r");
      }
      else if(strcmp(buffer,"Off") == 0) {
        PORTB &= ~(1 << 7);
      }
      else if(strcmp(buffer, "--v") == 0) {
        tx_string("GPIO UART v.1.0\r\n");
      }
      else if(strcmp(buffer, "uptime") == 0) {
        itoa(count,buffer,10);
        tx_string(buffer);
        x_char('\n');
        x_char('\r');
      }
      head = 0;
      buffer[0] = '\0';
    }
    
  }
}
