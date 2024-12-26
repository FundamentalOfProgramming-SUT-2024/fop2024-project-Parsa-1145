#include <ncurses.h>
#include "ProgramStates/EngineState.h"




extern EngineState* engineState;
extern int scrW;
extern int scrH;
extern int mousex;
extern int mousey;
extern int terminate;
extern int colorPairNum;

extern char* playerDbAddress;

extern int max(int a, int b);
extern int min(int a, int b);
extern char* hashWithKey(char* str, char* key);
extern int freadBinStr(FILE* file, char* dest);
extern int randBetween(int min, int max, int seed);

extern MEVENT mEvent;