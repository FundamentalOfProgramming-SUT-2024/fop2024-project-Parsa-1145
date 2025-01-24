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


Widget mgSidePane;
Widget mgTerminalWidget;
TextBox mgTerminalTextBox;
Widget mgMessagesArea;

Widget mgItemWidget;
Widget mgItemWidgetWrapper;

Widget mgStatusWidget;
TabWidget mgTabWidget;
Widget mgStatsTab;
TextWidget healthTextWidget;
TextWidget goldTextWidget;
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
        if(1){
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
            createTextWidget(tmpTextWidget, tmpWidget, ALIGN_LEFT, ABSOLUTE, 0, 0, "%s(%u) x%d", (iterPtr->name), &(iterPtr->sprite), &(iterPtr->quantity));
            createButton(tmpButton, tmpWidget, "...", ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 0, 0, 3);
            tmpButton->contextObject = iterPtr;
            tmpButton->contextCallback = openItemInfo;
            linkedListPushBack(tmpWidget->children, tmpButton->uiBase);
            linkedListPushBack(tmpWidget->children, tmpTextWidget->uiBase);
            linkedListPushBack(mgInventoryTab.children, tmpWidget->uiBase);
        }
    }
    mgInventoryTab.tmpIterPtr = NULL; //this is here to free up the uibase iterator of the widget.
}
void mgToggleExitMenu(){
    mgPauseMenu.isVisible = !mgPauseMenu.isVisible;
    mgDebugMenu.isVisible = 0;
}
void mgtoggleDegbugMenu(){
    mgDebugMenu.isVisible = !mgDebugMenu.isVisible;
}
void uiMouseMove(){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    while(tmp1){
        tmp = tmp1[1];
        tmp->mouseMove(tmp->object);
        tmp1 = tmp1[0];
    }
}
void uiKeyPress(int key){
    static UiBase* tmp;
    static void** tmp1;
    tmp1 = mgUiList.data;
    while(tmp1){
        tmp = tmp1[1];
        tmp->keyPress(tmp->object, key);
        tmp1 = tmp1[0];
    }
}
void renderUi(){
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

void addMessage(char* message){
    TextWidget* tmp = malloc(sizeof(TextWidget));
    createTextWidget(tmp, &mgMessagesArea, ALIGN_LEFT, WITH_PARENT, 0, 0, message);
    linkedListInsert(mgMessagesArea.children, tmp->uiBase, 0);
    if(mgMessagesArea.children->size == 20){
        tmp = ((UiBase*)linkedListGetElement(mgMessagesArea.children, mgMessagesArea.children->size-1))->object;
        deleteTextWidget(tmp);
        linkedListDeleteElement(mgMessagesArea.children, mgMessagesArea.children->size-1);
    }
    free(message);
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

        linkedListPushBack(mgDebugMenu.children, mgNoClipCb.uiBase);
        linkedListPushBack(mgDebugMenu.children, mgItemDebugInfo.uiBase);
        linkedListPushBack(mgDebugMenu.children, mgShowPointCloudCb.uiBase);

        linkedListPushBack(mgDebugMenu.children, mgCloseDebugBtn.uiBase);
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

        mgCloseDebugBtn.callBack = &mgtoggleDegbugMenu;
        mgResumeButton.callBack = &mgToggleExitMenu;
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

                    linkedListPushBack(mgStatsTab.children, healthTextWidget.uiBase);
                    linkedListPushBack(mgStatsTab.children, goldTextWidget.uiBase);
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
    gameSettings.maxRoomSize = 8;
    gameSettings.roomSpread = 1;
    gameSettings.roomThemeProb = malloc(4 * 4);
    gameSettings.roomThemeProb[0] = 0.5;
    gameSettings.roomThemeProb[1] = 0.2;
    gameSettings.roomThemeProb[2] = 0.25;
    gameSettings.roomThemeProb[3] = 0.05;
    gameSettings.roomThemeNum = 4;
    
    gameSettings.maxHealth = 16;

    gameSettings.debugMode = 1;
    gameSettings.debugShowPointCloud = 1;
    gameSettings.noClip = 0;
    gameSettings.debugItemInfo = 0;

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
        p->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4) + 1, 0);
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
void enterMainGame(){
    clear();
    refresh();
    FOR(i, floorNum){
        deleteFloor(floors + i);
    }
    free(floors);

    player.x = 0;
    player.y = 0;
    player.z = 0;
    player.heldObject = NULL;
    FOR(i, player.items.size){
        ItemBase* tmp;
        tmp = linkedListGetElement(&(player.items), 0);
        tmp->deleteObject(tmp);
        popLinkedList(&(player.items));
    }

        
    generateMap();

    player.x = floors[0].roomList[0]->x + 2;
    player.y = floors[0].roomList[0]->y + 2;
    player.z = 0;
    player.totalGold = 0;
    player.visionRadius = 5;
    player.health = gameSettings.maxHealth;
    player.maxHealth = gameSettings.maxHealth;
    player.speedModifier = 1;
    player.healthRegenModifier = 1;
    player.healthModifier = 1;
    player.strengthModifier = 1;
    player.speed = 1;

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
                linkedListPushBack(f->itemList, s1);
                s2->z = j+1;
                s2->x = x2;
                s2->y = y2;
                s2->sprite = '#';
                linkedListPushBack(nextF->itemList, s2);
                s1->relId = s2->id;
                s2->relId = s1->id;
            }
            FOR(i, f->roomNum){
                Room* r = f->roomList[i];
                int num = randBetween(cJSON_GetObjectItem(roomLootTable[r->theme], "minRolls")->valueint, cJSON_GetObjectItem(roomLootTable[r->theme], "maxRolls")->valueint + 1, i + j);

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
                        gen->x = randBetween(1 + r->x, r->w + r->x-1, 0);
                        gen->y = randBetween(1 + r->y, r->h  + r->y-1, 0);
                        while(!validForItemPosition(gen->x, gen->y, j)){
                            gen->x = randBetween(1 + r->x, r->w + r->x-1, 0);
                            gen->y = randBetween(1 + r->y, r->h  + r->y-1, 0);
                        }
                        gen->z = j;
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
                linkedListPushBack(f->itemList, d);
            }else if(f->groundMesh->data[i][j] == 'L'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->sprite = '$';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 1;
                d->collider = 1;
                linkedListPushBack(f->itemList, d);
            }else if(f->groundMesh->data[i][j] == 'H'){
                ItemBase* d = calloc(1, sizeof(ItemBase));
                initDoor(d);
                d->sprite = '#';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->locked = 0;
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
            }else if(f->groundMesh->data[i][j] == 'D' || f->groundMesh->data[i][j] == 'H'){
                f->groundMesh->data[i][j] = '.';
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
    float lastRatio = 1;
    FOR(i, f->roomNum){
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
        r->theme = randIndexWithProb(gameSettings.roomThemeNum, gameSettings.roomThemeProb,0);
    }
    if(f->index != 0){
        Floor* prevF = floors + f->index -1;
        f->roomList[f->stairRooms[0]]->w = prevF->roomList[prevF->stairRooms[1]]->w;
        f->roomList[f->stairRooms[0]]->h = prevF->roomList[prevF->stairRooms[1]]->h;
    }

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
               f->featureMesh->data[z][j] = 0;

            }
        }
        f->roomList[i]->neighbours = 0;
        if(f->roomList[i]->right) f->roomList[i]->neighbours++;
        if(f->roomList[i]->left) f->roomList[i]->neighbours++;
        if(f->roomList[i]->bottom) f->roomList[i]->neighbours++;
        if(f->roomList[i]->top) f->roomList[i]->neighbours++;
    }

    MovingCell* cell;
    FOR(i, f->roomNum){
        for(int j = i + 1; j < f->roomNum; j++){
            
            if(f->adjMat[i][j]){
                //int x1[2], y1[2], x2[2], y2[2];
                

                // x1[0] = f->roomList[i]->x;
                // x1[1] = f->roomList[i]->x + f->roomList[i]->w - 1;
                // x2[0] = f->roomList[j]->x;
                // x2[1] = f->roomList[j]->x+ f->roomList[j]->w - 1;
                // y1[0] = f->roomList[i]->y;
                // y1[1] = f->roomList[i]->y + f->roomList[i]->h - 1;
                // y2[0] = f->roomList[j]->y;
                // y2[1] = f->roomList[j]->y+ f->roomList[j]->h - 1;
                // int rx, ry, dx, dy;
                // Room* start, end;

                // float xdif = x2[0] - x1[0], ydif = y2[0] - y1[0];
                // float a = atanf(ydif/xdif);
                
                // int xRel = xdif > 0, yRel = ydif > 0;
                // float mainXdif, mainYdif;
                // if(xRel) mainXdif = x2[0] - x1[1];
                // else mainXdif = x1[0] - x2[1];
                // if(yRel) mainYdif = y2[0] - y1[1];
                // else mainYdif = y1[0] - y2[1];

                cell = malloc(sizeof(MovingCell));
                linkedListPushBack(&PathCells, cell);
                cell->x = f->roomList[i]->x+ f->roomList[i]->w/2;
                cell->y = f->roomList[i]->y+ f->roomList[i]->h/2;
                cell->type = 0;
                cell->attr[0] = f->roomList[j]->x+ f->roomList[j]->w/2;
                cell->attr[1] = f->roomList[j]->y+ f->roomList[j]->h/2;
                cell->attr[2] = 0;
                cell->attr[3] = randBetween(0, 2, 0);
                cell->attr[4] = j+1;
                cell->attr[5] = i+1;
                cell->attr[6] = 0;
                if(f->roomList[i]->neighbours == 2 && f->roomList[j]->neighbours == 1){
                    cell->attr[7] = 1;
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

    if(!moveValid){
        player.x -= x;
        player.y -= y;
    }else{
        moves ++;
    }
    if(moves >= getPlayerSpeed(&player)){
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

    if(deltaTime){
        for(void** i = player.effects.data; i ;){
            tmpEffect = i[1];
            i = i[0];

            tmpEffect->elapsed++;
            tmpEffect->func(tmpEffect);
            if(tmpEffect->duration <= tmpEffect->elapsed){
                deleteEffect(tmpEffect);
                removeItemFromLinkedList(&(player.effects), tmpEffect);
                updateEffectsTab();
            }
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

    for(void** i = playerActionList.data; i;){
        tmpIntracion = i[1];
        i = i[0];

        if(ch == tmpIntracion->key){
            tmpIntracion->func(tmpIntracion->object);
        }
    }
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
            uiKeyPress(ch);
            playerKeyPress(ch);
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

    
    mrenderTexture(floors[player.z].groundMesh, NULL, 0, 0, frameBuffer, NULL);
    
    mrenderTexture(floors[player.z].visited, NULL, 0, 0, visitedMaskBuffer, NULL);

    void** tmpPtr = floors[player.z].itemList->data;
    ItemBase* tmp;
    FOR(i, floors[player.z].itemList->size){
        tmp = tmpPtr[1];
        tmp->render(tmp, frameBuffer, NULL, &mainCamera);
        if(tmpPtr) tmpPtr = *tmpPtr;
    }
    //maskFrameBuffer(frameBuffer, NULL, visitedMaskBuffer);

    renderFrameBuffer(frameBuffer);

    mvprintw(scrH/2, scrW/2, "@");

    renderUi();


    refresh();
}

void exitMainGame(){

}