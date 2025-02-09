#ifndef ROLLDIALOUGE
#define ROLLDIALOUGE
#include "EngineState.h"

extern EngineState loadNewGame;

void enterRollDialouge();
void updateRollDialouge(int ch);
void renderRollDialouge();
void exitRollDialouge();

#endif