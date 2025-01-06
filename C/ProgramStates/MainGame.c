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

Widget debugMenu;
Widget pauseMenu;
Button mgOptionsBtn;
Button mgSaveBtn;
Button mgExitBtn;
Button mgResumeButton;


Button mgCloseDebugBtn;
CheckBox mgShowPointCloudCb;
CheckBox mgNoClipCb;

LinkedList mgUiList;

Button* mgButtonList[1] = {&mgCloseDebugBtn};
CheckBox* mgCheckBoxList[2] = {&mgShowPointCloudCb, &mgNoClipCb};


void mgToggleExitMenu(){
    pauseMenu.isVisible = !pauseMenu.isVisible;
    debugMenu.isVisible = 0;
}
void mgtoggleDegbugMenu(){
    debugMenu.isVisible = !debugMenu.isVisible;
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



void initMainGame(){
    createLinkedList(&mgUiList, sizeof(UiBase*));

    createWidget(&debugMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_GRAY0);
    debugMenu.isVisible = 0;

    createButton(&mgCloseDebugBtn, &debugMenu, "Close", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 7);
    createCheckBox(&mgShowPointCloudCb, &debugMenu, "Toggle point cloud", &(gameSettings.debugShowPointCloud), RELATIVE, ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 5, 100);
    createCheckBox(&mgNoClipCb, &debugMenu, "Toggle noclip", &(gameSettings.noClip), ABSOLUTE, ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 7, 100);

    createWidget(&pauseMenu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 21, 11, C_BG_GRAY0);
    createButton(&mgResumeButton, &pauseMenu, "Resume", RELATIVE, ALIGN_CENTER, RELATIVE, 0, 2, 80);
    createButton(&mgOptionsBtn, &pauseMenu, "Options", RELATIVE, ALIGN_CENTER, RELATIVE, 0, 4, 80);
    createButton(&mgSaveBtn, &pauseMenu, "Save", RELATIVE, ALIGN_CENTER, RELATIVE, 0, 6, 80);
    createButton(&mgExitBtn, &pauseMenu, "Exit", RELATIVE, ALIGN_CENTER, RELATIVE, 0, 8, 80);
    linkedListPushBack(&mgUiList, debugMenu.uiBase);
    linkedListPushBack(&mgUiList, mgCloseDebugBtn.uiBase);
    linkedListPushBack(&mgUiList, mgShowPointCloudCb.uiBase);
    linkedListPushBack(&mgUiList, mgNoClipCb.uiBase);
    linkedListPushBack(&mgUiList, pauseMenu.uiBase);
    linkedListPushBack(&mgUiList, mgResumeButton.uiBase);
    linkedListPushBack(&mgUiList, mgOptionsBtn.uiBase);
    linkedListPushBack(&mgUiList, mgSaveBtn.uiBase);
    linkedListPushBack(&mgUiList, mgExitBtn.uiBase);

    mgCloseDebugBtn.callBack = &mgtoggleDegbugMenu;
    mgResumeButton.callBack = &mgToggleExitMenu;
    mgExitBtn.callBack = &enterMainMenu;

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
    FOR(i, player.items.size){
        ItemBase* tmp;
        tmp = linkedListGetElement(&(player.items), 0);
        tmp->deleteObject(tmp->object);
        popLinkedList(&(player.items));
    }

    mainCamera.h = scrH;
    mainCamera.w = scrW;
    mainCamera.x = - scrW / 2;
    mainCamera.y = - scrH / 2;


    
    
    generateMap();
    player.x = floors[0].roomList[0]->x + 2;
    player.y = floors[0].roomList[0]->y + 2;
    player.z = 0;
    player.visionRadius = 5;
    createLinkedList(&(player.items), sizeof(ItemBase*));

    getmaxyx(stdscr, scrH, scrW);
    frameBuffer = createCharTexture(scrW, scrH);
    visitedMaskBuffer = createCharTexture(scrW, scrH);

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);

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
            FOR(i, floors[j].roomNum){
                Room* r = floors[j].roomList[i];
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
                    gen->y[0] = r->y;
                    gen->x[0] = r->x + z;
                    gen->z[0] = j;
                    linkedListPushBack(floors[j].itemList, gen);
                }
            }
        }
        cJSON_free(lootTableJson);

    }
}
void postProccessFloor(Floor* f){
    int tmp = 0;
    for(int i = 1; i < f->maxy - f->miny -1 ; i++){
        for(int j = 1; j < f->maxx - f->minx -1; j++){
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
                d->x = f->minx + j;
                d->y = f->miny + i;
                d->z = f->index;
                d->hidden = 0;
                d->locked = 0;
                createDoor(d);
                linkedListPushBack(f->itemList, d->itemBase);
            }
        }
    }
    for(int i = 1; i < f->maxy - f->miny -1 ; i++){
        for(int j = 1; j < f->maxx - f->minx -1; j++){
            if(f->groundMesh->data[i][j] == ';'){
                f->groundMesh->data[i][j] = '.';
            }
        }
    }
    for(int i = 1; i < f->maxy - f->miny -1 ; i++){
        for(int j = 1; j < f->maxx - f->minx -1; j++){
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
    for(int i = 1; i < f->maxy - f->miny -1 ; i++){
        for(int j = 1; j < f->maxx - f->minx -1; j++){
            if(f->groundMesh->data[i][j] == ';'){
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

    f->rootRoom = createRoomGraph(NULL, NULL,NULL, gameSettings.minRoomNum, gameSettings.maxRoomNum, 0.5, 0);
    countRooms(f->rootRoom, f);
    f->pointCloud = malloc(sizeof(Point*) * f->roomNum);
    f->roomList = malloc(sizeof(Room*) * f->roomNum);
    f->roomNum = -1;
    parseRoomGraph(f->rootRoom, f,  0, 0);
    f->roomNum++;
    createAdjMat(f);
    

    
    int roomsPlaced = 0;
    while(1){
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
        if(roomsPlaced > 1000){
            break;
        }else{
            z = 1;

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

            f->rootRoom = createRoomGraph(NULL, NULL,NULL, gameSettings.minRoomNum, gameSettings.maxRoomNum, 0.7, 0);
            countRooms(f->rootRoom, f);
            f->pointCloud = malloc(sizeof(Point*) * f->roomNum);
            f->roomList = malloc(sizeof(Room*) * f->roomNum);
            f->roomNum = -1;
            parseRoomGraph(f->rootRoom,f,  0, 0);
            f->roomNum++;
            createAdjMat(f);
        }
    }


    float lastRatio = 1;
    FOR(i, f->roomNum){
        f->roomList[i]->w = randBetween(gameSettings.minRoomSize, 2 * ceil(sqrt(pow(f->pointCloud[i]->radius, 2) - pow(gameSettings.minRoomSize / 2,2)))-1,0);
        f->roomList[i]->h = 2 * floor(sqrt(pow(f->pointCloud[i]->radius, 2) - pow(f->roomList[i]->w / 2, 2)));
        
        if((lastRatio - 1) * ((f->roomList[i]->w / f->roomList[i]->h) - 1) > 0){
            int tmp = f->roomList[i]->h;
            f->roomList[i]->h = f->roomList[i]->w;
            f->roomList[i]->w = tmp;
        }
        lastRatio = f->roomList[i]->w / f->roomList[i]->h;

        f->roomList[i]->x = 2 * f->pointCloud[i]->x- f->roomList[i]->w/2;
        f->roomList[i]->y = f->pointCloud[i]->y- f->roomList[i]->h/2;
        f->roomList[i]->theme = randIndexWithProb(gameSettings.roomThemeNum, gameSettings.roomThemeProb,0);
    }

    f->minx = 0; f->miny = 0; f->maxx = 0; f->maxy = 0;
    FOR(i, f->roomNum){
        if(!(f->minx)) f->minx = f->roomList[i]->x;
        else f->minx = min(f->roomList[i]->x, f->minx);
        if(!(f->miny)) f->miny = f->roomList[i]->y;
        else f->miny = min(f->roomList[i]->y, f->miny);
        f->maxx = max(f->roomList[i]->x + f->roomList[i]->w - 1, f->maxx);
        f->maxy = max(f->roomList[i]->y + f->roomList[i]->h - 1, f->maxy);
    }


    f->minx -= 5;
    f->maxx += 5;
    f->miny -= 5;
    f->maxy += 5;
    f->groundMesh = createCharTexture(f->maxx - f->minx, f->maxy - f->miny);
    f->featureMesh = createCharTexture(f->maxx - f->minx, f->maxy - f->miny);

    FOR(i, f->roomNum){
        for(int j = f->roomList[i]->x - f->minx - 1; j <= f->roomList[i]->x - f->minx + f->roomList[i]->w ; j++){
            for(int z = f->roomList[i]->y - f->miny - 1; z <= f->roomList[i]->y - f->miny + f->roomList[i]->h; z++){
               f->groundMesh->data[z][j] = '#';
               f->featureMesh->data[z][j] = i + 1;
            }
        }
        f->featureMesh->data[f->roomList[i]->y - f->miny - 1][f->roomList[i]->x - f->minx - 1] = 0;
        f->featureMesh->data[f->roomList[i]->y - f->miny - 1][f->roomList[i]->x - f->minx + f->roomList[i]->w ] = 0;
        f->featureMesh->data[f->roomList[i]->y - f->miny + f->roomList[i]->h][f->roomList[i]->x - f->minx - 1] = 0;
        f->featureMesh->data[f->roomList[i]->y - f->miny + f->roomList[i]->h][f->roomList[i]->x - f->minx + f->roomList[i]->w ] = 0;


        for(int j = f->roomList[i]->x - f->minx; j < f->roomList[i]->x - f->minx + f->roomList[i]->w; j++){
            for(int z = f->roomList[i]->y - f->miny; z < f->roomList[i]->y - f->miny + f->roomList[i]->h; z++){
               f->groundMesh->data[z][j] = '.';
               f->featureMesh->data[z][j] = 0;

            }
        }
        // for(int j = f->roomList[i]->x - f->minx - 1; j <= f->roomList[i]->x - f->minx + f->roomList[i]->w; j++){
        //     f->groundMesh->data[f->roomList[i]->y - 1 - f->miny][j] = '#';
        //     f->groundMesh->data[f->roomList[i]->y + f->roomList[i]->h - f->miny][j] = '#';
        // }
        // for(int j = f->roomList[i]->y - f->miny - 1; j <= f->roomList[i]->y - f->miny + f->roomList[i]->h; j++){
        //     f->groundMesh->data[j][f->roomList[i]->x - 1 - f->minx] = '#';
        //     f->groundMesh->data[j][f->roomList[i]->x + f->roomList[i]->w - f->minx] = '#';
        // }
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
                cell->x = f->roomList[i]->x+ f->roomList[i]->w/2 - f->minx;
                cell->y = f->roomList[i]->y+ f->roomList[i]->h/2 - f->miny;
                cell->type = 0;
                cell->attr[0] = f->roomList[j]->x+ f->roomList[j]->w/2 - f->minx;
                cell->attr[1] = f->roomList[j]->y+ f->roomList[j]->h/2 - f->miny;
                cell->attr[2] = 0;
                cell->attr[3] = randBetween(0, 2, 0);
                cell->attr[4] = j+1;
                cell->attr[5] = i+1;
                cell->attr[6] = 0;
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

    if((floors[player.z].groundMesh->data[y-floors[player.z].miny][x-floors[player.z].minx] == '.') || (floors[player.z].groundMesh->data[y-floors[player.z].miny][x-floors[player.z].minx] == '#')){
        return 1;
    }else return 0;
    
}

void updatePlayer(){
    drawCircleOnCharTexture(floors[player.z].visited, player.x - floors[player.z].minx, player.y - floors[player.z].miny, player.visionRadius, 1);
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

            resizeCharTexture(&frameBuffer, scrW, scrH);
            resizeCharTexture(&visitedMaskBuffer, scrW, scrH);
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
                case 't':
                    mgtoggleDegbugMenu();
                    break;  
            }
            break;
            
    }
}

void renderMainGame(){
    erase();
    fillCharTexture(frameBuffer, ' ');
    fillCharTexture(visitedMaskBuffer, 0);

    mainCamera.x = player.x-mainCamera.w/2;
    mainCamera.y = player.y-mainCamera.h/2;

    
    mrenderTexture(floors[player.z].groundMesh, NULL, floors[player.z].minx, floors[player.z].miny, frameBuffer, NULL);
    mrenderTexture(floors[player.z].visited, NULL, floors[player.z].minx, floors[player.z].miny, visitedMaskBuffer, NULL);

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

    mvprintw(scrH/2, scrW/2, "@");

    renderUi();


    refresh();
}

void exitMainGame(){

}