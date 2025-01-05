#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "MainGame.h"
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
Button mgCloseDebugBtn;
CheckBox mgShowPointCloudCb;
CheckBox mgNoClipCb;

Button* mgButtonList[1] = {&mgCloseDebugBtn};
CheckBox* mgCheckBoxList[2] = {&mgShowPointCloudCb, &mgNoClipCb};

Point** pointCloud;
Room** roomList;
int** adjMat;
int pointNum = 0;
int pointPlacement = 0;

void mgtoggleDegbugMenu(){
    debugMenu.isVisible = !debugMenu.isVisible;
}
void uiMouseMove(){
    FOR(i, 1){
        buttonMouseMoveCallback(mgButtonList[i]);
    }
    FOR(i, 2){
        CBMouseMoveCb(mgCheckBoxList[i]);
    }
}
void renderUi(){
    renderWidget(&debugMenu);
    FOR(i, 1){
        renderButton(mgButtonList[i]);
    }
    FOR(i, 2){
        renderCheckBox(mgCheckBoxList[i]);
    }

}
void uiMouseClick(){
    FOR(i, 1){
        buttonMouseClickEvent(mgButtonList[i]);
    }
    FOR(i, 2){
        CBMouseClickCb(mgCheckBoxList[i]);
    }
}



void initMainGame(){

    createWidget(&debugMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_GRAY0);
    debugMenu.layoutType = HORIZONTAL_LAYOUT;
    debugMenu.layoutPadding = 1;
    debugMenu.isVisible = 0;

    createButton(&mgCloseDebugBtn, &debugMenu, "Close", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 7);
    createCheckBox(&mgShowPointCloudCb, &debugMenu, "Toggle point cloud", &(gameSettings.debugShowPointCloud), RELATIVE, ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 100);
    createCheckBox(&mgNoClipCb, &debugMenu, "Toggle noclip", &(gameSettings.noClip), RELATIVE, ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 100);
    linkedListPushBack(debugMenu.children, mgShowPointCloudCb.widget);
    linkedListPushBack(debugMenu.children, mgShowPointCloudCb.widget);
    updateWidgetChildren(&debugMenu);
    mgCloseDebugBtn.callBack = &mgtoggleDegbugMenu;


    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNum = 15;
    gameSettings.minRoomNum = 6;
    gameSettings.maxFloorNum = 6;
    gameSettings.minFloorNum = 4;
    gameSettings.minRoomSize = 6;
    gameSettings.maxRoomSize = 8;
    gameSettings.roomSpread = 4;
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
                tmp = createRoomGraph(branchRoomList[i], 1,  &n,max(1.1,minN / 3), maxN / 3 , branchingProb/3, depth+1);
                if(tmp){
                    branchRoomList[(int)(min(roomNum-0.9, i + n))]->top = tmp;
                    tmp->bottom = branchRoomList[(int)(min(roomNum-0.9, i + n))];
                }
            }else{
                tmp = createRoomGraph(branchRoomList[i], 0, &n, max(1.1,minN / 3), maxN / 3, branchingProb/3, depth+1);
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
            return NULL;
        }
    }else{

        Room* r = branchRoomList[0];
        free(branchRoomList);
        return r;
    }
}
void parseRoomGraph(Room* r,float x, float y){
    if(!(r->visited)){
        Point* p = malloc(sizeof(Point));
        p->x = x;
        p->y = y + randBetween(0, 3, 0) - 6;
        p->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4) + 1, 0);
        p->spread = randBetween(gameSettings.roomSpread, gameSettings.roomSpread * 2, 0);
        p->locked = 0;
        pointCloud = realloc(pointCloud, (++pointNum) * sizeof(Point*));
        pointCloud[pointNum-1] = p;
        r->visited = 1;

        roomList = realloc(roomList, pointNum * sizeof(Room*));
        roomList[pointNum-1] = r;
        r->point = p;
        r->index = pointNum-1;
        if(r->right){
            parseRoomGraph(r->right, x + p->radius, y);
        }if(r->top){
            parseRoomGraph(r->top, x, y + p->radius);
        }if(r->bottom){
            parseRoomGraph(r->bottom, x, y-p->radius);
        }if(r->left){
            parseRoomGraph(r->left, x-p->radius, y);
        }
    }
}
void createAdjMat(){
    adjMat = malloc(pointNum * sizeof(int*));
    FOR(i, pointNum){
        adjMat[i] = calloc(pointNum, sizeof(int));
    }

    FOR(i, pointNum){
        if(roomList[i]->top){
            adjMat[i][roomList[i]->top->index] = 1;
        }if(roomList[i]->right){
            adjMat[i][roomList[i]->right->index] = 1;
        }if(roomList[i]->bottom){
            adjMat[i][roomList[i]->bottom->index] = 1;
        }if(roomList[i]->left){
            adjMat[i][roomList[i]->left->index] = 1;
        }
    }
}
Room* roomRoot;
void enterMainGame(){
    
    clear();
    refresh();


    FOR(i, floorNum){
        FOR(j, floors[i].roomNum){
            deleteRoom(floors[i].roomList[j]);
            deletePoint(floors[i].pointCloud[j]);
        }
        free(floors[i].roomList);
        free(floors[i].pointCloud);
        floors[i].pointCloud = NULL;
        floors[i].roomList = NULL;

        ItemBase* tmp;
        FOR(j, floors[i].itemList->size){
            tmp = linkedListGetElement(floors[i].itemList, 0);
            tmp->deleteObject(tmp->object);
            popLinkedList(floors[i].itemList);
        }
        free(floors[i].itemList);
        floors[i].itemList = NULL;

        free(floors + i);
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

    roomRoot = createRoomGraph(NULL, NULL,NULL, 5, 8, 0.1, 0);
    parseRoomGraph(roomRoot, 0, 0);
    createAdjMat();

    //generateMap();

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);

    frameBuffer = createCharTexture(scrW, scrH);
    visitedMaskBuffer = createCharTexture(scrW, scrH);
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
void generateFloor(int index){
    floors[index].roomNum = randBetween(gameSettings.minRoomNum, gameSettings.maxRoomNum + 1, 1 + index);
    floors[index].pointCloud = malloc(floors[index].roomNum * sizeof(Point*));
    floors[index].roomList = malloc(sizeof(Room*) * floors[index].roomNum);
    floors[index].itemList = malloc(sizeof(LinkedList));
    createLinkedList(floors[index].itemList, sizeof(ItemBase*));

    FOR(i, floors[index].roomNum){
        floors[index].pointCloud[i] = malloc(sizeof(Point));
        floors[index].pointCloud[i]->x = 0;
        floors[index].pointCloud[i]->y = 0;
        floors[index].pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i + index);
        floors[index].pointCloud[i]->locked = 0;
    }

    int roomsPlaced = 0;
    while(roomsPlaced < 1000){
        FOR(i, 40000){
            if(iteratePointCloud(floors[index].pointCloud, adjMat, floors[index].roomNum, gameSettings.roomSpread)){
                roomsPlaced += 1;
                if(roomsPlaced == 1000) break;
            }
            getmaxyx(stdscr, scrH, scrW);
            
            erase();
            FOR(i, floors[index].roomNum){
                pointRender(floors[index].pointCloud[i]);
            }
            refresh();
        }
        if(roomsPlaced < 1000){
            gameSettings.roomSpread+= 5;
            FOR(i, floors[index].roomNum){
                floors[index].pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i + index);
                floors[index].pointCloud[i]->x = 0;
                floors[index].pointCloud[i]->y = 0;
                floors[index].pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i + index);
            }
            roomsPlaced = 0;
        }
    }

    float lastRatio = 1;
    FOR(i, floors[index].roomNum){
        floors[index].roomList[i] = malloc(sizeof(Room));
        floors[index].roomList[i]->w = randBetween(gameSettings.minRoomSize, 2 * ceil(sqrt(pow(floors[index].pointCloud[i]->radius, 2) - pow(gameSettings.minRoomSize / 2,2)))-1, i + index);
        floors[index].roomList[i]->h = 2 * floor(sqrt(pow(floors[index].pointCloud[i]->radius, 2) - pow(floors[index].roomList[i]->w / 2, 2)));
        
        if((lastRatio - 1) * ((floors[index].roomList[i]->w / floors[index].roomList[i]->h) - 1) > 0){
            int tmp = floors[index].roomList[i]->h;
            floors[index].roomList[i]->h = floors[index].roomList[i]->w;
            floors[index].roomList[i]->w = tmp;
        }
        lastRatio = floors[index].roomList[i]->w / floors[index].roomList[i]->h;

        floors[index].roomList[i]->x = 2 * floors[index].pointCloud[i]->x- floors[index].roomList[i]->w/2;
        floors[index].roomList[i]->y = floors[index].pointCloud[i]->y- floors[index].roomList[i]->h/2;
        floors[index].roomList[i]->theme = randIndexWithProb(gameSettings.roomThemeNum, gameSettings.roomThemeProb, i + index);
    }

    floors[index].minx = INT32_MAX; floors[index].miny = INT32_MAX; floors[index].maxx = 0; floors[index].maxy = 0;
    FOR(i, floors[index].roomNum){
        floors[index].minx = min(floors[index].roomList[i]->x, floors[index].minx);
        floors[index].miny = min(floors[index].roomList[i]->y, floors[index].miny);
        floors[index].maxx = max(floors[index].roomList[i]->x + floors[index].roomList[i]->w - 1, floors[index].maxx);
        floors[index].maxy = max(floors[index].roomList[i]->y + floors[index].roomList[i]->h - 1, floors[index].maxy);
    }


    floors[index].minx -= 5;
    floors[index].maxx += 5;
    floors[index].miny -= 5;
    floors[index].maxy += 5;
    floors[index].groundMesh = createCharTexture(floors[index].maxx - floors[index].minx, floors[index].maxy - floors[index].miny);

    FOR(i, floors[index].roomNum){
        for(int j = floors[index].roomList[i]->x - floors[index].minx; j < floors[index].roomList[i]->x - floors[index].minx + floors[index].roomList[i]->w; j++){
            for(int z = floors[index].roomList[i]->y - floors[index].miny; z < floors[index].roomList[i]->y - floors[index].miny + floors[index].roomList[i]->h; z++){
               floors[index].groundMesh->data[z][j] = '.';
            }
        }
        for(int j = floors[index].roomList[i]->x - floors[index].minx - 1; j <= floors[index].roomList[i]->x - floors[index].minx + floors[index].roomList[i]->w; j++){
            floors[index].groundMesh->data[floors[index].roomList[i]->y - 1 - floors[index].miny][j] = '#';
            floors[index].groundMesh->data[floors[index].roomList[i]->y + floors[index].roomList[i]->h - floors[index].miny][j] = '#';
        }
        for(int j = floors[index].roomList[i]->y - floors[index].miny - 1; j <= floors[index].roomList[i]->y - floors[index].miny + floors[index].roomList[i]->h; j++){
            floors[index].groundMesh->data[j][floors[index].roomList[i]->x - 1 - floors[index].minx] = '#';
            floors[index].groundMesh->data[j][floors[index].roomList[i]->x + floors[index].roomList[i]->w - floors[index].minx] = '#';
        }
    }

    Room* p;

    FOR(i, floors[index].roomNum){
        FOR(j, floors[index].roomNum){
            if(floors[index].roomList[i]->x + floors[index].roomList[i]->w/2 > floors[index].roomList[j]->x + floors[index].roomList[i]->w/2){
                p = floors[index].roomList[i];
                floors[index].roomList[i] = floors[index].roomList[j];
                floors[index].roomList[j] = p;
            }
        }
    }

    MovingCell* cell;
    FOR(i, floors[index].roomNum - 1){
        cell = malloc(sizeof(MovingCell));
        linkedListPushBack(&PathCells, cell);
        cell->x = floors[index].roomList[i]->x+ floors[index].roomList[i]->w/2 - floors[index].minx;
        cell->y = floors[index].roomList[i]->y+ floors[index].roomList[i]->h/2 - floors[index].miny;
        cell->type = 0;
        cell->attr[0] = floors[index].roomList[i+1]->x+ floors[index].roomList[i+1]->w/2 - floors[index].minx;
        cell->attr[1] = floors[index].roomList[i+1]->y+ floors[index].roomList[i+1]->h/2 - floors[index].miny;
    }
    while(PathCells.size){
        iterateMovingCells(&PathCells, floors[index].groundMesh);
    }

    player.x = floors[index].roomList[0]->x + floors[index].roomList[0]->w/2;
    player.y = floors[index].roomList[0]->y + floors[index].roomList[0]->h/2;
    player.visionRadius = 5;

    floors[index].visited = createCharTexture(floors[index].groundMesh->w, floors[index].groundMesh->h);
    fillCharTexture(floors[index].visited, 0);
}

void generateMap(){
    //floorNum = randBetween(gameSettings.minFloorNum, gameSettings.maxFloorNum + 1, 0);
    //floors = malloc(floorNum * sizeof(Floor));
    
    //FOR(i, floorNum){
    //    generateFloor(i);
    //}

    //generateLoot();
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
    if(1){
        player.x += x;
        player.y += y;
        //updatePlayer();
    }
    
}
void deleteRoomGraph(Room* r){
    if(r->visited != 2){
        r->visited = 2;
        if(r->top){
            r->top->bottom = NULL;
            deleteRoomGraph(r->top);
        }if(r->bottom){
            r->bottom->top = NULL;
            deleteRoomGraph(r->bottom);
        }if(r->right){
            r->right->left = NULL;
            deleteRoomGraph(r->right);
        }if(r->left){
            r->left->right = NULL;
            deleteRoomGraph(r->left);
        }
        free(r);
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
                        // if(pointNum && pointPlacement){
                        //     Point* p = pointCloud[pointNum-1];
                        //     p->radius = hypot(mEvent.x + mainCamera.x - p->x, mEvent.y + mainCamera.y - p->y);
                        // }
                        break;
                    default:
                        uiMouseClick();
                        // if(mEvent.bstate & BUTTON1_PRESSED){
                        //     if(pointPlacement){
                        //         pointPlacement = 0;
                        //     }else{
                        //         Point* p = malloc(sizeof(Point));
                        //         p->x = mainCamera.x + mEvent.x;
                        //         p->y = mainCamera.y + mEvent.y;
                        //         p->locked = 0;
                        //         pointNum++;
                        //         pointCloud = realloc(pointCloud ,sizeof(Point*) * pointNum);
                        //         pointCloud[pointNum-1] = p;
                        //         pointPlacement = 1;
                        //     }
                        // }
                        break;
                }
            }
            break;
        case ERR:
            break;
        default:
            switch(ch){
                case 'q':
                    gameSettings.debugMode = !gameSettings.debugMode;
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
                    // if(player.z != 0){
                    //     player.z--;
                    //     player.x = floors[player.z].roomList[0]->x + floors[player.z].roomList[0]->w / 2;
                    //     player.y = floors[player.z].roomList[0]->y + floors[player.z].roomList[0]->h / 2;
                    // }
                    break;
                case 'j':
                    // if(player.z != floorNum-1){
                    //     player.z++;
                    //     player.x = floors[player.z].roomList[0]->x + floors[player.z].roomList[0]->w / 2;
                    //     player.y = floors[player.z].roomList[0]->y + floors[player.z].roomList[0]->h / 2;
                    // }
                    break;
                case 'i':
                    iteratePointCloud(pointCloud,adjMat, pointNum, 5);
                    break;
                case 'c':
                    if(player.items.size){
                        ItemBase* tmp = linkedListGetElement(&(player.items), 0);
                        tmp->drop(tmp);
                    }
                    break;
                case 'g':
                    deleteRoomGraph(roomRoot);
                    FOR(i, pointNum){
                        free(pointCloud[i]);
                        free(adjMat[i]);
                    }
                    free(adjMat);
                    adjMat = NULL;
                    free(pointCloud);
                    pointCloud = NULL;
                    free(roomList);
                    roomList = NULL;
                    pointNum = 0;
                    roomRoot = createRoomGraph(NULL, NULL, NULL, 7, 14, 0.2, 0);
                    parseRoomGraph(roomRoot, 0, 0);
                    createAdjMat();
                    break;
                case 't':
                    mgtoggleDegbugMenu();
                    break;  
            }
            break;
            
    }
}

void renderMainGame(){
    //erase();
    fillCharTexture(frameBuffer, ' ');
    //fillCharTexture(visitedMaskBuffer, 0);

    mainCamera.x = player.x-mainCamera.w/2;
    mainCamera.y = player.y-mainCamera.h/2;

    //attron(COLOR_PAIR(100));
    
    //mrenderTexture(floors[player.z].groundMesh, NULL, floors[player.z].minx, floors[player.z].miny, frameBuffer, NULL);
    //mrenderTexture(floors[player.z].visited, NULL, floors[player.z].minx, floors[player.z].miny, visitedMaskBuffer, NULL);
    //maskFrameBuffer(frameBuffer, NULL, visitedMaskBuffer);

    // void** tmpPtr = floors[player.z].itemList->data;
    // ItemBase* tmp;
    // FOR(i, floors[player.z].itemList->size){
    //     tmp = tmpPtr[1];
    //     tmp->render(tmp, frameBuffer, NULL, &mainCamera);
    //     if(tmpPtr) tmpPtr = *tmpPtr;
    // }

    FOR(i, pointNum){
        drawCircleOnCharTexture(frameBuffer, pointCloud[i]->x - mainCamera.x, pointCloud[i]->y - mainCamera.y, pointCloud[i]->radius, '.');
    }
    FOR(i, pointNum){
        for(int j = i + 1; j < pointNum; j++){
            if(adjMat[i][j]){
                renderLine('o', roomList[i]->point->x,  roomList[i]->point->y,  roomList[j]->point->x,  roomList[j]->point->y, &mainCamera, 
                frameBuffer, NULL);
            }
        }
    }

    FOR(i, pointNum){
        FOR(j, pointNum){
            frameBuffer->data[j][i] = adjMat[i][j] + '0';
        }
    }

    renderFrameBuffer(frameBuffer);
    
    // tmpPtr = player.items.data;
    // int j = 0;
    // FOR(i, player.items.size){
    //     tmp = tmpPtr[1];
    //     if(tmp->objectType == TYPE_AMMO){
    //         Ammo* a = tmp->object;
    //         mvprintw(2 + (j++), 2, "%s : %d", a->name, a->quantity);
    //     }else if(tmp->objectType == TYPE_WEAPON){
    //         Weapon* w = tmp->object;
    //         mvprintw(2 + (j++), 2, "%s : %d", w->name, w->quantity);
    //     }else if(tmp->objectType == TYPE_KEY){
    //         Key* k = tmp->object;
    //         mvprintw(2 + (j++), 2, "%s : %d", k->name, k->quantity);
    //     }else if(tmp->objectType == TYPE_COIN){
    //         Coin* c = tmp->object;
    //         mvprintw(2 + (j++), 2, "%s : %d", c->name, c->quantity);
    //     }
    //     if(tmpPtr) tmpPtr = *tmpPtr;
    // }

    mvprintw(scrH/2, scrW/2, "@");

    //renderUi();


    refresh();
}

void exitMainGame(){

}