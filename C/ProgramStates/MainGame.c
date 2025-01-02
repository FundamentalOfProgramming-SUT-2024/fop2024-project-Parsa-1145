#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"

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
#include "../GameObjects/Renderer.h"
#include "../GameObjects/Texture.h"
#include "../GameObjects/GameObject.h"
#include "../GameObjects/Weapon.h"

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

int roomNum = 0;

Widget debugMenu;
Button mgCloseDebugBtn;
CheckBox mgShowPointCloudCb;
CheckBox mgNoClipCb;

Button* mgButtonList[1] = {&mgCloseDebugBtn};
CheckBox* mgCheckBoxList[2] = {&mgShowPointCloudCb, &mgNoClipCb};

void mgtoggleDegbugMenu(){
    debugMenu.isVisible = !debugMenu.isVisible;
}
void uiMouseMove(){
    FOR(i, 1){
        buttonMouseMoveCallback(mgButtonList[i]);
    }
    FOR(i, 2){
        checkBoxMouseMoveCallback(mgCheckBoxList[i]);
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
        checkBoxMouseClickEvent(mgCheckBoxList[i]);
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
    gameSettings.roomSpread = 5;
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

        GameObject* tmp;
        FOR(j, floors[i].itemList->size){
            tmp = linkedListGetElement(floors[i].itemList, 0);
            tmp->deleteObject(tmp->object);
            popLinkedList(floors[i].itemList);
        }
        free(floors[i].itemList);

        free(floors + i);
    }

    player.x = 0;
    player.y = 0;
    player.z = 0;
    FOR(i, player.items.size){
        GameObject* tmp;
        tmp = linkedListGetElement(&(player.items), 0);
        tmp->deleteObject(tmp->object);
        popLinkedList(&(player.items));
    }
    mainCamera.h = scrH;
    mainCamera.w = scrW;

    generateMap();

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);

    frameBuffer = createCharTexture(scrW, scrH);
    visitedMaskBuffer = createCharTexture(scrW, scrH);
}

void generateFloor(int index){
    floors[index].roomNum = randBetween(gameSettings.minRoomNum, gameSettings.maxRoomNum + 1, 1 + index);
    floors[index].pointCloud = malloc(floors[index].roomNum * sizeof(Point*));
    floors[index].roomList = malloc(sizeof(Room*) * floors[index].roomNum);
    floors[index].itemList = malloc(sizeof(LinkedList));
    createLinkedList(floors[index].itemList, sizeof(GameObject*));

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
            if(iteratePointCloud(floors[index].pointCloud, floors[index].roomNum, gameSettings.roomSpread)){
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

        Weapon* w = malloc(sizeof(Weapon));
        w->sprite = '*';
        w->x = floors[index].roomList[i]->x + 2;
        w->y = floors[index].roomList[i]->y + 2;
        w->z = index;
        createWeapon(w);
        linkedListPushBack(floors[index].itemList, w->gameObject);
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
    floorNum = randBetween(gameSettings.minFloorNum, gameSettings.maxFloorNum + 1, 0);
    floors = malloc(floorNum * sizeof(Floor));
    
    FOR(i, floorNum){
        generateFloor(i);
    }
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
                        mousex = mEvent.x;
                        mousey = mEvent.y;
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
                case 'q':
                    gameSettings.debugMode = !gameSettings.debugMode;
                    break;
                case 'w':
                    if(moveValid(0, -1)){
                        player.y--;
                        updatePlayer();
                    }
                    break;
                case 'd':
                    if(moveValid(1, 0)){
                        player.x++;
                        updatePlayer();
                    }

                    break;
                case 'a':
                    if(moveValid(-1, 0)){
                    player.x--;
                    updatePlayer();
                    }

                    break;
                case 's':
                    if(moveValid(0, 1)){
                    player.y++;
                    updatePlayer();
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
    fillCharTexture(visitedMaskBuffer, 0);

    mainCamera.x = player.x-mainCamera.w/2;
    mainCamera.y = player.y-mainCamera.h/2;

    //attron(COLOR_PAIR(100));
    
    mrenderTexture(floors[player.z].groundMesh, NULL, floors[player.z].minx, floors[player.z].miny, frameBuffer, NULL);
    mrenderTexture(floors[player.z].visited, NULL, floors[player.z].minx, floors[player.z].miny, visitedMaskBuffer, NULL);
    //maskFrameBuffer(frameBuffer, NULL, visitedMaskBuffer);


    if(gameSettings.debugMode){
        if(gameSettings.debugShowPointCloud){
            FOR(i, roomNum){
                pointRender(floors[player.z].pointCloud[i]);
                mvprintw(i + 1,2, "%f %f", floors[player.z].pointCloud[i]->x, floors[player.z].pointCloud[i]->y);
            }
        }
    }

    void** tmpPtr = floors[player.z].itemList->data;
    GameObject* tmp;
    FOR(i, floors[player.z].itemList->size){
        tmp = tmpPtr[1];
        tmp->render(tmp->object, frameBuffer, NULL, &mainCamera);
        if(tmpPtr) tmpPtr = *tmpPtr;
    }

    renderFrameBuffer(frameBuffer);
    
    mvprintw(scrH/2, scrW/2, "@");

    renderUi();


    refresh();
}

void exitMainGame(){

}