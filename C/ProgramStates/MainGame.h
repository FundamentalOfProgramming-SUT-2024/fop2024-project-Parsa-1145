#ifndef MAINGAME
#define MAINGAME

#include "EngineState.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Floor.h"
#include "../UiElements/Widget.h"

void initMainGame();
void enterMainGame();
void updateMainGame(int key);
void renderMainGame();
void exitMainGame();

extern EngineState mainGame;
extern Camera mainCamera;
extern Player player;
extern Floor* floors;
extern int floorNum;
extern int gameEnded;

extern CharTexture* frameBuffer;
extern CharTexture* visitedMaskBuffer;
extern CharTexture* depthBuffer;


extern int globalTime;
extern int deltaTime;

extern Widget mgInventoryTab;
extern Widget mgItemWidget;
extern ItemBase* shownItem;

extern TextWidget mgEquipedNameTextWidget;
extern TextWidget mgEquipedPrimaryTextWidget;
extern TextWidget mgEquipedSecondaryextWidget;

extern LinkedList playerActionList;
int terminalGetInt();
void loadGame(cJSON* json);

void startNewGame();
void updateInventoryTab();
void updateEffectsTab();
void updateWorld(int x, int y);
void renderMainGameToFramebuffer();
void renderMainGameToTerminal();

int validForItemPosition(int x, int y, int z);
int placeInRange(Floor* f, int x1, int y1, int x2, int y2, int* x, int* y);

typedef struct RayCollision{
    int x, y;
    int prevx, prevy;
    ItemBase* entity;
}RayCollision;


int castRay(int x1, int y1, int x2, int y2, Floor* f, float length, RayCollision* details);
int castRayWithDirAndDraw(int x1, int y1,long double x,long double y, Floor* f, long double length, CharTexture* tex, wchar_t c, int color, RayCollision* details);
int castRayAndDraw(int x1, int y1, int x2, int y2, Floor* f, long double length, CharTexture* tex, wchar_t c, int color, RayCollision* details);
extern TextWidget* addMessage(char* message);

void endGame(int won, char * message);
#define addFormattedMessage(f, ...) changeTextWidget(addMessage(writeLog(" ")), f, __VA_ARGS__)

#endif