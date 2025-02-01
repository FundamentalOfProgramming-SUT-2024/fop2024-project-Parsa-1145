#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "MainGame.h"
#include "MainMenu.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../GameObjects/Items/ItemTypes.h"

#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"
#include "../Utilities/MovingCell.h"

#include "../UiElements/Button.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Checkbox.h"
#include "../UiElements/UiBase.h"
#include "../UiElements/TabWidget.h"
#include "../UiElements/TextWidget.h"



#include "../GameObjects/Player.h"
#include "../GameObjects/Room.h"
#include "../GameObjects/GameSettings.h"
#include "../GameObjects/Floor.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Line.h"
#include "../GameObjects/Renderer.h"
#include "../GameObjects/Texture.h"
#include "../GameObjects/Items/ItemBase.h"
#include "../GameObjects/Items/Useable.h"
#include "../GameObjects/Items/Intractable.h"
#include "../GameObjects/Items/Action.h"
#include "../GameObjects/Items/Monster.h"


void generateMap();

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};

Player player;
LinkedList playerActionList;

Camera mainCamera;
CharTexture* frameBuffer;
CharTexture* visitedMaskBuffer;

Floor* floors;
int floorNum;
int gameEnded;
LinkedList PathCells;

int globalTime = 0;
int deltaTime = 0;

Widget mgPauseMenu;
Button mgOptionsBtn;
Button mgSaveBtn;
Button mgExitBtn;
Button mgResumeButton;


Widget mgDebugMenu;
Button mgCloseDebugBtn;
CheckBox mgShowPointCloudCb;
CheckBox mgNoClipCb;
CheckBox mgItemDebugInfo;
CheckBox mgSeeAllCb;

Widget mgEndGameMenu;
TextWidget mgEndGameDialouge1;
TextWidget mgEndGameTotalMovesTextWidget;
TextWidget mgEndGameTotalScoreTextWidget;
TextWidget mgEndGameTotalTimeTextWidget;
Button mgEndGameExitBtn;

Widget mgSidePane;
Widget mgTerminalWidget;
TextBox mgTerminalTextBox;
Widget mgMessagesArea;

ItemBase* shownItem;
Widget mgItemWidget;
Widget mgItemWidgetWrapper;

Widget mgStatusWidget;
TabWidget mgTabWidget;
Widget mgStatsTab;
TextWidget healthTextWidget;
TextWidget goldTextWidget;
TextWidget totalMovesTextWidget;
TextWidget fullnessTextWidget;
TextWidget playerNameTextWidget;

Widget mgEffectsTab;

Widget mgInventoryTab;

Widget mgEquipedWidget;
TextWidget mgEquipedNameTextWidget;
TextWidget mgEquipedPrimaryTextWidget;
TextWidget mgEquipedSecondaryextWidget;
Widget mgInteractionsWidget;

LinkedList mgUiList;

Button* mgButtonList[1] = {&mgCloseDebugBtn};
CheckBox* mgCheckBoxList[2] = {&mgShowPointCloudCb, &mgNoClipCb};

char mgTerminalInput[50];
LinkedList messages;

void saveGame(){
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "global time", globalTime);
    cJSON_AddNumberToObject(json, "floor num", floorNum);
    
    cJSON* floorsJ = cJSON_CreateArray();
    FOR(i, floorNum){
        Floor* f = floors + i;
        cJSON* fJ = cJSON_CreateObject();
        cJSON_AddNumberToObject(fJ, "width", f->w);
        cJSON_AddNumberToObject(fJ, "height", f->h);

        cJSON_AddItemToObject(fJ, "ground mesh", saveCharTextureToJson(f->groundMesh));
        cJSON_AddItemToObject(fJ, "ground color", saveColorTextureToJson(f->groundMesh));
        cJSON_AddItemToObject(fJ, "feature mesh", saveCharTextureToJson(f->featureMesh));
        cJSON_AddItemToObject(fJ, "visited", saveCharTextureToJson(f->visited));

        cJSON* itemsJ = cJSON_CreateArray();
        {
            void** tmpPtr = f->itemList->data;
            ItemBase* ptr;

            while(tmpPtr){
                ptr = tmpPtr[1];
                tmpPtr = tmpPtr[0];

                cJSON_AddItemToArray(itemsJ, itemToJson(ptr));
            }
        }
        cJSON_AddItemToObject(fJ, "items", itemsJ);

        cJSON_AddItemToArray(floorsJ, fJ);
    }
    cJSON_AddItemToObject(json, "floors", floorsJ);

    cJSON* playerJ = cJSON_CreateObject();
    cJSON* inventoryJ = cJSON_CreateArray();
    {
        void** tmpPtr = player.items.data;
        ItemBase* ptr;

        while(tmpPtr){
            ptr = tmpPtr[1];
            tmpPtr = tmpPtr[0];

            cJSON_AddItemToArray(inventoryJ, itemToJson(ptr));
        }
    }

    cJSON_AddItemToObject(playerJ, "inventory", inventoryJ);
    cJSON_AddNumberToObject(playerJ, "x", player.x);
    cJSON_AddNumberToObject(playerJ, "y", player.y);
    cJSON_AddNumberToObject(playerJ, "z", player.z);
    cJSON_AddNumberToObject(playerJ, "total golds", player.totalGold);
    cJSON_AddNumberToObject(playerJ, "health", player.health);
    cJSON_AddNumberToObject(playerJ, "fullness", player.fullness);
    cJSON_AddNumberToObject(playerJ, "base max health", player.baseMaxHealth);
    cJSON_AddNumberToObject(playerJ, "base speed", player.baseSpeed);
    cJSON_AddNumberToObject(playerJ, "base strength", player.baseStrength);
    cJSON_AddNumberToObject(playerJ, "base fullness", player.baseFullness);
    cJSON_AddNumberToObject(playerJ, "base strength", player.baseStrength);
    cJSON_AddNumberToObject(playerJ, "vision radius", player.visionRadius);
    if(player.heldObject){
        cJSON_AddNumberToObject(playerJ, "held object", player.heldObject->id);
    }
    cJSON_AddNumberToObject(playerJ, "hunger time", player.hungerTime);
    cJSON_AddNumberToObject(playerJ, "health regen time", player.healthRegenTime);
    cJSON_AddNumberToObject(playerJ, "health regen amount", player.baseHealthRegenAmount);
    {
        cJSON* effectsJ = cJSON_CreateArray();
        
        void** tmp = player.effects.data;
        Effect* e;
        while(tmp){
            e = tmp[1];
            tmp = tmp[0];
            cJSON* eJ = effectToJson(e);
            cJSON_AddItemToArray(effectsJ, eJ);
        }
        cJSON_AddItemToObject(playerJ, "effects", effectsJ);
    }

    cJSON_AddItemToObject(json, "player", playerJ);


    saveJsonToFile("outa.json", json);
    cJSON_Delete(json);
}
void loadGame(const char* address){
    resetGame();

    cJSON* json = openJsonFile(address);

    floorNum = cJSON_GetObjectItem(json, "floor num")->valueint;
    globalTime = cJSON_GetObjectItem(json, "global time")->valueint;

    cJSON* floorsJ = cJSON_GetObjectItem(json, "floors");

    floors = malloc(sizeof(Floor) * floorNum);
    FOR(i, floorNum){
        cJSON* fJ = cJSON_GetArrayItem(floorsJ, i);
        Floor* f = floors + i;
        f->index = i;
        f->roomNum = 0;

        f->w = cJSON_GetObjectItem(fJ, "width")->valueint;
        f->h = cJSON_GetObjectItem(fJ, "height")->valueint;

        f->groundMesh = createCharTexture(f->w, f->h);
        f->featureMesh = createCharTexture(f->w, f->h);
        f->visited = createCharTexture(f->w, f->h);

        loadCharTextureFromJson(cJSON_GetObjectItem(fJ, "ground mesh"), f->groundMesh);
        loadCharTextureFromJson(cJSON_GetObjectItem(fJ, "feature mesh"), f->featureMesh);
        loadCharTextureFromJson(cJSON_GetObjectItem(fJ, "visited"), f->visited);
        loadColorTextureFromJson(cJSON_GetObjectItem(fJ, "ground color"), f->groundMesh);

        f->itemList = malloc(sizeof(LinkedList));
        createLinkedList(f->itemList, sizeof(ItemBase*));

        cJSON* tmp = cJSON_GetObjectItem(fJ, "items")->child;
        while(tmp){
            linkedListPushBack(f->itemList, loadItemFromJson(tmp));
            tmp = tmp->next;
        }
    }

    cJSON* playerJ = cJSON_GetObjectItem(json, "player");
    {
        cJSON* tmp = cJSON_GetObjectItem(playerJ, "inventory")->child;
        while(tmp){
            linkedListPushBack(&(player.items), loadItemFromJson(tmp));
            tmp = tmp->next;
        }
        updateInventoryTab();
    }
    player.x = cJSON_GetObjectItem(playerJ, "x")->valueint;
    player.y = cJSON_GetObjectItem(playerJ, "y")->valueint;
    player.z = cJSON_GetObjectItem(playerJ, "z")->valueint;
    player.totalGold = cJSON_GetObjectItem(playerJ, "total golds")->valueint;
    player.health = cJSON_GetObjectItem(playerJ, "health")->valueint;
    player.fullness = cJSON_GetObjectItem(playerJ, "fullness")->valueint;
    player.baseMaxHealth = cJSON_GetObjectItem(playerJ, "base max health")->valueint;
    player.baseSpeed = cJSON_GetObjectItem(playerJ, "base speed")->valueint;
    player.baseStrength = cJSON_GetObjectItem(playerJ, "base strength")->valueint;
    player.baseFullness = cJSON_GetObjectItem(playerJ, "base fullness")->valueint;
    player.baseStrength = cJSON_GetObjectItem(playerJ, "base strength")->valueint;
    player.visionRadius = cJSON_GetObjectItem(playerJ, "vision radius")->valueint;
    if(cJSON_GetObjectItem(playerJ, "held object")){
        player.heldObject = findItemById(cJSON_GetObjectItem(playerJ, "held object")->valueint);
    }
    {
        cJSON* effectsJ = cJSON_GetObjectItem(playerJ, "effects");
        
        cJSON* e = effectsJ->child;
        while(e){
            linkedListPushBack(&(player.effects), loadEffect(e));
            e = e->next;
        }
    }
    player.hungerTime = cJSON_GetObjectItem(playerJ, "hunger time")->valueint;
    player.healthRegenTime = cJSON_GetObjectItem(playerJ, "health regen time")->valueint;
    player.baseHealthRegenAmount = cJSON_GetObjectItem(playerJ, "health regen amount")->valueint;
    
    
    cJSON_Delete(json);


    mainCamera.h = scrH;
    mainCamera.w = scrW;
    frameBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    visitedMaskBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    updateWorld(0, 0);
    updateEffectsTab();

    engineState = &mainGame;
    timeout(100);
    nodelay(stdscr, FALSE);
}

void mgToggleStatMenu(){
    mgSidePane.isVisible =! mgSidePane.isVisible;

    renderMainGame();
}
void openItemInfo(ItemBase* o){
    if(gameSettings.debugItemInfo){
        debugItemInfo(o);
    }else{
        o->openItemInfo(o);
    }
}
void updateEffectsTab(){
    emptyWidget(&mgEffectsTab);

    Effect* iterPtr;
    void** tmpIterPtr = player.effects.data;
    int i = 0;
    while(tmpIterPtr){
        iterPtr = tmpIterPtr[1];
        tmpIterPtr = tmpIterPtr[0];
        if(iterPtr->duration != 0){
            TextWidget* name = malloc(sizeof(TextWidget));
            TextWidget* duration = malloc(sizeof(TextWidget));

            createTextWidget(name, &mgEffectsTab, ALIGN_LEFT, WITH_PARENT, 0, !i, "%s", (iterPtr->type));
            createTextWidget(duration, &mgEffectsTab, ALIGN_LEFT, WITH_PARENT, 1, 0, "duration : %d / %d", &(iterPtr->elapsed), &(iterPtr->duration));

            linkedListPushBack(mgEffectsTab.children, name->uiBase);
            linkedListPushBack(mgEffectsTab.children, duration->uiBase);
        }
        i++;
    }
}

void updateInteractionsWidget(){
    Interaction* tmpInteraction;
    TextWidget* tmpText;

    emptyWidget(&mgInteractionsWidget);

    for(void** i = playerActionList.data; i ;){
        tmpInteraction = i[1];
        i = i[0];
        tmpText = malloc(sizeof(TextWidget));
        switch(tmpInteraction->key){
            case KEY_UP:
                createTextWidget(tmpText, &mgInteractionsWidget, ALIGN_LEFT, WITH_PARENT, 1, 0, "press up to %s", tmpInteraction->name);
                break;
            case KEY_DOWN:
                createTextWidget(tmpText, &mgInteractionsWidget, ALIGN_LEFT, WITH_PARENT, 1, 0, "press down to %s", tmpInteraction->name);
                break;
            default:
                createTextWidget(tmpText, &mgInteractionsWidget, ALIGN_LEFT, WITH_PARENT, 1, 0, "press %u to %s", &(tmpInteraction->key), tmpInteraction->name);
                break;
        }
        linkedListPushBack(mgInteractionsWidget.children, tmpText->uiBase);
    }
}
void updateInventoryTab(){
    emptyWidget(&mgInventoryTab);

    ItemBase* iterPtr;
    void** tmpIterPtr = player.items.data;
    while(tmpIterPtr){
        iterPtr = tmpIterPtr[1];
        tmpIterPtr = tmpIterPtr[0];
        if(1){
            Widget* tmpWidget = malloc(sizeof(Widget));
            TextWidget* tmpTextWidget = malloc(sizeof(TextWidget));
            Button* tmpButton = malloc(sizeof(Button));

            createWidget(tmpWidget, &mgInventoryTab, RELATIVE, ABSOLUTE, ABSOLUTE, WITH_PARENT, 1, 0, 90, 1, C_BG_BLACK);
            createTextWidget(tmpTextWidget, tmpWidget, ALIGN_LEFT, ABSOLUTE, 0, 0, "%s(%o%u%O) x%d", (iterPtr->name), iterPtr->color[0], iterPtr->color[1], iterPtr->color[2], &(iterPtr->sprite), &(iterPtr->quantity));
            createButton(tmpButton, tmpWidget, "...", ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 0, 0, 3);
            tmpButton->contextObject = iterPtr;
            tmpButton->contextCallback = &openItemInfo;
            linkedListPushBack(tmpWidget->children, tmpButton->uiBase);
            linkedListPushBack(tmpWidget->children, tmpTextWidget->uiBase);
            linkedListPushBack(mgInventoryTab.children, tmpWidget->uiBase);
        }
    }
    mgInventoryTab.tmpIterPtr = NULL; //this is here to freez up the uibase iterator of the widget.
}
void mgToggleExitMenu(){
    mgPauseMenu.isVisible = !mgPauseMenu.isVisible;
    mgDebugMenu.isVisible = 0;
}
void mgtoggleDegbugMenu(){
    mgDebugMenu.isVisible = !mgDebugMenu.isVisible;
}
int uiMouseMove(){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    int flag = 0;
    while(tmp1){
        tmp = tmp1[1];
        flag |= tmp->mouseMove(tmp->object);
        tmp1 = tmp1[0];
    }
    return flag;
}
int uiKeyPress(int key){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    int flag = 0;
    while(tmp1){
        tmp = tmp1[1];
        flag |= tmp->keyPress(tmp->object, key);
        tmp1 = tmp1[0];
    }
    return flag;
}
int renderUi(){
    static UiBase* tmp;
    static void** tmp1;

    tmp1 = mgUiList.data;
    while(tmp1){
        tmp = tmp1[1];
        tmp->render(tmp->object);
        tmp1 = tmp1[0];
    }

}
void uiMouseClick(){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    while(tmp1){
        tmp = tmp1[1];
        tmp->mouseClick(tmp->object);
        tmp1 = tmp1[0];
    }
}
void updateUi(){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    while(tmp1){
        tmp = tmp1[1];
        tmp->update(tmp->object);
        tmp1 = tmp1[0];
    }
}

TextWidget* addMessage(char* message){
    TextWidget* tmp = calloc(1, sizeof(TextWidget));
    createTextWidget(tmp, &mgMessagesArea, ALIGN_LEFT, WITH_PARENT, 0, 0, message);
    linkedListInsert(mgMessagesArea.children, tmp->uiBase, 0);
    if(mgMessagesArea.children->size == 20){
        TextWidget* tmp2 = ((UiBase*)linkedListGetElement(mgMessagesArea.children, mgMessagesArea.children->size-1))->object;
        deleteTextWidget(tmp2);
        linkedListDeleteElement(mgMessagesArea.children, mgMessagesArea.children->size-1);
    }
    free(message);
    return tmp;
}

void debugRenderFramebuffer(Floor* f){
    erase();

    mainCamera.h = scrH;
    mainCamera.w = scrW;
    mainCamera.x = player.x;
    mainCamera.y = player.y;

    renderFrameBuffer(frameBuffer);
    
    fillCharTexture(frameBuffer, ' ');

    refresh();
}
void debugRenderMesh(Floor* f){
    erase();
    fillCharTexture(frameBuffer, ' ');

    mainCamera.h = scrH;
    mainCamera.w = scrW;
    mainCamera.x = player.x;
    mainCamera.y = player.y;
    
    if(gameSettings.debugMapGenerationLayer == 0){
        renderTexture(f->groundMesh, 0, 0, &mainCamera, frameBuffer);
    }else if (gameSettings.debugMapGenerationLayer==1){
        renderTexture(f->featureMesh, 0, 0, &mainCamera, frameBuffer);
    }else{
        renderTexture(f->visited, 0, 0, &mainCamera, frameBuffer);
    }
    FOR(j, f->roomNum){
        drawCircleOnCharTexture(frameBuffer, f->pointCloud[j]->x - mainCamera.x, f->pointCloud[j]->y - mainCamera.y, f->pointCloud[j]->radius, '.');
        for(int k = j+1; k < f->roomNum; k++){
            if(f->adjMat[k][j]){
                renderLine('#', -1, -1, f->pointCloud[j]->x, f->pointCloud[j]->y, f->pointCloud[k]->x, f->pointCloud[k]->y, &mainCamera, frameBuffer);
            }
        }
    }
    renderFrameBuffer(frameBuffer);

    refresh();
}

void initMainGame(){
    createLinkedList(&mgUiList, sizeof(UiBase*));

    createWidget(&mgDebugMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_GRAY0);
    {
        mgDebugMenu.isVisible = 0;
        mgDebugMenu.layoutType = VERTICAL_LAYOUT;
        mgDebugMenu.layoutPadding = 1;
        mgDebugMenu.bordered = 1;

        createButton(&mgCloseDebugBtn, &mgDebugMenu, "Close", ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 7);
        createCheckBox(&mgNoClipCb, &mgDebugMenu, "Toggle noclip", &(gameSettings.noClip), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 5, 100);
        createCheckBox(&mgItemDebugInfo, &mgDebugMenu, "Toggle item debug info", &(gameSettings.debugItemInfo), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 0, 100);
        createCheckBox(&mgShowPointCloudCb, &mgDebugMenu, "Toggle point cloud", &(gameSettings.debugShowPointCloud), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 0, 100);
        createCheckBox(&mgSeeAllCb, &mgDebugMenu, "Toggle see all", &(gameSettings.debugSeeAll), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 0, 100);

        linkedListPushBack(mgDebugMenu.children, mgNoClipCb.uiBase);
        linkedListPushBack(mgDebugMenu.children, mgItemDebugInfo.uiBase);
        linkedListPushBack(mgDebugMenu.children, mgShowPointCloudCb.uiBase);
        linkedListPushBack(mgDebugMenu.children, mgSeeAllCb.uiBase);

        linkedListPushBack(mgDebugMenu.children, mgCloseDebugBtn.uiBase);
        mgCloseDebugBtn.callBack = &mgtoggleDegbugMenu;
    }

    createWidget(&mgEndGameMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_BLACK);
    mgEndGameMenu.isVisible = 0;
    mgEndGameMenu.bordered = 1;
    mgEndGameMenu.layoutType = VERTICAL_LAYOUT;
    mgEndGameMenu.layoutPadding = 1;
    {
        createTextWidget(&mgEndGameDialouge1, &mgEndGameMenu, ALIGN_LEFT, WITH_PARENT, 1, 1, "You Won");
        createTextWidget(&mgEndGameTotalScoreTextWidget, &mgEndGameMenu, ALIGN_LEFT, WITH_PARENT, 1, 0, "You Won");
        createTextWidget(&mgEndGameTotalMovesTextWidget, &mgEndGameMenu, ALIGN_LEFT, WITH_PARENT, 1, 0, "You Won");
        createTextWidget(&mgEndGameTotalTimeTextWidget, &mgEndGameMenu, ALIGN_LEFT, WITH_PARENT, 1, 0, "You Won");
        createButton(&mgEndGameExitBtn, &mgEndGameMenu, "Return to main menu", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 1, 2, 80);
        mgEndGameExitBtn.callBack = &enterMainMenu;

        linkedListPushBack(mgEndGameMenu.children, mgEndGameDialouge1.uiBase);
        linkedListPushBack(mgEndGameMenu.children, mgEndGameTotalScoreTextWidget.uiBase);
        linkedListPushBack(mgEndGameMenu.children, mgEndGameTotalMovesTextWidget.uiBase);
        linkedListPushBack(mgEndGameMenu.children, mgEndGameTotalTimeTextWidget.uiBase);
        linkedListPushBack(mgEndGameMenu.children, mgEndGameExitBtn.uiBase);
    }


    createWidget(&mgPauseMenu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 21, 11, C_BG_GRAY0);
    mgPauseMenu.isVisible = 0;
    mgPauseMenu.bordered = 1;
    mgPauseMenu.layoutType = VERTICAL_LAYOUT;
    mgPauseMenu.layoutPadding = 1;
    {

        createButton(&mgResumeButton, &mgPauseMenu, "Resume", RELATIVE, ALIGN_CENTER, WITH_PARENT, 0, 2, 80);
        createButton(&mgOptionsBtn, &mgPauseMenu, "Options", RELATIVE, ALIGN_CENTER, WITH_PARENT, 0, 0, 80);
        createButton(&mgSaveBtn, &mgPauseMenu, "Save", RELATIVE, ALIGN_CENTER, WITH_PARENT, 0, 0, 80);
        createButton(&mgExitBtn, &mgPauseMenu, "Exit", RELATIVE, ALIGN_CENTER, WITH_PARENT, 0, 0, 80);

        linkedListPushBack(mgPauseMenu.children, mgResumeButton.uiBase);
        linkedListPushBack(mgPauseMenu.children, mgOptionsBtn.uiBase);
        linkedListPushBack(mgPauseMenu.children, mgSaveBtn.uiBase);
        linkedListPushBack(mgPauseMenu.children, mgExitBtn.uiBase);

        mgResumeButton.callBack = &mgToggleExitMenu;
        mgSaveBtn.callBack = &saveGame;
        mgExitBtn.callBack = &enterMainMenu;
    }


    createWidget(&mgSidePane, NULL, ABSOLUTE, RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 0, 30, 100, NULL);
    mgSidePane.layoutType = VERTICAL_LAYOUT;
    mgSidePane.layoutPadding = 0;
    {
        createWidget(&mgItemWidgetWrapper, &mgSidePane, RELATIVE, RELATIVE, ALIGN_LEFT, WITH_PARENT, 0, 0, 100, 20, C_BG_BLACK);
        mgItemWidgetWrapper.bordered = 1;
        {
            createWidget(&mgItemWidget, &mgItemWidgetWrapper, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 1, 1, 100, 100, C_BG_BLACK);
            mgItemWidget.layoutType = VERTICAL_LAYOUT;
            mgItemWidget.scrollOn = 1;
            mgItemWidget.layoutPadding = 0;

            linkedListPushBack(mgItemWidgetWrapper.children, mgItemWidget.uiBase);
        }

        createWidget(&mgStatusWidget, &mgSidePane, RELATIVE, RELATIVE, ALIGN_LEFT, WITH_PARENT, 0, 0, 100, 45, C_BG_BLACK);
        mgStatusWidget.bordered = 1;
        {
            createTabWidget(&mgTabWidget, &mgStatusWidget, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 1, 1, 100, 100, NULL);
            {
                createWidget(&mgStatsTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);
                mgStatsTab.layoutType = VERTICAL_LAYOUT;
                mgStatsTab.scrollOn = 1;
                {   
                    if(account.username){
                        createTextWidget(&playerNameTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Name: %s", account.username);
                    }else{
                        createTextWidget(&playerNameTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Name: Guest");
                    }
                    createTextWidget(&healthTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 1, "Health: %d / %d", &(player.health), &(player.maxHealth));
                    createTextWidget(&goldTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Golds: %d", &(player.totalGold));
                    createTextWidget(&totalMovesTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Total moves: %d", &globalTime);
                    createTextWidget(&fullnessTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Fullness: %d / %d", &player.fullness, &player.baseFullness);

                    linkedListPushBack(mgStatsTab.children, playerNameTextWidget.uiBase);
                    linkedListPushBack(mgStatsTab.children, healthTextWidget.uiBase);
                    linkedListPushBack(mgStatsTab.children, fullnessTextWidget.uiBase);
                    linkedListPushBack(mgStatsTab.children, goldTextWidget.uiBase);
                    linkedListPushBack(mgStatsTab.children, totalMovesTextWidget.uiBase);
                }

                createWidget(&mgInventoryTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);
                mgInventoryTab.layoutType = VERTICAL_LAYOUT;
                mgInventoryTab.scrollOn = 1;

                createWidget(&mgEffectsTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);
                mgEffectsTab.layoutType = VERTICAL_LAYOUT;
                mgEffectsTab.scrollOn = 1;

                tabWidgetAddTab(&mgTabWidget, "Stats", &mgStatsTab, NULL);
                tabWidgetAddTab(&mgTabWidget, "Effects", &mgEffectsTab, NULL);
                tabWidgetAddTab(&mgTabWidget, "Inventory", &mgInventoryTab, NULL);
            }
            linkedListPushBack(mgStatusWidget.children, mgTabWidget.uiBase);
        }

        createWidget(&mgTerminalWidget, NULL, ABSOLUTE, RELATIVE, ALIGN_RIGHT, ALIGN_BOTTOM, 0, 0, 40, 30, C_BG_BLACK);
        mgTerminalWidget.bordered = 1;
        {
            createWidget(&mgMessagesArea, &mgTerminalWidget, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 1, 4, 100, 100, NULL);
            mgMessagesArea.layoutType = VERTICAL_LAYOUT;
            mgMessagesArea.layoutPadding = 1;
            mgMessagesArea.scrollOn = 1;
            createTextBox(&mgTerminalTextBox, &mgTerminalWidget, "", mgTerminalInput, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 1, 1, 100);

            linkedListPushBack(mgTerminalWidget.children, mgTerminalTextBox.uiBase);
            linkedListPushBack(mgTerminalWidget.children, mgMessagesArea.uiBase);
        }
        
        linkedListPushBack(mgSidePane.children, mgStatusWidget.uiBase);
        linkedListPushBack(mgSidePane.children, mgItemWidgetWrapper.uiBase);
    }

    createWidget(&mgEquipedWidget, NULL, ABSOLUTE, RELATIVE, ALIGN_LEFT, ALIGN_CENTER, 0, 0, 20, 100, NULL);
    mgEquipedWidget.layoutType = VERTICAL_LAYOUT;
    mgEquipedWidget.layoutPadding = 0;
    {
        createTextWidget(&mgEquipedNameTextWidget, &mgEquipedWidget, ALIGN_LEFT, WITH_PARENT, 1, 1, "Nothing is equiped");
        createTextWidget(&mgEquipedPrimaryTextWidget, &mgEquipedWidget, ALIGN_LEFT, WITH_PARENT, 1, 0, "");
        createTextWidget(&mgEquipedSecondaryextWidget, &mgEquipedWidget, ALIGN_LEFT, WITH_PARENT, 1, 0, "");
        createWidget(&mgInteractionsWidget, &mgEquipedWidget, RELATIVE, ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 2, 100, 10, NULL);
        mgInteractionsWidget.layoutType = VERTICAL_LAYOUT;
        mgInteractionsWidget.scrollOn = 1;


        mgEquipedPrimaryTextWidget.widget->isVisible = 0;
        mgEquipedSecondaryextWidget.widget->isVisible = 0;

        linkedListPushBack(mgEquipedWidget.children, mgEquipedNameTextWidget.uiBase);
        linkedListPushBack(mgEquipedWidget.children, mgEquipedPrimaryTextWidget.uiBase);
        linkedListPushBack(mgEquipedWidget.children, mgEquipedSecondaryextWidget.uiBase);
        linkedListPushBack(mgEquipedWidget.children, mgInteractionsWidget.uiBase);
    }
    linkedListPushBack(&mgUiList, mgSidePane.uiBase);
    linkedListPushBack(&mgUiList, mgTerminalWidget.uiBase);
    linkedListPushBack(&mgUiList, mgEquipedWidget.uiBase);
    linkedListPushBack(&mgUiList, mgDebugMenu.uiBase);
    linkedListPushBack(&mgUiList, mgEndGameMenu.uiBase);
    linkedListPushBack(&mgUiList, mgPauseMenu.uiBase);


    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNum = 7;
    gameSettings.minRoomNum = 5;
    gameSettings.maxFloorNum = 6;
    gameSettings.minFloorNum = 4;
    gameSettings.minRoomSize = 6;
    gameSettings.maxRoomSize = 15;
    gameSettings.roomSpread = 3;
    gameSettings.roomThemeProb = malloc(4 * 4);
    gameSettings.roomThemeProb[0] = 0.5;
    gameSettings.roomThemeProb[1] = 0.2;
    gameSettings.roomThemeProb[2] = 0.25;
    gameSettings.roomThemeProb[3] = 0.05;
    gameSettings.roomThemeNum = 4;
    
    gameSettings.baseMaxHealth = 1000;
    gameSettings.baseHealthRegenTime = 5;
    gameSettings.baseHungerTime = 10;
    gameSettings.baseSpeed = 1;
    gameSettings.baseStrength = 1;
    gameSettings.maxFullness = 20;
    gameSettings.baseLuck = 1;
    gameSettings.debugMode = 1;
    gameSettings.debugShowPointCloud = 1;
    gameSettings.noClip = 1;
    gameSettings.debugItemInfo = 0;
    gameSettings.debugSeeAll = 1;
    gameSettings.debugMapGenerationLayer = 0;
    gameSettings.debugMapGenerationStepped = 0;

    floorNum = 0;

    createLinkedList(&PathCells, sizeof(MovingCell*));
    createLinkedList(&messages, sizeof(TextWidget*));

    createLinkedList(&(player.items), sizeof(ItemBase*));
    createLinkedList(&(player.effects), sizeof(Effect*));
    createLinkedList(&(playerActionList), sizeof(Interaction*));
}
void resetGame(){
    FOR(i, floorNum){
        deleteFloor(floors + i);
    }
    free(floors);

    player.x = 0;
    player.y = 0;
    player.z = 0;
    player.heldObject = NULL;
    while(player.items.size){
        defaultItemDelete((player.items.data)[1]);
        popLinkedList(&(player.items));
    }
    while(playerActionList.size){
        deleteInteraction((playerActionList.data)[1]);
        popLinkedList(&(playerActionList));
    }
    while(player.effects.size){
        deleteEffect((player.effects.data)[1]);
        popLinkedList(&(player.effects));
    }
    updateInventoryTab();
    updateEffectsTab();
    updateInteractionsWidget();
    emptyWidget(&mgItemWidget);
    emptyWidget(&mgMessagesArea);
    checkEquiped();

    player.z = 0;
    player.totalGold = 0;
    player.visionRadius = 5;
    
    player.baseMaxHealth = gameSettings.baseMaxHealth;
    player.healthModifier = 1;
    player.baseHealthRegenAmount = 1;
    player.healthRegenModifier = 1;
    player.healthRegenTime = gameSettings.baseHealthRegenTime;
    player.health = gameSettings.baseMaxHealth;

    player.baseStrength = gameSettings.baseStrength;
    player.strengthModifier = 1;

    player.baseSpeed = gameSettings.baseSpeed;
    player.speedModifier = 1;

    player.baseFullness = gameSettings.maxFullness;
    player.fullness = gameSettings.maxFullness;
    player.hungerTime = gameSettings.baseHungerTime;

    player.baseLuck = gameSettings.baseLuck;
    player.luck = player.baseLuck;
    player.luckModifier = 1;

    player.levitating = 0;
    player.invisible = 0;

    globalItemIdCounter = 1;

    if(account.username){
        changeTextWidget(&playerNameTextWidget, "Name: %s", account.username);
    }else{
        changeTextWidget(&playerNameTextWidget, "Name: Guest");
    }

    mgEndGameMenu.isVisible = 0;
    mgPauseMenu.isVisible = 0;
    mgDebugMenu.isVisible = 0;
}
void enterMainGame(){
    clear();
    refresh();
    resetGame();
        
    getmaxyx(stdscr, scrH, scrW);
    mainCamera.h = scrH;
    mainCamera.w = scrW;
    frameBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    visitedMaskBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    generateMap();

    player.z = 0;
    player.x = floors[0].roomList[0]->x+2;
    player.y = floors[0].roomList[0]->y+2;

    engineState = &mainGame;

    if(account.username){
        if(!account.gamesPlayed){
            account.firstPlayTime = time(NULL);
        }
        account.gamesPlayed++;
        saveAccount();
    }


    timeout(100);
    nodelay(stdscr, FALSE);
}
void endGame(int won, char * message){
    mgDebugMenu.isVisible = 0;
    mgPauseMenu.isVisible = 0;
    gameEnded = 1;
    if(won){
        changeTextWidget(&mgEndGameDialouge1, "%oYou found the Amulet Of Yendor%O", 5, 5, 0);
        if(account.username){
            account.gamesFinished++;
        }
    }else{
        if(message){
            changeTextWidget(&mgEndGameDialouge1, "%o%S%O", 5, 1, 0, message);
            free(message);
        }else{
            changeTextWidget(&mgEndGameDialouge1, "%oYou Died%O", 5, 1, 0);
        }
    }
    changeTextWidget(&mgEndGameTotalScoreTextWidget, "Your total score: %o%d%O", 4, 4, 0, &(player.totalGold));
    changeTextWidget(&mgEndGameTotalMovesTextWidget, "Your total moves: %d", &globalTime);
    changeTextWidget(&mgEndGameTotalTimeTextWidget, "Total play time: %d", &globalTime);
    if(account.username){
        account.goldsCollected += player.totalGold;
        if(player.totalGold >= account.goldRecord) account.goldRecord = player.totalGold;
        saveAccount();
    }


    mgEndGameMenu.isVisible = 1;
}

int validForItemPosition(int x, int y, int z){
    Floor* f = floors + z;
    static void** tmpPtr;
    static ItemBase* i;
    tmpPtr = f->itemList->data;
    if(f->groundMesh->data[y][x] == '.'){
        while(tmpPtr){
            i = tmpPtr[1];
            if((i->x == x) && (i->y == y)){
                return 0;
            }else{
                tmpPtr = tmpPtr[0];
            }
        }
        return 1;
    }
    else return 0;
}
int placeInRange(Floor* f, int x1, int y1, int x2, int y2, int* x, int* y){
    do{
        x[0] = randBetween(x1, x2, 0);
        y[0] = randBetween(y1, y2, 0);
    }
    while(!validForItemPosition(x[0], y[0], f->index));
}
int castRay(int x1, int y1, int x2, int y2, Floor* f, float length, RayCollision* details){
    float xdir = x2 - x1, ydir = y2 - y1;
    float dist = hypot(xdir, ydir);
    if(dist < 0.001){
        return 0;
    }
    xdir = xdir / dist;
    ydir = ydir / dist;

    float curx = x1, cury = y1, traversed = 0;
    while(1){
        curx += xdir;
        cury += ydir;
        traversed += 1;

        if(f->groundMesh->data[(int)cury][(int)curx] != '.'){
            if(details){
                details->x = curx;
                details->y = cury;
            }
            return 1;
        }else if(length > 0){
            if(traversed > length){
                if(details){
                    details->x = curx;
                    details->y = cury;
                }
                return 1;
            }
        }

        if(dist - traversed <= 1){
            break;
        }
    }
    return 0;
}
int castRayAndDraw(int x1, int y1, int x2, int y2, Floor* f, long double length, CharTexture* tex){
    long double xdir = x2 - x1, ydir = y2 - y1;
    x2 += 0.5;
    x1 += 0.5;
    y1 += 0.5;
    y2 += 0.5;

    long double dist = hypot(xdir, ydir);
    xdir = xdir / dist;
    ydir = ydir / dist;

    long double curx = x1, cury = y1, traversed = 0;
    int curxd, curyd;
    while(1){
        curx += xdir;
        cury += ydir;
        curxd = round(curx);
        curyd = round(cury);

        traversed += 1;
        tex->data[curyd][curxd] = 1;

        if((f->groundMesh->data[curyd][curxd] != '.')){
            return 1;
        }else if(length > 1){
            if(traversed >= length){
                return 1;
            }
        }
        if(dist - traversed <= 1){
            break;
        }
    }
    return 0;
}
int castRayWithDirAndDraw(int x1, int y1,long double x,long double y, Floor* f, long double length, CharTexture* tex){
    x1 += 0.5;
    y1 += 0.5;

    long double curx = x1, cury = y1, traversed = 0;
    int curxd, curyd;
    while(1){
        curx += x;
        cury += y;
        curxd = round(curx);
        curyd = round(cury);
        
        if(!f->featureMesh->data[curyd][curxd]){
            traversed += 1;
        }else{
            if(f->roomList[f->featureMesh->data[curyd][curxd]-1]->theme == 2){
                traversed+=2;
            }
        }
        tex->data[curyd][curxd] = 1;

        if((curx >= 0) && (curx < f->w) && (cury >= 0) && (cury < f->h) && (f->groundMesh->data[curyd][curxd] != '.')){
            return 1;
        }else if(length > 1){
            if(traversed >= length){
                return 1;
            }
        }
    }
    return 0;
}

Room* createRoomGraph(Room* start, int branchSide, int* createdNum, int minN, int maxN, float branchingProb, int depth){
    Room** branchRoomList;
    int roomNum = randBetween(minN, maxN+1, 0);
    branchRoomList = malloc(roomNum * sizeof(Room*));

    FOR(i, roomNum){
        branchRoomList[i] = malloc(sizeof(Room));
        branchRoomList[i]->right = 0;
        branchRoomList[i]->top = 0;
        branchRoomList[i]->bottom = 0;
        branchRoomList[i]->left = 0;
        branchRoomList[i]->visited = 0;
        branchRoomList[i]->index = 0;
        branchRoomList[i]->stairCandidate = 0;
    }

    int unbranchedLength = 0;
    FOR(i, roomNum){
        if(!i){
            if(roomNum > 1){
                branchRoomList[i]->right = branchRoomList[i+1];
            }
        }else if(i == roomNum-1){
            branchRoomList[i]->left = branchRoomList[i-1];
        }else{
            branchRoomList[i]->right = branchRoomList[i+1];
            branchRoomList[i]->left = branchRoomList[i-1];
        }
    }
    FOR(i, roomNum){
        if(randWithProb(branchingProb * unbranchedLength / (depth + 1))){
            int n;
            Room* tmp;
            if(randWithProb(0.5)){
                tmp = createRoomGraph(branchRoomList[i], 1,  &n,max(1.1,minN / 2), maxN / 2 , branchingProb/2, depth+1);
                if(tmp){
                    branchRoomList[(int)(min(roomNum-0.9, i + n))]->top = tmp;
                    tmp->bottom = branchRoomList[(int)(min(roomNum-0.9, i + n))];
                }
            }else{
                tmp = createRoomGraph(branchRoomList[i], 0, &n, max(1.1,minN / 2), maxN / 2, branchingProb/2, depth+1);
                if(tmp){
                    branchRoomList[(int)(min(roomNum-0.9, i + n))]->bottom = tmp;
                    tmp->top = branchRoomList[(int)(min(roomNum-0.9, i + n))];
                }
            }
            unbranchedLength = 0;
        }else{
            unbranchedLength++;
        }
    }
    if(depth!=0){
        if(branchSide){
            start->top = branchRoomList[0];
            branchRoomList[0]->bottom = start;
        }else{
            start->bottom = branchRoomList[0];
            branchRoomList[0]->top = start;
        }
        if(randWithProb(roomNum * (depth + 1) * 0.1)){
            createdNum[0] = roomNum;
            Room* r = branchRoomList[roomNum-1];
            free(branchRoomList);
            return r;
        }else{
            free(branchRoomList);
            return NULL;
        }
    }else{

        Room* r = branchRoomList[0];
        branchRoomList[roomNum-1]->stairCandidate = 1;
        branchRoomList[0]->stairCandidate = 1;
        free(branchRoomList);
        return r;
    }
}
void countRooms(Room* r, Floor* f){
    if(r->visited != 2){
        r->visited = 2;
        if(r->top){
            countRooms(r->top, f);
        }if(r->bottom){
            countRooms(r->bottom, f);
        }if(r->right){
            countRooms(r->right, f);
        }if(r->left){
            countRooms(r->left, f);
        }
        f->roomNum++;
    }
    
}
void parseRoomGraph(Room* r, Floor* f, float x, float y){
    if((r->visited) != 1){
        Point* p = malloc(sizeof(Point));
        p->x = x;
        p->y = y + randBetween(0, 3, 0) - 6;
        p->radius = randBetween(ceil(gameSettings.minRoomSize * 1.4 / 2) + 1, ceil(gameSettings.maxRoomSize * 1.4 / 2) + 1, 0);
        p->spread = randBetween(gameSettings.roomSpread, gameSettings.roomSpread * 2, 0);
        p->locked = 0;
        f->pointCloud[++f->roomNum] = p;
        r->visited = 1;
        f->roomList[f->roomNum] = r;
        r->point = p;
        r->index = f->roomNum;
        if(r->right){
            parseRoomGraph(r->right, f, x + p->radius, y);
        }if(r->top){
            parseRoomGraph(r->top, f, x, y + p->radius);
        }if(r->bottom){
            parseRoomGraph(r->bottom, f, x, y-p->radius);
        }if(r->left){
            parseRoomGraph(r->left, f, x-p->radius, y);
        }
    }
}
void createAdjMat(Floor* f){
    f->adjMat = malloc(f->roomNum * sizeof(int*));
    FOR(i, f->roomNum){
        f->adjMat[i] = calloc(f->roomNum, sizeof(int));
    }

    FOR(i, f->roomNum){
        if(f->roomList[i]->top){
            f->adjMat[i][f->roomList[i]->top->index] = 1;
        }if(f->roomList[i]->right){
            f->adjMat[i][f->roomList[i]->right->index] = 1;
        }if(f->roomList[i]->bottom){
            f->adjMat[i][f->roomList[i]->bottom->index] = 1;
        }if(f->roomList[i]->left){
            f->adjMat[i][f->roomList[i]->left->index] = 1;
        }
    }
}
void placeRoomPointCloud(Floor* f){
    int roomsPlaced = 0;
    do{
        if(roomsPlaced != 0){
            roomsPlaced = 0;
            FOR(i, f->roomNum){
                free(f->roomList[i]);
                free(f->pointCloud[i]);
                free(f->adjMat[i]);
            }
            free(f->adjMat);
            f->adjMat = NULL;
            free(f->pointCloud);
            f->pointCloud = NULL;
            free(f->roomList);
            f->roomList = NULL;
            f->roomNum = 0;
        }

        f->rootRoom = createRoomGraph(NULL, NULL,NULL, gameSettings.minRoomNum, gameSettings.maxRoomNum, 0.7, 0);
        countRooms(f->rootRoom, f);
        f->pointCloud = malloc(sizeof(Point*) * f->roomNum);
        f->roomList = malloc(sizeof(Room*) * f->roomNum);
        f->roomNum = -1;
        parseRoomGraph(f->rootRoom,f,  0, 0);
        f->roomNum++;
        createAdjMat(f);
        setFloorStairRooms(f);

        if(f->index != 0){
            Floor* prevF = floors + f->index -1;
            f->pointCloud[f->stairRooms[0]]->radius = prevF->pointCloud[prevF->stairRooms[1]]->radius;
        }
        if(gameSettings.debugMapGenerationStepped){
            timeout(1000);
            int tmp = 1;
            
            FOR(i, 4000){
                switch(getch()){
                    case KEY_RESIZE:
                        getmaxyx(stdscr, scrH, scrW);
                        clear();
                        refresh();

                        mainCamera.h = scrH;
                        mainCamera.w = scrW;

                        resizeCharTexture(&frameBuffer, mainCamera.w, mainCamera.h);
                        resizeCharTexture(&visitedMaskBuffer, mainCamera.w, mainCamera.h);
                        break;
                    case 'w':
                        player.y--;
                        break;
                    case 'd':
                        player.x++;
                        break;
                    case 's':
                        player.y++;
                        break;
                    case 'a':
                        player.x--;
                        break;
                    case 't':
                        if(iteratePointCloud(f->pointCloud, f->adjMat, f->roomNum, gameSettings.roomSpread)){
                            roomsPlaced++;
                        }
                        if(roomsPlaced > 200){
                            tmp = 0;
                        }
                        break;
                    case 'x':
                        gameSettings.debugMapGenerationLayer++;
                        gameSettings.debugMapGenerationLayer %= 3;
                        break;
                    case 'l':
                        tmp = 0;
                        break;
                }
                FOR(j, f->roomNum){
                    drawCircleOnCharTexture(frameBuffer, f->pointCloud[j]->x - mainCamera.x, f->pointCloud[j]->y - mainCamera.y, f->pointCloud[j]->radius, '.');
                    for(int k = j+1; k < f->roomNum; k++){
                        if(f->adjMat[k][j]){
                            renderLine('#', -1, -1, f->pointCloud[j]->x, f->pointCloud[j]->y, f->pointCloud[k]->x, f->pointCloud[k]->y, &mainCamera, frameBuffer);
                        }
                    }
                }
                debugRenderFramebuffer(f);
                if(!tmp) break;
            }
            timeout(100);
        }
        FOR(i, 4000){
            if(iteratePointCloud(f->pointCloud, f->adjMat, f->roomNum, gameSettings.roomSpread)){
                roomsPlaced++;
            }
            if(roomsPlaced > 200){
                break;
            }
        }
    }while(roomsPlaced < 200);
    return;
}
void roomDimensions(Floor* f){
    float lastRatio = 1;
    int k;
    FOR(i, f->roomNum){
        k = f->pointCloud[i]->radius;
        Room* r = f->roomList[i];
        r->w = randBetween(gameSettings.minRoomSize, 2 * ceil(sqrt(pow(f->pointCloud[i]->radius, 2) - pow(gameSettings.minRoomSize / 2,2)))-1,0);
        r->h = 2 * floor(sqrt(pow(f->pointCloud[i]->radius, 2) - pow(r->w / 2, 2)));
        r->z = f->index;
        if((lastRatio - 1) * ((r->w / r->h) - 1) > 0){
            int tmp = r->h;
            r->h = r->w;
            r->w = tmp;
        }
        lastRatio = r->w / r->h;

        r->x = f->pointCloud[i]->x- r->w/2;
        r->y = f->pointCloud[i]->y- r->h/2;
    }
    
    return;
}
void firstGroundmeshPass(Floor* f){
    int minx = 0, miny = 0, maxx = 0, maxy = 0;
    FOR(i, f->roomNum){
        if(!(minx)) minx = f->roomList[i]->x;
        else minx = min(f->roomList[i]->x, minx);
        if(!(miny)) miny = f->roomList[i]->y;
        else miny = min(f->roomList[i]->y, miny);
        maxx = max(f->roomList[i]->x + f->roomList[i]->w - 1, maxx);
        maxy = max(f->roomList[i]->y + f->roomList[i]->h - 1, maxy);
    }
   
    minx -= 5;
    maxx += 5;
    miny -= 5;
    maxy += 5;
    FOR(i, f->roomNum){
        f->roomList[i]->x -= minx;
        f->roomList[i]->y -= miny;
        f->pointCloud[i]->x -= minx;
        f->pointCloud[i]->y -= miny;
    }
    f->w = maxx - minx;
    f->h = maxy - miny;

    f->groundMesh = createCharTexture(f->w, f->h);
    f->featureMesh = createCharTexture(f->w, f->h);

    FOR(i, f->roomNum){
        for(int j = f->roomList[i]->x - 1; j <= f->roomList[i]->x + f->roomList[i]->w ; j++){
            for(int z = f->roomList[i]->y - 1; z <= f->roomList[i]->y + f->roomList[i]->h; z++){
               f->groundMesh->data[z][j] = '#';
               f->groundMesh->color[z][j] = bgRgb[rgb[3][3][3]][rgb[3][3][3]];
               f->featureMesh->data[z][j] = i + 1;
            }
        }
        f->featureMesh->data[f->roomList[i]->y - 1][f->roomList[i]->x - 1] = 97;
        f->featureMesh->data[f->roomList[i]->y - 1][f->roomList[i]->x + f->roomList[i]->w ] = 97;
        f->featureMesh->data[f->roomList[i]->y + f->roomList[i]->h][f->roomList[i]->x - 1] = 97;
        f->featureMesh->data[f->roomList[i]->y + f->roomList[i]->h][f->roomList[i]->x + f->roomList[i]->w ] = 97;

        for(int j = f->roomList[i]->x; j < f->roomList[i]->x + f->roomList[i]->w; j++){
            for(int z = f->roomList[i]->y; z < f->roomList[i]->y + f->roomList[i]->h; z++){
               f->groundMesh->data[z][j] = '.';
               f->groundMesh->color[z][j] = rgb[2][2][2];
               f->featureMesh->data[z][j] = 0;

            }
        }
        f->roomList[i]->neighbours = 0;
        if(f->roomList[i]->right) f->roomList[i]->neighbours++;
        if(f->roomList[i]->left) f->roomList[i]->neighbours++;
        if(f->roomList[i]->bottom) f->roomList[i]->neighbours++;
        if(f->roomList[i]->top) f->roomList[i]->neighbours++;
    }
}
void secondGroundMeshPass(Floor* f){
    FOR(i, f->roomNum){
        for(int j = f->roomList[i]->x - 1; j <= f->roomList[i]->x + f->roomList[i]->w ; j++){
            for(int z = f->roomList[i]->y - 1; z <= f->roomList[i]->y + f->roomList[i]->h; z++){
               f->featureMesh->data[z][j] = i + 1;
            }
        }
        for(int j = f->roomList[i]->x; j < f->roomList[i]->x + f->roomList[i]->w; j++){
            for(int z = f->roomList[i]->y; z < f->roomList[i]->y + f->roomList[i]->h; z++){
                if(f->roomList[i]->theme == 1){
                    f->groundMesh->color[z][j] = rgb[1][1][2];
                }else if(f->roomList[i]->theme == 2){
                    f->groundMesh->color[z][j] = rgb[2][1][1];
                }else if(f->roomList[i]->theme == 3){
                    f->groundMesh->color[z][j] = rgb[3][3][1];
                }
            }
        }
    }
}
void setFloorStairRooms(Floor* f){
    int k = 0;
    FOR(i, f->roomNum){
        if(f->roomList[i]->stairCandidate) f->stairRooms[k++] = i;
    }
}
void postProccessFloor(Floor* f){
    int tmp = 0;
    for(int i = 1; i < f->h -1 ; i++){
        for(int j = 1; j < f->w -1; j++){
            if(!(f->groundMesh->data[i][j])){
                tmp = 0;
                if(f->groundMesh->data[i+1][j] == '.') tmp++;
                if(f->groundMesh->data[i-1][j] == '.') tmp++;
                if(f->groundMesh->data[i][j+1] == '.') tmp++;
                if(f->groundMesh->data[i][j-1] == '.') tmp++;
                if(tmp == 3) f->groundMesh->data[i][j] = ';';
            }else if(f->groundMesh->data[i][j] == 'D'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->name = writeLog("Door");
                d->sprite = '+';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 0;
                d->color[1] = 2;
                d->color[0] = 2;
                d->color[2] = 0;
                linkedListPushBack(f->itemList, d);
                f->groundMesh->data[i][j] = ';';
            }else if(f->groundMesh->data[i][j] == 'L'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->name = writeLog("Door");
                d->sprite = '$';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 1;
                d->collider = 1;
                d->color[0] = 5;
                d->color[1] = 1;
                d->color[2] = 0;
                linkedListPushBack(f->itemList, d);
                f->groundMesh->data[i][j] = ';';

                ItemBase* p = calloc(1, sizeof(ItemBase));
                initPasswordGenerator(p);
                p->name = writeLog("Password generator");
                p->sprite = '&';
                p->relId = d->id;
                Room* r = f->roomList[f->featureMesh->data[i][j]-1];
                placeInRange(f, r->x + 1, r->y + 1, r->x + r->w-1, r->y + r->h-1, &(p->x), &(p->y));
                p->z = f->index;
                p->collider = 1;
                p->color[1] = 5;
                p->color[0] = 5;
                p->color[2] = 5;
                linkedListPushBack(f->itemList, p);
            }else if(f->groundMesh->data[i][j] == 'H'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->name = writeLog("Door");
                d->sprite = '+';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 0;
                d->hidden = 1;
                d->color[1] = 2;
                d->color[0] = 2;
                d->color[2] = 0;
                f->groundMesh->data[i][j] = '#';
                linkedListPushBack(f->itemList, d);
            }
        }
    }
    for(int i = 1; i < f->h -1 ; i++){
        for(int j = 1; j < f->w -1; j++){
            if(f->groundMesh->data[i][j] == ';'){
                f->groundMesh->data[i][j] = '.';
            }
        }
    }
    // for(int i = 1; i < f->h -1 ; i++){
    //     for(int j = 1; j < f->w -1; j++){
    //         if(!(f->groundMesh->data[i][j])){
    //             tmp = 0;
    //             if(f->groundMesh->data[i+1][j] == '.') tmp++;
    //             if(f->groundMesh->data[i-1][j] == '.') tmp++;
    //             if(f->groundMesh->data[i][j+1] == '.') tmp++;
    //             if(f->groundMesh->data[i][j-1] == '.') tmp++;
    //             if(tmp >= 3) f->groundMesh->data[i][j] = ';';
    //         }
    //     }
    // }
    for(int i = 1; i < f->h -1 ; i++){
        for(int j = 1; j < f->w -1; j++){
            if(f->groundMesh->data[i][j] == ';'){
                //f->groundMesh->data[i][j] = '.';
            }
        }
    }
}
void placeTraps(Floor* f){
    float trapProbs[4] = {0.5, 0.3, 0.1, 0.2};
    FOR(i, f->roomNum){
        Room* r = f->roomList[i];

        int n;

        if(r->theme == 0){
            n = randBetween(0, 4, 0);
        }else if(r->theme == 1){
            n = 0;
        }else if(r->theme == 2){
            n = randBetween(2, 5, 0);
        }else if(r->theme == 3){
            n = randBetween(5, 12, 0);
            n = min(r->w * r->h / 2 - 1, n);
        }

        FOR(j, n){
            ItemBase* t = calloc(1, sizeof(ItemBase));

            t->name = writeLog("Trap");
            placeInRange(f, r->x, r->y, r->x + r->w, r->y + r->h, &(t->x), &(t->y));

            int type = randIndexWithProb(4, trapProbs, 0);
            if((type == 3) && (f->index == floorNum-1)) type = 2;

            switch(type){
                case 0:
                    t->damage = 2;
                    t->sprite = '^';
                    t->color[0] = 5;
                    t->color[1] = 2;
                    t->color[2] = 2;
                    t->primaryUseName = writeLog("trapSimpleDamage");
                    t->primaryUse = trapSimpleDamage;
                    break;
                case 1:
                    t->sprite = '^';
                    t->color[0] = 1;
                    t->color[1] = 4;
                    t->color[2] = 1;
                    t->primaryUseName = writeLog("trapPoisonDamage");
                    t->primaryUse = trapPoisonDamage;
                    break;
                case 2:
                    t->sprite = 9635;
                    t->color[0] = 1;
                    t->color[1] = 4;
                    t->color[2] = 5;
                    t->primaryUseName = writeLog("trapTeleport");
                    t->primaryUse = trapTeleport;
                    break;
                case 3:
                    t->sprite = 11041;
                    t->color[0] = 3;
                    t->color[1] = 3;
                    t->color[2] = 2;
                    t->primaryUseName = writeLog("trapFallToNextFloor");
                    t->primaryUse = trapFallToNextFloor;
                    break;
            }
            t->hidden = 1;
            initTrap(t);
            linkedListPushBack(f->itemList, t);
        }
    }
}
void placePillars(Floor* f){
    FOR(i, f->roomNum){
        Room* r = f->roomList[i];
        int n = randBetween(0, (r->w-2) * (r->h-2) / 5, 0);
        n = min(n, 4);
        int x, y;
        FOR(j, n){
            placeInRange(f, r->x + 1, r->y + 1, r->x + r->w - 1, r->y + r->h - 1, &x, &y);
            f->groundMesh->data[y][x] = 'O';
            f->groundMesh->color[y][x] = rgb[3][3][3];
        }
    }
}
void placeMonsters(){

    cJSON* monsterTable = openJsonFile("../Data/MonsterTable.json");

    int themeNum = cJSON_GetArraySize(monsterTable);
    cJSON* roomMonsterTable[themeNum];

    FOR(i, themeNum){
        roomMonsterTable[i] = cJSON_GetArrayItem(monsterTable, i);
    }
    FOR(j, floorNum){ 
        Floor* f = floors + j;
        FOR(i, f->roomNum){
            Room* r = f->roomList[i];
            int num = randBetween(cJSON_GetObjectItem(roomMonsterTable[r->theme], "min")->valueint, cJSON_GetObjectItem(roomMonsterTable[r->theme], "max")->valueint + 1, i + j);
            num = min(num, (r->w-1) * (r->h-1) / 2);

            cJSON* enteries = cJSON_GetObjectItem(roomMonsterTable[r->theme], "enteries");
            FOR(z, num){
                int index = chooseWithWeight(enteries);

                cJSON* entery = cJSON_GetArrayItem(enteries, index);
                char* name = cJSON_GetObjectItem(entery, "name")->valuestring;
                ItemBase* gen;

                cJSON* tmp = itemsJson->child;
                int found = 0;
                while(tmp){
                    if(!strcmp(cJSON_GetObjectItem(tmp, "name")->valuestring, name)){
                        gen = loadItemFromJson(tmp);
                        found = 1;
                        break;
                    }else{
                        tmp = tmp->next;
                    }
                }
                if(found){
                    placeInRange(f, r->x, r->y, r->x + r->w-1, r->y + r->h-1, &(gen->x), &(gen->y));
                    gen->z = j;
                    linkedListPushBack(f->itemList, gen);
                }
            }
        }
    }
    cJSON_free(monsterTable);
}
void generateLoot(){
    cJSON* lootTableJson = openJsonFile("../Data/LootTable.json");

    int themeNum = cJSON_GetArraySize(lootTableJson);
    cJSON* roomLootTable[themeNum];

    FOR(i, themeNum){
        roomLootTable[i] = cJSON_GetArrayItem(lootTableJson, i);
    }

    FOR(j, floorNum){ 
        Floor* f = floors + j;
        Floor* nextF = floors + j + 1;
        if(j!=floorNum-1){
            int x1, y1, x2, y2;
            do{
                x1 = randBetween(f->roomList[f->stairRooms[1]]->x + 1,f->roomList[f->stairRooms[1]]->x + f->roomList[f->stairRooms[1]]->w-1, 0);
                y1 = randBetween(f->roomList[f->stairRooms[1]]->y + 1,f->roomList[f->stairRooms[1]]->y + f->roomList[f->stairRooms[1]]->h - 1, 0);
                x2 = x1 - f->roomList[f->stairRooms[1]]->x + (f+1)->roomList[f->stairRooms[0]]->x;
                y2 = y1 - f->roomList[f->stairRooms[1]]->y + (f+1)->roomList[f->stairRooms[0]]->y;;
            }
            while((!validForItemPosition(x2, y2, j+1)) && (!validForItemPosition(x1, y1, j)));
            ItemBase* s1 = calloc(1, sizeof(ItemBase));
            ItemBase* s2 = calloc(1, sizeof(ItemBase));
            initStair(s1);
            initStair(s2);
            s1->name = writeLog("Stair");
            s2->name = writeLog("Stair");
            s1->z = j;
            s1->x = x1;
            s1->y = y1;
            s1->sprite = '#';
            s1->color[0] = 5; s1->color[1] = 5;s1->color[2] = 5;
            linkedListPushBack(f->itemList, s1);
            s2->z = j+1;
            s2->x = x2;
            s2->y = y2;
            s2->sprite = '#';
            s2->color[0] = 5; s2->color[1] = 5;s2->color[2] = 5;
            linkedListPushBack(nextF->itemList, s2);
            s1->relId = s2->id;
            s2->relId = s1->id;
        }
        FOR(i, f->roomNum){
            Room* r = f->roomList[i];
            int num = randBetween(cJSON_GetObjectItem(roomLootTable[r->theme], "minRolls")->valueint, cJSON_GetObjectItem(roomLootTable[r->theme], "maxRolls")->valueint + 1, i + j);
            num = min(num, (r->w-1) * (r->h-1) / 2);

            cJSON* enteries = cJSON_GetObjectItem(roomLootTable[r->theme], "enteries");
            FOR(z, num){
                int index = chooseWithWeight(enteries);

                cJSON* entery = cJSON_GetArrayItem(enteries, index);
                char* name = cJSON_GetObjectItem(entery, "name")->valuestring;
                ItemBase* gen;

                cJSON* tmp = itemsJson->child;
                int found = 0;
                while(tmp){
                    if(!strcmp(cJSON_GetObjectItem(tmp, "name")->valuestring, name)){
                        gen = loadItemFromJson(tmp);
                        found = 1;
                        break;
                    }else{
                        tmp = tmp->next;
                    }
                }
                if(found){
                    if(cJSON_GetObjectItem(entery, "min")){
                        gen->quantity = randBetween(cJSON_GetObjectItem(entery, "min")->valueint, cJSON_GetObjectItem(entery, "max")->valueint + 1, 0);
                    }else{
                        gen->quantity = 1;
                    }
                    placeInRange(f, r->x, r->y, r->x + r->w-1, r->y + r->h-1, &(gen->x), &(gen->y));
                    gen->z = j;
                    if(r->theme == 2) gen->fake = 1;
                    linkedListPushBack(f->itemList, gen);
                }
            }
            if(r->theme == 3){
                ItemBase* amulet = LoadItemWithName("Amulet of yendor");
                amulet->quantity = 1;

                placeInRange(f, r->x, r->y, r->x + r->w-1, r->y + r->h-1, &(amulet->x), &(amulet->y));
                amulet->z = j;
                linkedListPushBack(f->itemList, amulet);
            }
        }
    }
    cJSON_free(lootTableJson);
}

void generateFloor(Floor* f){
    f->roomNum = 0;
    f->pointCloud = NULL;
    f->roomList = NULL;
    f->adjMat = NULL;
    f->itemList = malloc(sizeof(LinkedList));
    createLinkedList(f->itemList, sizeof(ItemBase*));

    placeRoomPointCloud(f);
    roomDimensions(f);
    if(f->index != 0){
        Floor* prevF = floors + f->index -1;
        f->roomList[f->stairRooms[0]]->w = prevF->roomList[prevF->stairRooms[1]]->w;
        f->roomList[f->stairRooms[0]]->h = prevF->roomList[prevF->stairRooms[1]]->h;
    }
    firstGroundmeshPass(f);
    f->pathFindMesh = createCharTexture(f->groundMesh->w, f->groundMesh->h);
    f->visited = createCharTexture(f->groundMesh->w, f->groundMesh->h);
    f->visionMesh = createCharTexture(f->groundMesh->w, f->groundMesh->h);
    fillCharTexture(f->visited, 0);
    
    MovingCell* cell;
    FOR(i, f->roomNum){
        for(int j = i + 1; j < f->roomNum; j++){
            
            if(f->adjMat[i][j]){
                int x1[2], y1[2], x2[2], y2[2];
                
                x1[0] = f->roomList[i]->x-1;
                x1[1] = f->roomList[i]->x + f->roomList[i]->w;
                x2[0] = f->roomList[j]->x-1;
                x2[1] = f->roomList[j]->x+ f->roomList[j]->w;
                y1[0] = f->roomList[i]->y-1;
                y1[1] = f->roomList[i]->y + f->roomList[i]->h;
                y2[0] = f->roomList[j]->y-1;
                y2[1] = f->roomList[j]->y+ f->roomList[j]->h;
                int rx, ry, dx, dy;

                float xdif = x2[0] - x1[0], ydif = y2[0] - y1[0];

                cell = calloc(1, sizeof(MovingCell));
                linkedListPushBack(&PathCells, cell);
                cell->x = randBetween(f->roomList[i]->x, f->roomList[i]->x+ f->roomList[i]->w, 0);
                cell->y = randBetween(f->roomList[i]->y, f->roomList[i]->y+ f->roomList[i]->h, 0);
                cell->type = 0;
                cell->attr[0] = randBetween(f->roomList[j]->x, f->roomList[j]->x+ f->roomList[j]->w, 0);
                cell->attr[1] = randBetween(f->roomList[j]->y, f->roomList[j]->y+ f->roomList[j]->h, 0);
                cell->attr[2] = 0;
                cell->attr[3] = randBetween(0, 2, 0);
                cell->attr[4] = j+1;
                cell->attr[5] = i+1;
                cell->attr[6] = 0;
                if((f->roomList[i]->neighbours == 2) && (f->roomList[j]->neighbours == 1)&& (!f->roomList[j]->stairCandidate)){
                    cell->attr[7] = 1;
                    f->roomList[j]->hidden = 1;
                }else if(f->roomList[i]->neighbours == 2){
                    if(randWithProb(0.6)){
                        cell->attr[7] = 2;
                    }else{
                        cell->attr[7] = 0;
                    }
                }
            }
        }
    }

    if(gameSettings.debugMapGenerationStepped){
        int tmp = 1;
        timeout(1000);
        while(PathCells.size && tmp){
        switch(getch()){
            case KEY_RESIZE:
                //getmaxyx(stdscr, scrH, scrW);
                clear();
                refresh();

                mainCamera.h = scrH;
                mainCamera.w = scrW;

                resizeCharTexture(&frameBuffer, mainCamera.w, mainCamera.h);
                resizeCharTexture(&visitedMaskBuffer, mainCamera.w, mainCamera.h);
                break;
            case 'w':
                player.y--;
                break;
            case 'd':
                player.x++;
                break;
            case 's':
                player.y++;
                break;
            case 'a':
                player.x--;
                break;
            case 't':
                iterateMovingCells(&PathCells, f);
                break;
            case 'x':
                gameSettings.debugMapGenerationLayer++;
                gameSettings.debugMapGenerationLayer %= 3;
                break;
            case 'l':
                tmp = 0;
                break;
        }
        
        debugRenderMesh(f);
        
        
        }
        timeout(100);
    }
    while(PathCells.size){
        iterateMovingCells(&PathCells, f);
    }
    
    FOR(i, f->roomNum){
        if(f->roomList[i]->hidden) f->roomList[i]->theme = 1;
        else if(f->roomList[i]->stairCandidate && randWithProb(0.7)){
            f->roomList[i]->theme = 1;
        }else if(randWithProb(((float)f->index / (float)floorNum) * 0.7)){
            f->roomList[i]->theme = 2;
        }else{
            f->roomList[i]->theme = 0;
        }
    }
    if(f->index == floorNum-1){
        f->roomList[f->stairRooms[1]]->theme = 3;
    }
    FOR(i, f->roomNum){
        for(int j = f->roomList[i]->x - 1; j <= f->roomList[i]->x + f->roomList[i]->w ; j++){
            for(int z = f->roomList[i]->y - 1; z <= f->roomList[i]->y + f->roomList[i]->h; z++){
               f->featureMesh->data[z][j] = '0' + i + 1;
            }
        }
    }

    secondGroundMeshPass(f);
    postProccessFloor(f);
}

void generateMap(){
    floorNum = randBetween(gameSettings.minFloorNum, gameSettings.maxFloorNum + 1, 0);
    floors = malloc(floorNum * sizeof(Floor));
    
    FOR(i, floorNum){
        floors[i].index = i;
        generateFloor(floors + i);
        placePillars(floors + i);
        placeTraps(floors + i);
        fillCharTexture(floors[i].visited, '\0');
    }

    placeMonsters();
    generateLoot();
}

int rays = 800;
void updateVisionMesh(){
    Floor* f = floors + player.z;
    fillCharTexture(f->visionMesh, '\0');

    static RayCollision c;
    int x2 = mainCamera.x + mainCamera.w, y2 = mainCamera.y + mainCamera.h;
    // for(int i = mainCamera.x; i < x2; i++){
    //     if(castRay(player.x, player.y, i, mainCamera.y, f, 0, &c)){
    //         renderLine(97, player.x, player.y, c.x, c.y, &mainCamera, visitedMaskBuffer);
    //     }
    //     if(castRay(player.x, player.y, i, y2, f, 0, &c)){
    //         renderLine(97, player.x, player.y, c.x, c.y, &mainCamera, visitedMaskBuffer);
    //     }
    // }
    // for(int i = mainCamera.y; i < y2; i++){
    //     if(castRay(player.x, player.y, mainCamera.x, i, f, 0, &c)){
    //         renderLine(97, player.x, player.y, c.x, c.y, &mainCamera, visitedMaskBuffer);
    //     }
    //     if(castRay(player.x, player.y, x2, i, f, 0, &c)){
    //         renderLine(97, player.x, player.y, c.x, c.y, &mainCamera, visitedMaskBuffer);
    //     }
    // }
    FOR(i, rays){
        castRayWithDirAndDraw(player.x, player.y, cos(PI * i / (rays / 2)), sin(PI * i / (rays / 2)), f, 5, f->visionMesh);
    }
    // for(int i = mainCamera.x; i < x2; i++){
    //     castRayAndDraw(player.x, player.y, i, mainCamera.y, f, 0, f->visionMesh);
    //     castRayAndDraw(player.x, player.y, i, y2, f, 0, f->visionMesh);
    // }
    // for(int i = mainCamera.y; i < y2; i++){
    //     castRayAndDraw(player.x, player.y, mainCamera.x, i, f, 0, f->visionMesh);
    //     castRayAndDraw(player.x, player.y, x2, i, f, 0, f->visionMesh);
    // }
}
void searchForHidden(ItemBase* o){
    void** tmpPtr;
    ItemBase* tmpItemBase;
    tmpPtr = floors[player.z].itemList->data;
    while(tmpPtr){
        tmpItemBase = tmpPtr[1];
        tmpPtr = *tmpPtr;

        if(tmpItemBase->hidden && isPlayerNextTo(tmpItemBase)){
            tmpItemBase->hidden = 0;
            addMessage(writeLog("found a hidden %s", tmpItemBase->name));
            floors[player.z].groundMesh->data[tmpItemBase->y][tmpItemBase->x] = '.';
        }
    }
    updateWorld(0, 0);
}
void updateWorld(int x, int y){
    void** tmpPtr;
    ItemBase* tmpItemBase;
    Effect* tmpEffect;
    Interaction* tmpInteracion;
    TextWidget* tmpInteractionText;
    static int moves = 0;
    int moveValid = 1;

    for(void** i = playerActionList.data; i ;){
        tmpInteracion = i[1];
        i = i[0];
        deleteInteraction(tmpInteracion);
    }
    emptyLinkedList(&playerActionList);

    if((x || y) && (x!= 100)){
        player.x += x;
        player.y += y;


        if((floors[player.z].groundMesh->data[player.y][player.x] != '.')){
            moveValid = 0;
        }

        for(void** i = floors[player.z].itemList->data; i && !gameEnded; ){
            tmpItemBase = i[1];
            i = i[0];

            if((player.x == tmpItemBase->x) && (player.y == tmpItemBase->y)){
                if(tmpItemBase->playerCollision) tmpItemBase->playerCollision(tmpItemBase);
                if(tmpItemBase->collider){
                    moveValid = 0;
                }
            }
        }
        moveValid = moveValid || gameSettings.noClip;
    }
    if(gameEnded){
        return;
    }

    if((moveValid && (y || x)) || (x == 100)){
        moves++;
    }else{
        player.x -= x;
        player.y -= y;
    }

    if(moves >= player.speed){
        globalTime++;
        deltaTime = 1;
        moves = 0;
    }else{
        deltaTime = 0;
    }

    resetPlayerStats(&player);

    //drawCircleOnCharTexture(floors[player.z].visited, player.x, player.y, player.visionRadius, 1);


    for(void** i = player.effects.data; i ;){
        tmpEffect = i[1];
        i = i[0];
        if(tmpEffect->duration == 0){
            tmpEffect->func(tmpEffect);
            deleteEffect(tmpEffect);
            removeItemFromLinkedList(&(player.effects), tmpEffect);
            updateEffectsTab();
        }else{
            tmpEffect->elapsed += deltaTime;
            if(tmpEffect->duration <= tmpEffect->elapsed){
                deleteEffect(tmpEffect);
                removeItemFromLinkedList(&(player.effects), tmpEffect);
                updateEffectsTab();
            }else{
                tmpEffect->func(tmpEffect);
            }
        }
    }

    updatePlayerStats(&player);

    if(player.health <= 0){
        endGame(0, "hoho");
    }
    if(deltaTime){
        player.hungerCounter++;
        if(player.hungerCounter >= player.hungerTime){
            if(player.fullness != 0){
                if(player.fullness == 1) addFormattedMessage("Starving, you'll %olose%O health", 5, 1, 0, 5, 0, 0);
                player.fullness--;
            }
            player.hungerCounter = 0;
        }
        player.healthRegenCounter++;
        if(player.healthRegenCounter >= player.healthRegenTime){
            if(player.fullness / (float)player.baseFullness > 0.7){
                player.health += player.healthRegenAmount;
                player.health = min(player.maxHealth * player.healthModifier, player.health);
            }else if(player.fullness == 0){
                player.health -= 2;
                if(player.health <= 0){
                    endGame(0, writeLog("Died of starvation"));
                    return;
                }
            }
            player.healthRegenCounter = 0;
        }        
    }

    addInteraction("search for hidden doors and traps", &searchForHidden, 'f', NULL);

    tmpPtr = floors[player.z].itemList->data;
    while(tmpPtr && !gameEnded){
        tmpItemBase = tmpPtr[1];
        tmpPtr = *tmpPtr;
        if(tmpItemBase->update)tmpItemBase->update(tmpItemBase);
    }
    tmpPtr = player.items.data;
    while(tmpPtr && !gameEnded){
        tmpItemBase = tmpPtr[1];
        tmpPtr = *tmpPtr;
        if(tmpItemBase->update)tmpItemBase->update(tmpItemBase);
    }
    if(gameEnded) return;


    updateInteractionsWidget();
    checkEquiped();
}
void playerKeyPress(int ch){
    static Interaction* tmpIntracion;
    switch(ch){
        case 'o':
            mgToggleExitMenu();
            break;
        case 'w':
            updateWorld(0, -1);
            break;
        case 'd':
            updateWorld(1, 0);
            break;
        case 'a':
            updateWorld(-1, 0);
            break;
        case 's':
            updateWorld(0, 1);
            break;
        case 'e':
            if(player.heldObject && player.heldObject->primaryUse){
                player.heldObject->primaryUse(player.heldObject);
            }
            updateWorld(100, 0);
            break;
        case 'q':
            if(player.heldObject && player.heldObject->primaryUse){
                player.heldObject->secondaryUse(player.heldObject);
            }
            updateWorld(100, 0);
            break;
        case 'u':
            if(player.z != 0){
                player.z--;
                player.x = floors[player.z].roomList[0]->x + floors[player.z].roomList[0]->w / 2;
                player.y = floors[player.z].roomList[0]->y + floors[player.z].roomList[0]->h / 2;
            }
            break;
        case 'j':
            if(player.z != floorNum-1){
                player.z++;
                player.x = floors[player.z].roomList[0]->x + floors[player.z].roomList[0]->w / 2;
                player.y = floors[player.z].roomList[0]->y + floors[player.z].roomList[0]->h / 2;
            }
            break;
        case 'i':
            mgToggleStatMenu();
            break;
        case 't':
            mgtoggleDegbugMenu();
            break;
    }

    int update = 0;
    for(void** i = playerActionList.data; i;){
        tmpIntracion = i[1];
        i = i[0];
        if(ch == tmpIntracion->key){
            update = 1;
            tmpIntracion->func(tmpIntracion->object);
        }
    }
    if(update) updateWorld(0, 0);
}

void updateMainGame(){
    int ch = getch();
    
    switch(ch){
        case KEY_RESIZE:
            getmaxyx(stdscr, scrH, scrW);
            clear();
            refresh();

            mainCamera.h = scrH;
            mainCamera.w = scrW;

            resizeCharTexture(&frameBuffer, mainCamera.w, mainCamera.h);
            resizeCharTexture(&visitedMaskBuffer, mainCamera.w, mainCamera.h);
            break;
        case KEY_MOUSE:
            if(getmouse(&mEvent) == OK){
                switch(mEvent.bstate){
                case KEY_MOUSE_MOVE:
                    uiMouseMove();
                    break;
                default:
                    uiMouseClick();
                    break;
                }
            }
            break;
        case 'k':
            rays += 100;
            break;
        case 'l':
            rays -= 100;
            break;  
        case ERR:
            break;
        default:
            if(!uiKeyPress(ch)){
                playerKeyPress(ch);
            }
            break;
            
    }
}

void renderMainGameToFramebuffer(){
    fillCharTexture(visitedMaskBuffer, 0);

    mainCamera.x = player.x-mainCamera.w/2;
    mainCamera.y = player.y-mainCamera.h/2;

    updateVisionMesh();
    
    renderTexture(floors[player.z].groundMesh, 0, 0, &mainCamera, frameBuffer);
    mixTextures(floors[player.z].visited, floors[player.z].visionMesh);
    renderTexture(floors[player.z].visionMesh, 0, 0, &mainCamera, visitedMaskBuffer);

    void** tmpPtr = floors[player.z].itemList->data;
    ItemBase* tmp;
    while(tmpPtr){
        tmp = tmpPtr[1];
        tmpPtr = tmpPtr[0];
        if((!tmp->type) || strcmp(tmp->type, "monster")){
            tmp->render(tmp, frameBuffer, &mainCamera);
        }
    }
    tmpPtr = floors[player.z].itemList->data;
    int i = 0;
    while(tmpPtr){
        i++;
        tmp = tmpPtr[1];
        tmpPtr = tmpPtr[0];
        if((tmp->type) && !strcmp(tmp->type, "monster")){
            tmp->render(tmp, frameBuffer, &mainCamera);
        }
    }
    colorMaskFrameBuffer(frameBuffer, visitedMaskBuffer);
    renderTexture(floors[player.z].visited, 0, 0, &mainCamera, visitedMaskBuffer);
    if(!gameSettings.debugSeeAll) maskFrameBuffer(frameBuffer, visitedMaskBuffer);

}
void renderMainGameToTerminal(){
    erase();
    updateUi();

    renderFrameBuffer(frameBuffer);

    mvprintw(scrH/2, scrW/2, "@");

    renderUi();

    refresh();

    fillCharTexture(frameBuffer, ' ');
}
void renderMainGame(){
    erase();
    updateUi();
    
    renderMainGameToFramebuffer();
    

    renderFrameBuffer(frameBuffer);

    mvprintw(scrH/2, scrW/2, "@");

    renderUi();

    refresh();

    fillCharTexture(frameBuffer, ' ');
}


void exitMainGame(){
}
int terminalGetInt(){
    mgTerminalTextBox.focused = 1;
    renderMainGame();

    int key;
    while(1){
        key = getch();
        if(key == KEY_RESIZE){
            getmaxyx(stdscr, scrH, scrW);
            clear();
            refresh();

            mainCamera.h = scrH;
            mainCamera.w = scrW;

            resizeCharTexture(&frameBuffer, mainCamera.w, mainCamera.h);
            resizeCharTexture(&visitedMaskBuffer, mainCamera.w, mainCamera.h);
        }else if(key == '\n'){
            if(sscanf(mgTerminalInput ,"%d", &key) == 1){
                resetTextbox(&mgTerminalTextBox);
                mgTerminalTextBox.focused = 0;
                return key;
            }else{
                resetTextbox(&mgTerminalTextBox);
                mgTerminalTextBox.focused = 0;
                return 0;
            }
            
        }else{
            TBKeyPressCb(&mgTerminalTextBox, key);
        }
        renderMainGame();
    }
}