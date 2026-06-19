#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"

#define F_CPU 16000000
#define TRUE 1
#define FALSE 0

#define IS_BUFFER_EMPTY(buff) (buff.in_idx == buff.out_idx)
#define IS_BUFFER_FULL(buff) (MOD(buff.in_idx + 1) == buff.out_idx)

static ringBuffer_t bufferRX[4];
static ringBuffer_t bufferTX[4];

volatile uint8_t *UCSRA[4] = {&UCSR0A, &UCSR1A, &UCSR2A, &UCSR3A};
volatile uint8_t *UCSRB[4] = {&UCSR0B, &UCSR1B, &UCSR2B, &UCSR3B};
volatile uint8_t *UCSRC[4] = {&UCSR0C, &UCSR1C, &UCSR2C, &UCSR3C};
volatile uint16_t *UBRR[4] = {&UBRR0,  &UBRR1,  &UBRR2,  &UBRR3};

ISR(USART0_UDRE_vect){
    if(bufferTX[0].in_idx != bufferTX[0].out_idx){
        UDR0 = bufferTX[0].buffer[bufferTX[0].out_idx];
        INC(bufferTX[0].out_idx);
    } else {
        *UCSRB[0] &= ~(1 << UDRIE0);
    }
}
ISR(USART1_UDRE_vect){
    if(bufferTX[1].in_idx != bufferTX[1].out_idx){
        UDR1 = bufferTX[1].buffer[bufferTX[1].out_idx];
        INC(bufferTX[1].out_idx);
    }else{
        *UCSRB[1] &= ~(1 << UDRIE1);
    }
}
ISR(USART2_UDRE_vect){
    if(bufferTX[2].in_idx != bufferTX[2].out_idx){
        UDR2 = bufferTX[2].buffer[bufferTX[2].out_idx];
        INC(bufferTX[2].out_idx);
    }else{
        *UCSRB[2] &= ~(1 << UDRIE2);
    }
}
ISR(USART3_UDRE_vect){
    if(bufferTX[3].in_idx != bufferTX[3].out_idx){
        UDR3 = bufferTX[3].buffer[bufferTX[3].out_idx];
        INC(bufferTX[3].out_idx);
    }else{
        *UCSRB[3] &= ~(1 << UDRIE3);
    }
}
// ------------------------------------------------------------------------------------------
ISR(USART0_RX_vect){
    if((MOD(bufferRX[0].in_idx + 1)) != bufferRX[0].out_idx){
        bufferRX[0].buffer[bufferRX[0].in_idx] = UDR0;
        INC(bufferRX[0].in_idx);
    }else{
       (void)UDR0;
    }
}
ISR(USART1_RX_vect){
    if((MOD(bufferRX[1].in_idx + 1)) != bufferRX[1].out_idx){
        bufferRX[1].buffer[bufferRX[1].in_idx] = UDR1;
        INC(bufferRX[1].in_idx);
    }else{
       (void)UDR1;
    }
}
ISR(USART2_RX_vect){
    if((MOD(bufferRX[2].in_idx + 1)) != bufferRX[2].out_idx){
        bufferRX[2].buffer[bufferRX[2].in_idx] = UDR2;
        INC(bufferRX[2].in_idx);
    }else{
       (void)UDR2;
    }
}
ISR(USART3_RX_vect){
    if((MOD(bufferRX[3].in_idx + 1)) != bufferRX[3].out_idx){
        bufferRX[3].buffer[bufferRX[3].in_idx] = UDR3;
        INC(bufferRX[3].in_idx);
    }else{
       (void)UDR3;
    }
}

void UART_init(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop){
    if (com > 3) return;

    bufferTX[com].in_idx = 0;
    bufferTX[com].out_idx = 0;
    bufferRX[com].in_idx = 0;
    bufferRX[com].out_idx = 0;

    uint32_t ubrr_n = (F_CPU / (16UL * baudrate)) - 1;
    uint32_t ubrr_d = (F_CPU / (8UL * baudrate)) - 1;
    uint32_t real_n = F_CPU / (16UL * (ubrr_n + 1));
    uint32_t real_d = F_CPU / (8UL * (ubrr_d + 1));
    uint32_t err_n = (real_n > baudrate) ? (real_n - baudrate) : (baudrate - real_n);
    uint32_t err_d = (real_d > baudrate) ? (real_d - baudrate) : (baudrate - real_d);

    if(err_d < err_n){
        *UCSRA[com] |= (1 << U2X0);
        *UBRR[com] = (uint16_t)ubrr_d;
    }else{
        *UCSRA[com] &= ~(1 << U2X0);
        *UBRR[com] = (uint16_t)ubrr_n;
    }

    *UCSRC[com] = 0;
    if (size == 6) *UCSRC[com] |= (1 << UCSZ00);
    else if (size == 7) *UCSRC[com] |= (1 << UCSZ01);
    else *UCSRC[com] |= (1 << UCSZ01) | (1 << UCSZ00);

    if (parity == 1) *UCSRC[com] |= (1 << UPM01) | (1 << UPM00);
    else if (parity == 2) *UCSRC[com] |= (1 << UPM01);

    if (stop == 2) *UCSRC[com] |= (1 << USBS0);

    *UCSRB[com] = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    sei();
}
uint8_t UART_isAvailable(uint8_t com){
    return bufferRX[com].in_idx != bufferRX[com].out_idx;
}

void UART_putchar(uint8_t com, char data){
    while(IS_BUFFER_FULL(bufferTX[com]));

    bufferTX[com].buffer[bufferTX[com].in_idx] = data;
    INC(bufferTX[com].in_idx);
    *UCSRB[com] |= (1 << UDRIE0);
}
char UART_getchar(uint8_t com){
    while(!UART_isAvailable(com));

    char c = bufferRX[com].buffer[bufferRX[com].out_idx];
    INC(bufferRX[com].out_idx);

    return c;
}
void UART_puts(uint8_t com, char *str){
    while (*str) UART_putchar(com, *str++);
}
void UART_gets(uint8_t com, char *buffer){
    if(com > 3){
		buffer[0] = '\0'; 
		return;
	}

    uint8_t i = 0;
    while(TRUE){
        char c = UART_getchar(com);
        if (c == '\r' || c == '\n') {
            buffer[i] = '\0';
            break;
        } else if (c == 8 || c == 127) {
            if(i > 0){
                i--;
                UART_puts(com, "\b \b");
            }
        } else if (i < 19) {
            buffer[i++] = c;
            UART_putchar(com, c);
        }
    }
}

void UART_resetColors(uint8_t com){
    UART_puts(com, "\033[0m");
}
void UART_clrscr(uint8_t com){
    UART_puts(com, "\033[2J\033[H");
}
void UART_setColorFont(uint8_t com, uint8_t color){
    char buffer[8];
    uint8_t i = 0;

    buffer[i++] = '\033';
    buffer[i++] = '[';
    buffer[i++] = '3';
    buffer[i++] = '0' + color;
    buffer[i++] = 'm';
    buffer[i++] = '\0';

    UART_puts(com, buffer);
}
void UART_setColorBackground(uint8_t com, uint8_t color){
    char buffer[8];
    uint8_t i = 0;

    buffer[i++] = '\033';
    buffer[i++] = '[';
    buffer[i++] = '4';
    buffer[i++] = '0' + color;
    buffer[i++] = 'm';
    buffer[i++] = '\0';

    UART_puts(com, buffer);
}
void UART_gotoXY(uint8_t com, uint8_t x, uint8_t y){
    char buffer[12];
    uint8_t i = 0;

    buffer[i++] = '\033';
    buffer[i++] = '[';
    if (y >= 10) buffer[i++] = '0' + (y / 10);
    buffer[i++] = '0' + (y % 10);
    buffer[i++] = ';';
    if (x >= 10) buffer[i++] = '0' + (x / 10);
    buffer[i++] = '0' + (x % 10);
    buffer[i++] = 'H';
    buffer[i++] = '\0';

    UART_puts(com, buffer);
}
void UART_showCursor(uint8_t com){
    UART_puts(com, "\033[?25h");
}
void UART_hideCursor(uint8_t com){
    UART_puts(com, "\033[?25l");
}

void itoa(uint16_t number, char *str, uint8_t base){
    if (!str)
        return;
    if (base < 2 || base > 16){
        *str = '\0';
        return;
    }

    const char digits[] = "0123456789ABCDEF";
    char buf[17];
    uint8_t i = 0;

    if (number == 0){
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (number > 0){
        buf[i++] = digits[number % base];
        number /= base;
    }
    buf[i] = '\0';

    uint8_t j = 0;
    while (i > 0){
        str[j++] = buf[--i];
    }
    str[j] = '\0';
}
uint16_t atoi(char *str){
    uint16_t result = 0;

    while (*str){
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}
