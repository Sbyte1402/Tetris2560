#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <avr/pgmspace.h>

#define HEIGHT 30
#define WIDTH 20

extern const PROGMEM uint16_t figures[7][4];
extern uint16_t board[HEIGHT][WIDTH];

void drawBoard(uint16_t board[HEIGHT][WIDTH]);
void drawScore(uint16_t score);

#endif
