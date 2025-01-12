#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "MainGame.h"
#include "MainMenu.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../GameObjects/Items/ItemTypes.h"

#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"
#include "../Utilities/MovingCell.h"

#include "../UiElements/Widget.h"
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
#include "../GameObjects/Items/Weapon.h"
#include "../GameObjects/Items/Ammo.h"
#include "../GameObjects/Items/Key.h"
#include "../GameObjects/Items/Coin.h"
#include "../GameObjects/Items/Potion.h"
#include "../GameObjects/Items/Door.h"
#include "../GameObjects/Items/Stair.h"





void generateMap();

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};

Player player;

Camera mainCamera;
CharTexture* frameBuffer;
CharTexture* visitedMaskBuffer;

Floor* floors;
int floorNum;
LinkedList PathCells;
int cellState = 0;

Widget mgPauseMenu;
Button mgOptionsBtn;
Button mgSaveBtn;
Button mgExitBtn;
Button mgResumeButton;


Widget mgDebugMenu;
Button mgCloseDebugBtn;
CheckBox mgShowPointCloudCb;
CheckBox mgNoClipCb;

Widget mgSidePane;
Widget mgTerminalWidget;
Widget mgStatusWidget;
Widget mgItemWidget;

TabWidget mgTabWidget;
Widget mgStatsTab;
Widget mgWeaponsTab;
Widget mgPotionsTab;
Widget mgFoodTab;

LinkedList mgUiList;

Button* mgButtonList[1] = {&mgCloseDebugBtn};
CheckBox* mgCheckBoxList[2] = {&mgShowPointCloudCb, &mgNoClipCb};

void mgToggleStatMenu(){
    mgStatusWidget.isVisible =! mgStatusWidget.isVisible;
    mgItemWidget.isVisible =! mgItemWidget.isVisible;

    renderMainGame();
}
void openWeaponTab(){
    void** tmpIterPtr;
    {
        UiBase* iterPtr;
        tmpIterPtr = mgWeaponsTab.children->data;
        while(tmpIterPtr){
            iterPtr = tmpIterPtr[1];
            tmpIterPtr = tmpIterPtr[0];
            iterPtr->delete(iterPtr->object);
        }
    }

    ItemBase* iterPtr;
    tmpIterPtr = player.items.data;
    while(tmpIterPtr){
        iterPtr = tmpIterPtr[1];
        tmpIterPtr = tmpIterPtr[0];
        if(iterPtr->objectType){
            Widget* tmpWidget = malloc(sizeof(Widget));
            TextWidget* tmpTextWidget = malloc(sizeof(TextWidget));
            Button* tmpButton = malloc(sizeof(Button));

            createWidget(tmpWidget, &mgWeaponsTab, RELATIVE, ABSOLUTE, ABSOLUTE, WITH_PARENT, 1, 0, 90, 1, C_BG_BLACK);
            tmpWidget->bordered = 1;
            createTextWidget(tmpTextWidget, tmpWidget, iterPtr->name, ALIGN_LEFT, ABSOLUTE, 0, 0);
            createButton(tmpButton, tmpWidget, "Drop", ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 0, 0, 4);
            linkedListPushBack(tmpWidget->children, tmpButton->uiBase);
            linkedListPushBack(tmpWidget->children, tmpTextWidget->uiBase);
            linkedListPushBack(mgWeaponsTab.children, tmpWidget->uiBase);
        }
    }
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

void tmpAddButton(){
    Button* btn = malloc(sizeof(Button));
    createButton(btn, &mgPotionsTab, "3", RELATIVE, ALIGN_LEFT, WITH_PARENT, 0, 0, 80);
    btn->contextCallback = &deleteButton;
    btn->contextObject = btn;
    btn->bgColor = C_BG_BLACK;
    btn->textAlign = ALIGN_LEFT;
    linkedListPushBack(mgPotionsTab.children, btn->uiBase);
}

void initMainGame(){
    createLinkedList(&mgUiList, sizeof(UiBase*));

    createWidget(&mgDebugMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_GRAY0);
    mgDebugMenu.isVisible = 0;
    mgDebugMenu.layoutType = VERTICAL_LAYOUT;
    mgDebugMenu.layoutPadding = 1;
    mgDebugMenu.bordered = 1;

    createButton(&mgCloseDebugBtn, &mgDebugMenu, "Close", ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 7);
    createCheckBox(&mgShowPointCloudCb, &mgDebugMenu, "Toggle point cloud", &(gameSettings.debugShowPointCloud), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 0, 100);
    createCheckBox(&mgNoClipCb, &mgDebugMenu, "Toggle noclip", &(gameSettings.noClip), RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 3, 5, 100);

    linkedListPushBack(mgDebugMenu.children, mgNoClipCb.uiBase);
    linkedListPushBack(mgDebugMenu.children, mgShowPointCloudCb.uiBase);
    linkedListPushBack(mgDebugMenu.children, mgCloseDebugBtn.uiBase);


    createWidget(&mgPauseMenu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 21, 11, C_BG_GRAY0);
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


    createWidget(&mgSidePane, NULL, ABSOLUTE, RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 0, 30, 100, NULL);
    mgSidePane.layoutType = VERTICAL_LAYOUT;
    mgSidePane.layoutPadding = 0;
    createWidget(&mgStatusWidget, &mgSidePane, RELATIVE, RELATIVE, ALIGN_LEFT, WITH_PARENT, 0, 0, 100, 45, C_BG_BLACK);
    createWidget(&mgItemWidget, &mgSidePane, RELATIVE, RELATIVE, ALIGN_LEFT, WITH_PARENT, 0, 0, 100, 20, C_BG_BLACK);
    createWidget(&mgTerminalWidget, &mgSidePane, RELATIVE, RELATIVE, ALIGN_LEFT, ALIGN_BOTTOM, 0, 0, 100, 35, C_BG_BLACK);
    mgTerminalWidget.bordered = 1;
    mgItemWidget.bordered = 1;
    mgStatusWidget.bordered = 1;

    createTabWidget(&mgTabWidget, &mgStatusWidget, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 1, 1, 100, 100, NULL);
    

    createWidget(&mgStatsTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);
    mgStatsTab.layoutType = VERTICAL_LAYOUT;
    mgStatsTab.bordered = 1;
    mgStatsTab.scrollOn = 1;

    createWidget(&mgWeaponsTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);
    mgWeaponsTab.layoutType = VERTICAL_LAYOUT;
    mgWeaponsTab.scrollOn = 1;


    createWidget(&mgFoodTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ALIGN_LEFT, ALIGN_TOP, 0, 0, 100, 100, NULL);
    mgFoodTab.layoutType = VERTICAL_LAYOUT;
    mgFoodTab.scrollOn = 1;

    createWidget(&mgPotionsTab, mgTabWidget.tabArea, RELATIVE, RELATIVE, ALIGN_LEFT, ALIGN_TOP, 0, 0, 100, 100, NULL);
    mgPotionsTab.layoutType = VERTICAL_LAYOUT;
    mgPotionsTab.scrollOn = 1;

    tabWidgetAddTab(&mgTabWidget, "Stats", &mgStatsTab, NULL);
    tabWidgetAddTab(&mgTabWidget, "Weaopns", &mgWeaponsTab, &openWeaponTab);
    tabWidgetAddTab(&mgTabWidget, "Food", &mgFoodTab, NULL);
    tabWidgetAddTab(&mgTabWidget, "Potions", &mgPotionsTab, NULL);

    linkedListPushBack(mgStatusWidget.children, mgTabWidget.uiBase);

    linkedListPushBack(mgSidePane.children, mgStatusWidget.uiBase);
    linkedListPushBack(mgSidePane.children, mgItemWidget.uiBase);
    linkedListPushBack(mgSidePane.children, mgTerminalWidget.uiBase);

    linkedListPushBack(&mgUiList, mgSidePane.uiBase);
    linkedListPushBack(&mgUiList, mgPauseMenu.uiBase);
    linkedListPushBack(&mgUiList, mgDebugMenu.uiBase);


    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNum = 7;
    gameSettings.minRoomNum = 5;
    gameSettings.maxFloorNum = 6;
    gameSettings.minFloorNum = 4;
    gameSettings.minRoomSize = 6;
    gameSettings.maxRoomSize = 8;
    gameSettings.roomSpread = 1;
    gameSettings.roomThemeProb = malloc(4 * 4);
    gameSettings.roomThemeProb[0] = 0.5;
    gameSettings.roomThemeProb[1] = 0.2;
    gameSettings.roomThemeProb[2] = 0.25;
    gameSettings.roomThemeProb[3] = 0.05;
    gameSettings.roomThemeNum = 4;


    gameSettings.debugMode = 1;
    gameSettings.debugShowPointCloud = 1;
    gameSettings.noClip = 0;

    floorNum = 0;

    createLinkedList(&PathCells, sizeof(MovingCell*));
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
    player.weapon = NULL;
    FOR(i, player.items.size){
        ItemBase* tmp;
        tmp = linkedListGetElement(&(player.items), 0);
        tmp->deleteObject(tmp->object);
        popLinkedList(&(player.items));
    }

    mainCamera.h = scrH;
    mainCamera.w = scrW;
    mainCamera.h = scrH - 6;
    mainCamera.w = scrW - 20;
        
    generateMap();
    player.x = floors[0].roomList[0]->x + 2;
    player.y = floors[0].roomList[0]->y + 2;
    player.z = 0;
    player.visionRadius = 5;
    createLinkedList(&(player.items), sizeof(ItemBase*));

    getmaxyx(stdscr, scrH, scrW);
    frameBuffer = createCharTexture(mainCamera.w, mainCamera.h);
    visitedMaskBuffer = createCharTexture(mainCamera.w, mainCamera.h);

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);
}
int validForItemPosition(int x, int y, Room* r){
    x += r->x;
    y += r->y;
    Floor* f = floors + r->z;
    if(f->groundMesh->data[y][x] == '.'){
        void** tmpPtr = f->itemList->data;
        ItemBase* i;
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
}
void generateLoot(){
    char* lootTableData;
    if(fileToStr("../Data/LootTable.json", &lootTableData)){
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
                int x, y;
                do{
                    x = randBetween(0, f->roomList[f->stairRooms[1]]->w, 0);
                    y = randBetween(0, f->roomList[f->stairRooms[1]]->h, 0);
                }
                while((!validForItemPosition(x, y, f->roomList[f->stairRooms[1]])) && (!validForItemPosition(x, y, nextF->roomList[f->stairRooms[0]])));
                Stair* s1 = malloc(sizeof(Stair));
                Stair* s2 = malloc(sizeof(Stair));
                s1->z = j;
                s1->x = x + f->roomList[f->stairRooms[1]]->x;
                s1->y = y + f->roomList[f->stairRooms[1]]->y;
                s1->dest = s2;
                s1->sprite = '#';
                createStair(s1);
                linkedListPushBack(f->itemList, s1->itemBase);
                s2->z = j+1;
                s2->x = x + nextF->roomList[nextF->stairRooms[0]]->x;
                s2->y = y + nextF->roomList[nextF->stairRooms[0]]->y;
                s2->dest = s1;
                s2->sprite = '#';
                createStair(s2);
                linkedListPushBack(nextF->itemList, s2->itemBase);
            }
            FOR(i, f->roomNum){
                Room* r = f->roomList[i];
                int num = randBetween(cJSON_GetObjectItem(roomLootTable[r->theme], "min")->valueint, cJSON_GetObjectItem(roomLootTable[r->theme], "max")->valueint + 1, i + j);

                cJSON* enteries = cJSON_GetObjectItem(roomLootTable[r->theme], "enteries");
                FOR(z, num){
                    int tmp = chooseWithWeight(enteries);

                    cJSON* entery = cJSON_GetArrayItem(enteries, tmp);

                    ItemBase* gen;
                    cJSON* pool = cJSON_GetObjectItem(entery, "pool");
                    switch(cJSON_GetObjectItem(entery, "type")->valueint){
                        case 0:
                            gen = generateWeapon(pool);
                            break;
                        case 1:
                            gen = generateAmmo(pool);
                            break;
                        case 2:
                            gen = generateCoin(pool);
                            break;
                        case 3:
                            gen = generateKey(pool);
                            break;
                        case TYPE_POTION:
                            gen = generatePotion(pool);
                            break;
                    }
                    gen->y[0] = randBetween(1, r->h, 0);
                    gen->x[0] = randBetween(1, r->w, 0);
                    while(!validForItemPosition(gen->x[0], gen->y[0], r)){
                        gen->y[0] = randBetween(1, r->h, 0);
                        gen->x[0] = randBetween(1, r->w, 0);
                    }
                    gen->y[0] += r->y;
                    gen->x[0] += r->x;
                    gen->z[0] = j;
                    linkedListPushBack(f->itemList, gen);
                }
            }
        }
        cJSON_free(lootTableJson);

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
                if(tmp >= 1) f->groundMesh->data[i][j] = ';';
            }else if(f->groundMesh->data[i][j] == 'D'){
                Door* d = malloc(sizeof(Door));
                d->sprite = '+';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->hidden = 0;
                d->locked = 0;
                createDoor(d);
                linkedListPushBack(f->itemList, d->itemBase);
            }else if(f->groundMesh->data[i][j] == 'H'){
                Door* d = malloc(sizeof(Door));
                d->sprite = '#';
                d->x = j;
                d->y = i;
                d->z = f->index;
                d->hidden = 1;
                d->locked = 0;
                createDoor(d);
                linkedListPushBack(f->itemList, d->itemBase);
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
    for(int i = 1; i < f->h -1 ; i++){
        for(int j = 1; j < f->w -1; j++){
            if(!(f->groundMesh->data[i][j])){
                tmp = 0;
                if(f->groundMesh->data[i+1][j] == '.') tmp++;
                if(f->groundMesh->data[i-1][j] == '.') tmp++;
                if(f->groundMesh->data[i][j+1] == '.') tmp++;
                if(f->groundMesh->data[i][j-1] == '.') tmp++;
                if(tmp >= 3) f->groundMesh->data[i][j] = ';';
            }
        }
    }
    for(int i = 1; i < f->h -1 ; i++){
        for(int j = 1; j < f->w -1; j++){
            if(f->groundMesh->data[i][j] == ';'){
                f->groundMesh->data[i][j] = '.';
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
    while(roomsPlaced < 1000){
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
            if(roomsPlaced > 1000){
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
    }

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
                }else{
                    cell->attr[7] = 0;
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

int moveValid(int x, int y){
    if(gameSettings.noClip) return 1;

    x += player.x;
    y += player.y;

    if((floors[player.z].groundMesh->data[y][x] == '.')){
        return 1;
    }else return 0;
    
}

void updatePlayer(){
    drawCircleOnCharTexture(floors[player.z].visited, player.x, player.y, player.visionRadius, 1);
    void** tmpPtr = floors[player.z].itemList->data;
    void** tmpCopy;
    ItemBase* tmp;
    while(tmpPtr){
        tmp = tmpPtr[1];
        tmpCopy = *tmpPtr;
        tmp->update(tmp);
        tmpPtr = tmpCopy;
    }
}
void movePlayer(int x, int y){
    if(moveValid(x, y)){
        player.x += x;
        player.y += y;
        updatePlayer();
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
            switch(ch){
                case 'o':
                    mgToggleExitMenu();
                    break;
                case 'w':
                    movePlayer(0, -1);
                    break;
                case 'd':
                    movePlayer(1, 0);
                    break;
                case 'a':
                    movePlayer(-1, 0);
                    break;
                case 's':
                    movePlayer(0, 1);
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
                case 'c':
                    if(player.items.size){
                        ItemBase* tmp = linkedListGetElement(&(player.items), 0);
                        tmp->drop(tmp);
                    }
                    break;
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
    
    tmpPtr = player.items.data;
    int j = 0;
    FOR(i, player.items.size){
        tmp = tmpPtr[1];
        if(tmp->objectType == TYPE_AMMO){
            Ammo* a = tmp->object;
            mvprintw(2 + (j++), 2, "%s : %d", a->name, a->quantity);
        }else if(tmp->objectType == TYPE_WEAPON){
            Weapon* w = tmp->object;
            mvprintw(2 + (j++), 2, "%s : %d", w->name, w->quantity);
        }else if(tmp->objectType == TYPE_KEY){
            Key* k = tmp->object;
            mvprintw(2 + (j++), 2, "%s : %d", k->name, k->quantity);
        }else if(tmp->objectType == TYPE_COIN){
            Coin* c = tmp->object;
            mvprintw(2 + (j++), 2, "%s : %d", c->name, c->quantity);
        }
        if(tmpPtr) tmpPtr = *tmpPtr;
    }

    mvprintw(mainCamera.h/2, mainCamera.w/2, "@");

    renderUi();


    refresh();
}

void exitMainGame(){

}