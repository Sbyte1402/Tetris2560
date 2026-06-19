#include <stdint.h>

typedef struct _state{
    uint16_t currentPiece;
    uint8_t currentRotation;
    uint8_t x;
    uint8_t y;
}PlayerState_t;

extern PlayerState_t playerState;