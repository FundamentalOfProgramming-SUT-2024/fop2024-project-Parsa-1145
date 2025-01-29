#ifndef MAINGAME
#define MAINGAME

#include "EngineState.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Floor.h"
#include "../UiElements/Widget.h"

void initMainGame();
void enterMainGame();
void updateMainGame();
void renderMainGame();
void exitMainGame();

extern EngineState mainGame;
extern Camera mainCamera;
extern Player player;
extern Floor* floors;
extern int floorNum;

extern int globalTime;
extern int deltaTime;

extern Widget mgInventoryTab;
extern Widget mgItemWidget;
extern ItemBase* shownItem;

extern TextWidget mgEquipedNameTextWidget;
extern TextWidget mgEquipedPrimaryTextWidget;
extern TextWidget mgEquipedSecondaryextWidget;

extern LinkedList playerActionList;
extern int terminalGetInt();
extern void loadGame(const char* address);

extern void updateInventoryTab();
extern void updateEffectsTab();
extern void updateWorld(int x, int y);

extern int validForItemPosition(int x, int y, int z);
extern TextWidget* addMessage(char* message);
#define addFormattedMessage(f, ...) changeTextWidget(addMessage(writeLog(" ")), f, __VA_ARGS__)

#endif