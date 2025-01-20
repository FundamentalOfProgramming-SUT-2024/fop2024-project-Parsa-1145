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

extern TextWidget mgEquipedNameTextWidget;
extern TextWidget mgEquipedPrimaryTextWidget;
extern TextWidget mgEquipedSecondaryextWidget;

extern void updateInventoryTab();
extern void updateEffectsTab();

extern int validForItemPosition(int x, int y, int z);
extern void addMessage(char* message);

#endif