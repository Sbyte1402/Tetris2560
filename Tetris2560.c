#include "UART.h"
#include "gameUtils.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t score = 0;

int main(void){
	UART_init(0, 9600, 8, 0, 1);
	UART_clrscr(0);
	drawBoard(board);
	drawScore(score);
	while(TRUE){
	}

	return 0;
}
