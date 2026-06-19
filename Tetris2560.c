#include "UART.h"
#include "gameUtils.h"
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t score = 12345;

int main(void){
	UART_init(0, 9600, 8, 0, 1);
	UART_clrscr(0);
	UART_hideCursor(0);

	drawScore(score);
	drawBoard(board);
	while(TRUE){
	}

	return 0;
}
