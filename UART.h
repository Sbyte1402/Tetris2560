#ifndef UART_H
#define UART_H

#include <stdint.h>

#define BUFFER_SIZE 64
#define TRUE  1
#define FALSE 0

#define CLEAR  0
#define RED    1
#define GREEN  2
#define YELLOW 3
#define BLUE   4

#define INC(n) (n = MOD(n + 1))
#define MOD(n) ((n) & (BUFFER_SIZE - 1))

typedef struct{
    char buffer[BUFFER_SIZE];
    volatile uint8_t in_idx;
    volatile uint8_t out_idx;
}ringBuffer_t;

void UART_init(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);
uint8_t UART_isAvailable(uint8_t com);

void UART_putchar(uint8_t com, char data);
char UART_getchar(uint8_t com);
void UART_puts(uint8_t com, char *str);
void UART_gets(uint8_t com, char *buffer);

void UART_clrscr(uint8_t com);
void UART_setColorFont(uint8_t com, uint8_t color);
void UART_setColorBackground(uint8_t com, uint8_t color);
void UART_gotoXY(uint8_t com, uint8_t x, uint8_t y);
void UART_resetColors(uint8_t com);

void itoa(uint16_t number, char *str, uint8_t base);
uint16_t atoi(char *str);

#endif
