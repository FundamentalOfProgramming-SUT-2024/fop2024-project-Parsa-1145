#include <ncurses.h>

//#include "Utilities/cJSON.h"
#include "ProgramStates/EngineState.h"
#include "GameObjects/GameSettings.h"
#include "Account.h"
#include "Utilities/cJSON.h"



extern EngineState* engineState;
extern GameSettings gameSettings;
extern int scrW;
extern int scrH;
extern int mousex;
extern int mousey;
extern int terminate;
extern Account account;

extern char* playerDbAddress;

extern float max(float a, float b);
extern float min(float a, float b);
extern char* hashWithKey(char* str, char* key);
extern int freadBinStr(FILE* file, char* dest);
extern int randBetween(int min, int max, int seed);
extern int randIndexWithProb(int n, float* prob,int seed);
extern int randWithProb(float p);
extern void wrapText(char* txt, int w);
extern int fileToStr(char* address, char** buffer);
extern int saveJsonToFile(char* address, void* json);
extern int isinRect(float x, float y, float rx, float ry, float w, float h);
extern int chooseWithWeight(cJSON* json);
extern char* copyString(char* str);
extern char* writeLog(const char* format, ...);

extern MEVENT mEvent;