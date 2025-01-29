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

void generateMap();

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};

Player player;
LinkedList playerActionList;

Camera mainCamera;
CharTexture* frameBuffer;
CharTexture* visitedMaskBuffer;

Floor* floors;
int floorNum;
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
    player.hungerTime = cJSON_GetObjectItem(playerJ, "hunger time")->valueint;
    player.healthRegenTime = cJSON_GetObjectItem(playerJ, "health regen time")->valueint;
    player.baseHealthRegenAmount = cJSON_GetObjectItem(playerJ, "health regen amount")->valueint;
    
    
    cJSON_Delete(json);


    mainCamera.h = scrH;
    mainCamera.w = scrW;
    frameBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    visitedMaskBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    updateWorld(0, 0);

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


    createWidget(&mgPauseMenu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 21, 11, C_BG_GRAY0);
    {
        mgPauseMenu.isVisible = 0;
        mgPauseMenu.bordered = 1;
        mgPauseMenu.layoutType = VERTICAL_LAYOUT;
        mgPauseMenu.layoutPadding = 1;

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
                    createTextWidget(&healthTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Health: %d / %d", &(player.health), &(player.maxHealth));
                    createTextWidget(&goldTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Golds: %d", &(player.totalGold));
                    createTextWidget(&totalMovesTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Total moves: %d", &globalTime);
                    createTextWidget(&fullnessTextWidget, &mgStatsTab, ALIGN_LEFT, WITH_PARENT, 0, 0, "Fullness: %d / %d", &player.fullness, &player.baseFullness);

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
    linkedListPushBack(&mgUiList, mgPauseMenu.uiBase);
    linkedListPushBack(&mgUiList, mgDebugMenu.uiBase);
    linkedListPushBack(&mgUiList, mgEquipedWidget.uiBase);


    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNum = 7;
    gameSettings.minRoomNum = 5;
    gameSettings.maxFloorNum = 6;
    gameSettings.minFloorNum = 4;
    gameSettings.minRoomSize = 4;
    gameSettings.maxRoomSize = 12;
    gameSettings.roomSpread = 1;
    gameSettings.roomThemeProb = malloc(4 * 4);
    gameSettings.roomThemeProb[0] = 0.5;
    gameSettings.roomThemeProb[1] = 0.2;
    gameSettings.roomThemeProb[2] = 0.25;
    gameSettings.roomThemeProb[3] = 0.05;
    gameSettings.roomThemeNum = 4;
    
    gameSettings.baseMaxHealth = 100;
    gameSettings.baseHealthRegenTime = 5;
    gameSettings.baseHungerTime = 10;
    gameSettings.baseSpeed = 1;
    gameSettings.baseStrength = 1;
    gameSettings.maxFullness = 20;
    gameSettings.debugMode = 1;
    gameSettings.debugShowPointCloud = 1;
    gameSettings.noClip = 1;
    gameSettings.debugItemInfo = 0;
    gameSettings.debugSeeAll = 1;

    floorNum = 0;

    createLinkedList(&PathCells, sizeof(MovingCell*));
    createLinkedList(&messages, sizeof(TextWidget*));

    createLinkedList(&(player.items), sizeof(ItemBase*));
    createLinkedList(&(player.effects), sizeof(Effect*));
    createLinkedList(&(playerActionList), sizeof(Interaction*));
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
void setFloorStairRooms(Floor* f){
    int k = 0;
    FOR(i, f->roomNum){
        if(f->roomList[i]->stairCandidate) f->stairRooms[k++] = i;
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
    player.healthRegenTime = gameSettings.baseHealthRegenTime;
    player.baseHealthRegenAmount = 1;
    player.hungerTime = gameSettings.baseHungerTime;
    player.baseSpeed = gameSettings.baseSpeed;
    player.baseStrength = gameSettings.baseStrength;
    player.health = 0;
    player.fullness = gameSettings.maxFullness;
    player.baseFullness = gameSettings.maxFullness;
    player.speedModifier = 1;
    player.healthRegenModifier = 1;
    player.healthModifier = 1;
    player.strengthModifier = 1;
    globalItemIdCounter = 1;
}
void enterMainGame(){
    clear();
    refresh();
    resetGame();
        
    generateMap();

    player.z = 0;
    player.x = floors[0].roomList[0]->x+2;
    player.y = floors[0].roomList[0]->y+2;

    getmaxyx(stdscr, scrH, scrW);
    mainCamera.h = scrH;
    mainCamera.w = scrW;
    frameBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    visitedMaskBuffer = createCharTexture(mainCamera.w, mainCamera.h);

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);
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
void generateLoot(){
    char* lootTableData;
    char* itemDb;
    if(fileToStr("../Data/LootTable.json", &lootTableData) && fileToStr("../Data/Items.json", &itemDb)){
        cJSON* lootTableJson = cJSON_Parse(lootTableData);

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
                        gen->x = randBetween(r->x, r->w + r->x, 0);
                        gen->y = randBetween(r->y, r->h  + r->y, 0);
                        while(!validForItemPosition(gen->x, gen->y, j)){
                            gen->x = randBetween(r->x, r->w + r->x, 0);
                            gen->y = randBetween(r->y, r->h  + r->y, 0);
                        }
                        gen->z = j;
                        if(r->theme == 2) gen->fake = 1;
                        linkedListPushBack(f->itemList, gen);

                    }
                     
                }
            }
        }
        cJSON_free(lootTableJson);
        free(lootTableData);
        free(itemDb);
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
                d->sprite = '+';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 0;
                d->color[1] = 5;
                d->color[0] = 5;
                d->color[2] = 5;
                linkedListPushBack(f->itemList, d);
                f->groundMesh->data[i][j] = ';';
            }else if(f->groundMesh->data[i][j] == 'L'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->sprite = '$';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 1;
                d->collider = 1;
                d->color[1] = 0;
                d->color[0] = 5;
                d->color[2] = 1;
                linkedListPushBack(f->itemList, d);
                f->groundMesh->data[i][j] = ';';

                ItemBase* p = calloc(1, sizeof(ItemBase));
                initPasswordGenerator(p);
                p->sprite = '&';
                p->relId = d->id;
                p->x = j + 2;
                p->y = i + 2;
                p->z = f->index;
                p->collider = 1;
                p->color[1] = 5;
                p->color[0] = 5;
                p->color[2] = 5;
                linkedListPushBack(f->itemList, p);
            }else if(f->groundMesh->data[i][j] == 'H'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->sprite = '#';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 0;
                d->color[1] = 5;
                d->color[0] = 5;
                d->color[2] = 5;
                linkedListPushBack(f->itemList, d);
                f->groundMesh->data[i][j] = ';';
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

        int z = 1;

        FOR(i, 4000){
            if(iteratePointCloud(f->pointCloud, f->adjMat, f->roomNum, gameSettings.roomSpread)){
                roomsPlaced++;
            }
            if(roomsPlaced > 200){
                break;
            }
            // if(z){
            // getmaxyx(stdscr, scrH, scrW);
            // erase();
            // FOR(j, f->roomNum){
            //     mvprintw(j, 0, "%.2f %.2f", f->pointCloud[j]->x, f->pointCloud[j]->y);
            //     mvaddch(f->pointCloud[j]->y + scrH/2, f->pointCloud[j]->x + scrW/2, '0');
            // }mvprintw(f->roomNum, 0, "%d", roomsPlaced);
            // refresh();
            //     int ch;

            // while(1){
            //     ch = getch();
            //     if(ch=='l') break;
            //     if(ch == 'p'){
            //         z = 0;
            //         break; 
            //     }
            // }

            // }
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

        r->x = 2 * f->pointCloud[i]->x- r->w/2;
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
               f->groundMesh->color[z][j] = rgb[3][3][3];
               f->featureMesh->data[z][j] = i + 1;
            }
        }
        f->featureMesh->data[f->roomList[i]->y - 1][f->roomList[i]->x - 1] = 0;
        f->featureMesh->data[f->roomList[i]->y - 1][f->roomList[i]->x + f->roomList[i]->w ] = 0;
        f->featureMesh->data[f->roomList[i]->y + f->roomList[i]->h][f->roomList[i]->x - 1] = 0;
        f->featureMesh->data[f->roomList[i]->y + f->roomList[i]->h][f->roomList[i]->x + f->roomList[i]->w ] = 0;

        for(int j = f->roomList[i]->x; j < f->roomList[i]->x + f->roomList[i]->w; j++){
            for(int z = f->roomList[i]->y; z < f->roomList[i]->y + f->roomList[i]->h; z++){
               f->groundMesh->data[z][j] = '.';
               f->groundMesh->color[z][j] = rgb[1][1][1];
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

                cell = malloc(sizeof(MovingCell));
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
    f->visited = createCharTexture(f->groundMesh->w, f->groundMesh->h);
    fillCharTexture(f->visited, 0);
}
void generateMap(){
    floorNum = randBetween(gameSettings.minFloorNum, gameSettings.maxFloorNum + 1, 0);
    floors = malloc(floorNum * sizeof(Floor));
    
    FOR(i, floorNum){
        floors[i].index = i;
        generateFloor(floors + i);
    }

    generateLoot();
}
void updateWorld(int x, int y){
    void** tmpPtr;
    ItemBase* tmpItemBase;
    Effect* tmpEffect;
    Interaction* tmpInteracion;
    TextWidget* tmpInteractionText;
    static int moves = 0;

    for(void** i = playerActionList.data; i ;){
        tmpInteracion = i[1];
        i = i[0];
        deleteInteraction(tmpInteracion);
    }
    emptyLinkedList(&playerActionList);

    player.x += x;
    player.y += y;
    int moveValid = 1;

    if((floors[player.z].groundMesh->data[player.y][player.x] != '.')){
        moveValid = 0;
    }

    for(void** i = floors[player.z].itemList->data; i; ){
        tmpItemBase = i[1];
        i = i[0];

        if((player.x == tmpItemBase->x) && (player.y == tmpItemBase->y)){
            tmpItemBase->playerCollision(tmpItemBase);
            if(tmpItemBase->collider){
                moveValid = 0;
            }
        }
    }
    moveValid = moveValid || gameSettings.noClip;

    if(moveValid && (y || x)){
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
    drawCircleOnCharTexture(floors[player.z].visited, player.x, player.y, player.visionRadius, 1);

    player.speedModifier = 1;
    player.healthRegenModifier = 1;
    player.healthModifier = 1;
    player.strengthModifier = 1;

    for(void** i = player.effects.data; i ;){
        tmpEffect = i[1];
        i = i[0];
        tmpEffect->elapsed += deltaTime;
        tmpEffect->func(tmpEffect);
        if(tmpEffect->duration <= tmpEffect->elapsed){
            deleteEffect(tmpEffect);
            removeItemFromLinkedList(&(player.effects), tmpEffect);
            updateEffectsTab();
        }
    }

    updatePlayerStats(&player);

    if(deltaTime){
        player.hungerCounter++;
        if(player.hungerCounter >= player.hungerTime){
            if(player.fullness != 0){
                if(player.fullness == 1) addFormattedMessage("You are %otoo%O hungry, you'll %olose%O health", 5, 1, 0, 5, 0, 0);
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
            }
            player.healthRegenCounter = 0;
        }        
    }

    tmpPtr = floors[player.z].itemList->data;
    while(tmpPtr){
        tmpItemBase = tmpPtr[1];
        tmpPtr = *tmpPtr;
        if(tmpItemBase->update)tmpItemBase->update(tmpItemBase);
    }
    tmpPtr = player.items.data;
    while(tmpPtr){
        tmpItemBase = tmpPtr[1];
        tmpPtr = *tmpPtr;
        if(tmpItemBase->update)tmpItemBase->update(tmpItemBase);
    }

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
            break;
        case 'q':
            if(player.heldObject && player.heldObject->primaryUse){
                player.heldObject->secondaryUse(player.heldObject);
            }
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
        case ERR:
            break;
        default:
            if(!uiKeyPress(ch)){
                playerKeyPress(ch);
            }
            switch(ch){
                case 'i':
                    mgToggleStatMenu();
                    break;
                case 't':
                    mgtoggleDegbugMenu();
                    break;  
            }
            break;
            
    }
}

void renderMainGame(){
    erase();
    updateUi();
    fillCharTexture(frameBuffer, ' ');
    fillCharTexture(visitedMaskBuffer, 0);

    mainCamera.x = player.x-mainCamera.w/2;
    mainCamera.y = player.y-mainCamera.h/2;

    
    renderTexture(floors[player.z].groundMesh, 0, 0, &mainCamera, frameBuffer);
    
    renderTexture(floors[player.z].visited, 0, 0, &mainCamera, visitedMaskBuffer);

    void** tmpPtr = floors[player.z].itemList->data;
    ItemBase* tmp;
    FOR(i, floors[player.z].itemList->size){
        tmp = tmpPtr[1];
        tmp->render(tmp, frameBuffer, &mainCamera);
        if(tmpPtr) tmpPtr = *tmpPtr;
    }
    if(!gameSettings.debugSeeAll) maskFrameBuffer(frameBuffer, visitedMaskBuffer);

    renderFrameBuffer(frameBuffer);

    mvprintw(scrH/2, scrW/2, "@");

    renderUi();

    refresh();
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