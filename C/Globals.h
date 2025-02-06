#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/curses.h>
//#include "Utilities/cJSON.h"
#include "ProgramStates/EngineState.h"
#include "GameObjects/GameSettings.h"
#include "Account.h"
#include "Utilities/cJSON.h"
#include "Utilities/LinkedList.h"
#include "Settings.h"



extern EngineState* engineState;
extern GameSettings gameSettings;
extern int scrW;
extern int scrH;
extern int terminate;
extern Account account;
extern cJSON* itemsJson;
extern cJSON* itemTypes;
extern short rgb[6][6][6];
extern Settings settings;


extern char* playerDbAddress;

float max(float a, float b);
float min(float a, float b);
char* hashWithKey(char* str, char* key);
int freadBinStr(FILE* file, char* dest);
int randBetween(int min, int max, int seed);
float fRandBetween(float min, float max, int seed);
int randIndexWithProb(int n, float* prob,int seed);
int randWithProb(float p);
void wrapText(char* txt, int w);
int fileToStr(char* address, char** buffer);
int saveJsonToFile(char* address, void* json);
int isinRect(float x, float y, float rx, float ry, float w, float h);
int chooseWithWeight(cJSON* json);
char* copyString(char* str);
char* writeLog(const char* format, ...);
char* vWriteLog(const char* format, va_list l);
int isVowel(char c);
cJSON* openJsonFile(const char * const address);
void sleepMili(unsigned int m);
int doLinesIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void resetRgbColors();
void memFree(void*, size_t size);

extern MEVENT mEvent;