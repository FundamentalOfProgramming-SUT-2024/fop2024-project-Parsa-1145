#ifndef ROLLDIALOUGE
#define ROLLDIALOUGE
#include<SDL2/SDL.h>
#include "EngineState.h"

extern EngineState loadNewGame;

enum dialougeRollType{
    simpleFade,
    wordFade,
    staryFade,
    horizontalFade
};

void startRollingDialouge(const char * const txt, enum dialougeRollType type, uint32_t ms, uint8_t stpSize, uint32_t startOffset, int cr, int cg, int cb, void (*extFunc)());

void enterRollDialouge();
void updateRollDialouge(int ch);
void renderRollDialouge();
void exitRollDialouge();

#endif