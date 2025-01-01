#include <ncurses.h>

//#include "Utilities/cJSON.h"
#include "ProgramStates/EngineState.h"
#include "GameObjects/GameSettings.h"



extern EngineState* engineState;
extern GameSettings gameSettings;
extern int scrW;
extern int scrH;
extern int mousex;
extern int mousey;
extern int terminate;

extern char* playerDbAddress;

extern float max(float a, float b);
extern float min(float a, float b);
extern char* hashWithKey(char* str, char* key);
extern int freadBinStr(FILE* file, char* dest);
extern int randBetween(int min, int max, int seed);
extern int randIndexWithProb(int n, float* prob,int seed);
extern void wrapText(char* txt, int w);
extern int fileToStr(char* address, char** buffer);
extern int saveJsonToFile(char* address, void* json);

extern MEVENT mEvent;