#include <stdlib.h>
#include <ncurses.h>

#include "MainMenu.h"
#include "MainGame.h"
#include "NewGameMenu.h"
#include "RollDialouge.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../UiElements/Widget.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Button.h"
#include "../UiElements/ComboBox.h"
 
Button ngmStart;
Button ngmBackBtn;
Widget ngmSideBar;

TextWidget ngmUserTextWidget;
TextWidget ngmPlayerColorTextWidget;
Widget ngmPlayerColorWidget;
TextBox ngmPlayerCrTextBox;
TextBox ngmPlayerCgTextBox;
TextBox ngmPlayerCbTextBox;

TextWidget ngmGameSettingsLabel;
Widget ngmDifficultyWidget;
TextWidget ngmDifficaultyLabel;
ComboBox ngmDifficultyCombo;
Widget ngmMapSizeWidget;
TextWidget ngmMapSizeLabel;
ComboBox ngmMapSizeCombo;
Widget ngmLootWidget;
TextWidget ngmLootLabel;
ComboBox ngmLootCombo;

LinkedList ngmUiList;

EngineState newGameMenu = {&enterNewGameMenu, &updateNewGameMenu, &renderNewGameMenu, &exitNewGameMenu, &ngmUiList};


void** ngmTmpIterPtr;
UiBase* ngmIterPtr;

void loadDifficulty(){

}

void ngmStartGame(){
    int difIndex = ngmDifficultyCombo.selected;

    
    cJSON* difficulties = openJsonFile("../Data/Difficulty.json");
    cJSON* lootProfiles = openJsonFile("../Data/LootProfiles.json");
    cJSON* mapSizeProfiles = openJsonFile("../Data/MapSizeProfiles.json");

    cJSON* difficulty = cJSON_GetArrayItem(difficulties, ngmDifficultyCombo.selected);
    cJSON* lootProfile = cJSON_GetArrayItem(lootProfiles, ngmLootCombo.selected);
    cJSON* mapSizeProfile = cJSON_GetArrayItem(mapSizeProfiles, ngmMapSizeCombo.selected);

    gameSettings.difficultyName = copyString(cJSON_GetObjectItem(difficulty, "name")->valuestring);
    gameSettings.rememberItems = cJSON_GetObjectItem(difficulty, "rememberItems")->valueint;
    gameSettings.visionRadius = cJSON_GetObjectItem(difficulty, "visionRadius")->valuedouble;
    gameSettings.baseMaxHealth = cJSON_GetObjectItem(difficulty, "baseMaximumHealth")->valueint;
    gameSettings.baseHealthRegenAmount = cJSON_GetObjectItem(difficulty, "healthRegenAmount")->valueint;
    gameSettings.baseLuck = cJSON_GetObjectItem(difficulty, "baseLuck")->valuedouble;
    gameSettings.baseSpeed = cJSON_GetObjectItem(difficulty, "baseSpeed")->valueint;

    gameSettings.minRoomNum = cJSON_GetObjectItem(mapSizeProfile, "minRoomNum")->valueint;
    gameSettings.maxRoomNum = cJSON_GetObjectItem(mapSizeProfile, "maxRoomNum")->valueint;
    gameSettings.minFloorNum = cJSON_GetObjectItem(mapSizeProfile, "minRoomNum")->valueint;
    gameSettings.maxFloorNum = cJSON_GetObjectItem(mapSizeProfile, "minRoomNum")->valueint;

    gameSettings.lootRarityMultiplier = cJSON_GetObjectItem(lootProfile, "multiplier")->valuedouble;

    if(fabs(gameSettings.visionRadius - PI) < 0.01){
        gameSettings.fullVision = 1;
    }else{
        gameSettings.fullVision = 0;
    }



    cJSON_free(difficulties);
    cJSON_free(lootProfiles);
    cJSON_free(mapSizeProfiles);

    changeAudio(getAudioByName("otherworldNight"), 1000);
    startRollingDialouge("loreDialouge", wordFade, 1000, 10, 1000, 1000, 800, 700, &startNewGame);
}


void initNewGameMenu(){
    createLinkedList(&ngmUiList, sizeof(UiBase*));

    createWidget(&ngmSideBar, NULL, ABSOLUTE, RELATIVE, ALIGN_LEFT, ALIGN_CENTER, 0, 0, 31, 100, C_BG_BLACK);
    ngmSideBar.layoutType = VERTICAL_LAYOUT;
    ngmSideBar.bordered = 1;

    createButton(&ngmStart, &ngmSideBar, "Start", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&ngmBackBtn, &ngmSideBar, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 1, 1, 6);

    {
        createTextWidget(&ngmUserTextWidget, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 3, "Playing as:");
        createTextWidget(&ngmPlayerColorTextWidget, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 1, "Charachter color:");

        createWidget(&ngmPlayerColorWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 1, 0, 100, 6, NULL);
        ngmPlayerColorWidget.bordered = 0;
        {
            createNumberInput(&ngmPlayerCrTextBox, &ngmPlayerColorWidget, "Red", player.color, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 1, 0, 9);
            createNumberInput(&ngmPlayerCgTextBox, &ngmPlayerColorWidget, "Green", player.color+1, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 10, 0, 9);
            createNumberInput(&ngmPlayerCbTextBox, &ngmPlayerColorWidget, "Blue", player.color+2, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 19, 0, 9);
            
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCrTextBox.uiBase);
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCgTextBox.uiBase);
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCbTextBox.uiBase);
        }

        createTextWidget(&ngmGameSettingsLabel, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 1, "Game settings:");
        createWidget(&ngmDifficultyWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 2, 0, 100, 1, NULL);
        {
            createTextWidget(&ngmDifficaultyLabel, &ngmDifficultyWidget, ABSOLUTE, ABSOLUTE, 0, 0, "Difficulty");
            createComboBox(&ngmDifficultyCombo, &ngmDifficultyWidget, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 11, 0, 15, 1, C_BG_GRAY0);
            {
                cJSON* difficulties = openJsonFile("../Data/Difficulty.json");
                cJSON* tmp = difficulties->child;
                while(tmp){
                    comboBoxAddOption(&ngmDifficultyCombo, cJSON_GetObjectItem(tmp, "name")->valuestring);
                    tmp = tmp->next;
                }
                cJSON_free(difficulties);
            }
            linkedListPushBack(ngmDifficultyWidget.children, ngmDifficaultyLabel.uiBase);
            linkedListPushBack(ngmDifficultyWidget.children, ngmDifficultyCombo.uiBase);
        }

        createWidget(&ngmMapSizeWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 2, 0, 100, 1, NULL);
        {
            createTextWidget(&ngmMapSizeLabel, &ngmMapSizeWidget, ABSOLUTE, ABSOLUTE, 0, 0, "Map size");
            createComboBox(&ngmMapSizeCombo, &ngmMapSizeWidget, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 11, 0, 15, 1, C_BG_GRAY0);
            {   
                cJSON* mapSizeProfiles = openJsonFile("../Data/MapSizeProfiles.json");
                cJSON* tmp = mapSizeProfiles->child;
                while(tmp){
                    comboBoxAddOption(&ngmMapSizeCombo, cJSON_GetObjectItem(tmp, "name")->valuestring);
                    tmp = tmp->next;
                }
                cJSON_free(mapSizeProfiles);
            }
            linkedListPushBack(ngmMapSizeWidget.children, ngmMapSizeLabel.uiBase);
            linkedListPushBack(ngmMapSizeWidget.children, ngmMapSizeCombo.uiBase);
        }

        createWidget(&ngmLootWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 2, 0, 100, 1, NULL);
        {
            createTextWidget(&ngmLootLabel, &ngmLootWidget, ABSOLUTE, ABSOLUTE, 0, 0, "Loot rarity");
            createComboBox(&ngmLootCombo, &ngmLootWidget, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 11, 0, 15, 1, C_BG_GRAY0);
            {
                cJSON* lootProfiles = openJsonFile("../Data/LootProfiles.json");
                cJSON* tmp = lootProfiles->child;
                while(tmp){
                    comboBoxAddOption(&ngmLootCombo, cJSON_GetObjectItem(tmp, "name")->valuestring);
                    tmp = tmp->next;
                }
                cJSON_free(lootProfiles);
            }
            linkedListPushBack(ngmLootWidget.children, ngmLootLabel.uiBase);
            linkedListPushBack(ngmLootWidget.children, ngmLootCombo.uiBase);
        }
        
        linkedListPushBack(ngmSideBar.children, ngmUserTextWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmGameSettingsLabel.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmDifficultyWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmMapSizeWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmLootWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmPlayerColorTextWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmPlayerColorWidget.uiBase);
    }

    linkedListPushBack(ngmSideBar.children, ngmStart.uiBase);
    linkedListPushBack(ngmSideBar.children, ngmBackBtn.uiBase);

    linkedListPushBack(&ngmUiList, ngmSideBar.uiBase);

    ngmBackBtn.callBack = maineMenu.enter;
    ngmStart.callBack = &ngmStartGame;
}

void enterNewGameMenu(){
    clear();
    refresh();
    engineState = &newGameMenu;

    player.color[0] = 5;
    player.color[1] = 5;
    player.color[2] = 5;

    changeTextWidget(&ngmPlayerColorTextWidget, "Character color: %i@%O", player.color, player.color+1, player.color+2);

    if(account.username){
        changeTextWidget(&ngmUserTextWidget, "Playing as: %S", account.username);
    }else{
        changeTextWidget(&ngmUserTextWidget, "Playing as: Guest");
    }

    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->update(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }


}
void updateNewGameMenu(int ch){
}
void renderNewGameMenu(){
    erase();
    emptyFrameBuffer(uiFrameBuffer);

    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->update(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }

    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->render(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }
    renderFrameBuffer(uiFrameBuffer);

    refresh();
}
void exitNewGameMenu(){

}