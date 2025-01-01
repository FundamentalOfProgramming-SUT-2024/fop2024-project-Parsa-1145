#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../GameObjects/Player.h"
#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"
#include "../Utilities/MovingCell.h"
#include "../UiElements/Widget.h"
#include "../UiElements/Button.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Checkbox.h"
#include "../GameObjects/Room.h"
#include "../GameObjects/GameSettings.h"
#include "../GameObjects/Floor.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Renderer.h"
#include "../GameObjects/Texture.h"

void generateMap();
void createFloor();
void makePath(float startPos, float endPos, int startRoom, int destRoom);

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};

Player player;
Floor floor1;
Camera mainCamera;
CharTexture* frameBuffer;
CharTexture* visitedMaskBuffer;

Room** roomList;
Point** pointCloud;
CharTexture* visited;
LinkedList PathCells;
int cellState = 0;

int roomNum = 0;

Widget debugMenu;
Button mgCloseDebugBtn;
Button debugTogglePointCloudBtn;
Button debugToggleNoclipBtn;
CheckBox showCloud;

Button* mgButtonList[3] = {&mgCloseDebugBtn, &debugToggleNoclipBtn, &debugTogglePointCloudBtn};

void mgtoggleDegbugMenu(){
    debugMenu.isVisible = !debugMenu.isVisible;
}
void uiMouseMove(){
    FOR(i, 3){
        buttonMouseMoveCallback(mgButtonList[i]);
    }
    checkBoxMouseMoveCallback(&showCloud);
}
void renderUi(){
    renderWidget(&debugMenu);
    FOR(i, 3){
        renderButton(mgButtonList[i]);
    }
    renderCheckBox(&showCloud);

}
void uiMouseClick(){
    FOR(i, 3){
        buttonMouseClickEvent(mgButtonList[i]);
    }
    checkBoxMouseClickEvent(&showCloud);
}

void initMainGame(){
    createWidget(&debugMenu, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, C_BG_GRAY0);
    debugMenu.layoutType = HORIZONTAL_LAYOUT;
    debugMenu.layoutPadding = 1;
    debugMenu.isVisible = 0;

    createButton(&mgCloseDebugBtn, &debugMenu, "Close", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 7);
    createButton(&debugTogglePointCloudBtn, &debugMenu, "Toggle point cloud", ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 5, 20);
    createButton(&debugToggleNoclipBtn, &debugMenu, "Toggle no clip", ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 20);
    createCheckBox(&showCloud, &debugMenu, "Show Clouds", &(gameSettings.debugShowPointCloud), RELATIVE, ABSOLUTE, ALIGN_LEFT, ABSOLUTE, 2, 2, 100);
    linkedListPushBack(debugMenu.children, debugTogglePointCloudBtn.widget);
    linkedListPushBack(debugMenu.children, debugToggleNoclipBtn.widget);
    linkedListPushBack(debugMenu.children, showCloud.widget);

    updateWidgetChildren(&debugMenu);
    mgCloseDebugBtn.callBack = &mgtoggleDegbugMenu;


    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNumber = 15;
    gameSettings.minRoomNumber = 6;
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

    createLinkedList(&PathCells, sizeof(MovingCell*));
}
void enterMainGame(){
    
    clear();
    refresh();

    FOR(i, roomNum){
        free(pointCloud[i]);
        free(roomList[i]);
    }

    free(roomList);
    free(pointCloud);

    player.x = 0;
    player.y = 0;
    mainCamera.h = scrH;
    mainCamera.w = scrW;

    generateMap();

    engineState = &mainGame;

    timeout(100);
    nodelay(stdscr, FALSE);

    frameBuffer = createCharTexture(scrW, scrH);
    visitedMaskBuffer = createCharTexture(scrW, scrH);
}

void makePath(float startPos, float endPos, int startRoom, int destRoom){
    

}

void createFloor(){
    float lastRatio = 1;
    FOR(i, roomNum){
        roomList[i] = createRoom(0, 0, 0, 0);
        roomList[i]->w = randBetween(gameSettings.minRoomSize, 2 * ceil(sqrt(pow(pointCloud[i]->radius, 2) - pow(gameSettings.minRoomSize / 2,2)))-1, i);
        roomList[i]->h = 2 * floor(sqrt(pow(pointCloud[i]->radius, 2) - pow(roomList[i]->w / 2, 2)));
        
        if((lastRatio - 1) * ((roomList[i]->w / roomList[i]->h) - 1) > 0){
            int tmp = roomList[i]->h;
            roomList[i]->h = roomList[i]->w;
            roomList[i]->w = tmp;
        }
        lastRatio = roomList[i]->w / roomList[i]->h;

        roomList[i]->x = 2 * pointCloud[i]->x- roomList[i]->w/2;
        roomList[i]->y = pointCloud[i]->y- roomList[i]->h/2;
        roomList[i]->theme = randIndexWithProb(gameSettings.roomThemeNum, gameSettings.roomThemeProb, i);
    }

    floor1.minx = INT32_MAX; floor1.miny = INT32_MAX; floor1.maxx = 0; floor1.maxy = 0;
    FOR(i, roomNum){
        floor1.minx = min(roomList[i]->x, floor1.minx);
        floor1.miny = min(roomList[i]->y, floor1.miny);
        floor1.maxx = max(roomList[i]->x + roomList[i]->w - 1, floor1.maxx);
        floor1.maxy = max(roomList[i]->y + roomList[i]->h - 1, floor1.maxy);
    }


    floor1.minx -= 5;
    floor1.maxx += 5;
    floor1.miny -= 5;
    floor1.maxy += 5;
    floor1.roomList = roomList;
    floor1.groundMesh = createCharTexture(floor1.maxx - floor1.minx, floor1.maxy - floor1.miny);

    FOR(i, roomNum){
        for(int j = roomList[i]->x - floor1.minx; j < roomList[i]->x - floor1.minx + roomList[i]->w; j++){
            for(int z = roomList[i]->y - floor1.miny; z < roomList[i]->y - floor1.miny + roomList[i]->h; z++){
               floor1.groundMesh->data[z][j] = '.';
            }
        }
        for(int j = roomList[i]->x - floor1.minx - 1; j <= roomList[i]->x - floor1.minx + roomList[i]->w; j++){
            floor1.groundMesh->data[roomList[i]->y - 1 - floor1.miny][j] = '#';
            floor1.groundMesh->data[roomList[i]->y + roomList[i]->h - floor1.miny][j] = '#';
        }
        for(int j = roomList[i]->y - floor1.miny - 1; j <= roomList[i]->y - floor1.miny + roomList[i]->h; j++){
            floor1.groundMesh->data[j][roomList[i]->x - 1 - floor1.minx] = '#';
            floor1.groundMesh->data[j][roomList[i]->x + roomList[i]->w - floor1.minx] = '#';
        }
    }

    Room* p;

    FOR(i, roomNum){
        FOR(j, roomNum){
            if(floor1.roomList[i]->x + floor1.roomList[i]->w/2 > floor1.roomList[j]->x + floor1.roomList[i]->w/2){
                p = floor1.roomList[i];
                floor1.roomList[i] = floor1.roomList[j];
                floor1.roomList[j] = p;
            }
        }
    }

    MovingCell* cell;
    FOR(i, roomNum - 1){
        cell = malloc(sizeof(MovingCell));
        linkedListPushBack(&PathCells, cell);
        cell->x = floor1.roomList[i]->x+ floor1.roomList[i]->w/2 - floor1.minx;
        cell->y = floor1.roomList[i]->y+ floor1.roomList[i]->h/2 - floor1.miny;
        cell->type = 0;
        cell->attr[0] = floor1.roomList[i+1]->x+ floor1.roomList[i+1]->w/2 - floor1.minx;
        cell->attr[1] = floor1.roomList[i+1]->y+ floor1.roomList[i+1]->h/2 - floor1.miny;
    }

    player.x = roomList[0]->x + roomList[0]->w/2;
    player.y = roomList[0]->y + roomList[0]->h/2;
    player.visionRadius = 5;

    visited = createCharTexture(floor1.groundMesh->w, floor1.groundMesh->h);
    fillCharTexture(visited, 0);
}

void generateMap(){
    roomNum = randBetween(gameSettings.minRoomNumber, gameSettings.maxRoomNumber, 0);
    
    pointCloud = malloc(roomNum * sizeof(Point*));
    roomList = malloc(sizeof(Room*) * roomNum);

    FOR(i, roomNum){
        pointCloud[i] = malloc(sizeof(Point));

        pointCloud[i]->x = 0;
        pointCloud[i]->y = 0;
        pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i);
        pointCloud[i]->locked = 0;
    }

    int roomsPlaced = 0;
    while(roomsPlaced < 100){
        FOR(i, 40000){
            if(iteratePointCloud(pointCloud, roomNum, gameSettings.roomSpread)){
                roomsPlaced += 1;
                if(roomsPlaced == 100) break;
            }
            getmaxyx(stdscr, scrH, scrW);
            
            erase();
            FOR(i, roomNum){
                pointRender(pointCloud[i]);
            }
            refresh();
        }
        if(roomsPlaced < 100){
            gameSettings.roomSpread+= 5;
            FOR(i, roomNum){
                pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i);
                pointCloud[i]->x = 0;
                pointCloud[i]->y = 0;
                pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i);
            }
            roomsPlaced = 0;
        }
    }
    
    createFloor();
}

int moveValid(int x, int y){
    if(gameSettings.noClip) return 1;
    x += player.x;
    y += player.y;

    if((floor1.groundMesh->data[y-floor1.miny][x-floor1.minx] == '.') || (floor1.groundMesh->data[y-floor1.miny][x-floor1.minx] == '#')){
        return 1;
    }else return 0;
}
void updatePlayer(){
    drawCircleOnCharTexture(visited, player.x - floor1.minx, player.y - floor1.miny, player.visionRadius, 1);
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
                case 'i':
                    iterateMovingCells(&PathCells, floor1.groundMesh);
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
    
    mrenderTexture(floor1.groundMesh, NULL, floor1.minx, floor1.miny, frameBuffer, NULL);
    mrenderTexture(visited, NULL, floor1.minx, floor1.miny, visitedMaskBuffer, NULL);
    maskFrameBuffer(frameBuffer, NULL, visitedMaskBuffer);
    renderFrameBuffer(frameBuffer);


    if(gameSettings.debugMode){
        if(gameSettings.debugShowPointCloud){
            FOR(i, roomNum){
                pointRender(pointCloud[i]);
                mvprintw(i + 1,2, "%f %f", pointCloud[i]->x, pointCloud[i]->y);
            }
        }
    }
    
    mvprintw(scrH/2, scrW/2, "@");

    renderUi();


    refresh();
}

void exitMainGame(){

}