#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

#include "MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../GameObjects/Player.h"
#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"
#include "../GameObjects/Room.h"
#include "../GameObjects/GameSettings.h"
#include "../GameObjects/Floor.h"

void generateMap();
void createFloor();
void addRoom();

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};


GameSettings gameSettings;
Player player;
Floor floor1;

Room** roomList;
Point** pointCloud;
char** groundMesh;

int roomNum = 0;
int camerax, cameray;


void initMainGame(){
    init_pair(100, COLOR_BLACK, COLOR_WHITE);

    gameSettings.difficaulity = 0;
    gameSettings.maxRoomNumber = 15;
    gameSettings.minRoomNumber = 6;
    gameSettings.minRoomSize = 40;
    gameSettings.maxRoomSize = 60;
    gameSettings.roomSpread = 50;
    gameSettings.debugMode = 1;
    gameSettings.debugShowPointCloud = 1;
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

    generateMap();

    engineState = &mainGame;

    timeout(1000);
    nodelay(stdscr, FALSE);
}

void createFloor(){
    FOR(i, roomNum){
        roomList[i] = malloc(sizeof(Room));
        roomList[i]->w = randBetween(gameSettings.minRoomSize, 2 * ceil(sqrt(pow(pointCloud[i]->radius, 2) - pow(gameSettings.minRoomSize / 3,2)) - 1), i);
        roomList[i]->h = ceil(sqrt(pow(pointCloud[i]->radius, 2) - pow(roomList[i]->w / 2, 2)));
        roomList[i]->x = pointCloud[i]->x * 2 - roomList[i]->w/2;
        roomList[i]->y = pointCloud[i]->y - roomList[i]->h/2;
    }

    floor1.minx = INT32_MAX; floor1.miny = INT32_MAX; floor1.maxx = 0; floor1.maxy = 0;
    FOR(i, roomNum){
        floor1.minx = min(roomList[i]->x, floor1.minx);
        floor1.miny = min(roomList[i]->y, floor1.miny);
        floor1.maxx = max(roomList[i]->x + roomList[i]->w - 1, floor1.maxx);
        floor1.maxy = max(roomList[i]->y + roomList[i]->h - 1, floor1.maxy);
    }

    floor1.roomList = roomList;
    floor1.groundMesh = malloc((floor1.maxy - floor1.miny + 1) * sizeof(char*));

    FOR(i, floor1.maxy - floor1.miny + 1){
        floor1.groundMesh[i] = malloc(floor1.maxx - floor1.minx + 1);
        FOR(j, floor1.maxx - floor1.minx + 1){
            floor1.groundMesh[i][j] = '\0';
        }
    }

    FOR(i, roomNum){
        for(int j = roomList[i]->x - floor1.minx; j < roomList[i]->x - floor1.minx + roomList[i]->w; j++){
            for(int z = roomList[i]->y - floor1.miny; z < roomList[i]->y - floor1.miny + roomList[i]->h; z++){
               floor1.groundMesh[z][j] = '0';
            }
        }
    }


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
    pointCloud[0]->locked = 1;

    int roomsPlaced = 0;
    while(!roomsPlaced){

        FOR(i, 1000){
            if(iteratePointCloud(pointCloud, roomNum, gameSettings.roomSpread)){
                roomsPlaced = 1;
                break;
            }
        }

        if(!roomsPlaced){
            gameSettings.roomSpread+= 5;
            FOR(i, roomNum){
                pointCloud[i]->x = i;
                pointCloud[i]->y = i;
                pointCloud[i]->radius = randBetween((int)(gameSettings.minRoomSize * 1.4), (int)(gameSettings.maxRoomSize * 1.4), i);
            }
        }

        
    }
    
    createFloor();
}
void addRoom(){

}
void createPaths(){

}

int moveValid(int x, int y){
    x += player.x;
    y += player.y;

    if(floor1.groundMesh[y-floor1.miny][x-floor1.minx] == '0'){
        return 1;
    }else return 0;
}
void updateMainGame(){
    int ch = getch();
    

    switch(ch){
        case KEY_RESIZE:
            getmaxyx(stdscr, scrH, scrW);
            clear();
            refresh();
            break;
        case KEY_MOUSE:
            if(getmouse(&mEvent) == OK){
                switch(mEvent.bstate){
                    case KEY_MOUSE_MOVE:
                        mousex = mEvent.x;
                        mousey = mEvent.y;
                        break;
                    default:
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
                    if(moveValid(0, -1))player.y--;
                    break;
                case 'd':
                    if(moveValid(1, 0))player.x++;
                    break;
                case 'a':
                    if(moveValid(-1, 0))player.x--;
                    break;
                case 's':
                    if(moveValid(0, 1))player.y++;
                    break;
            }
            break;
            
    }
}

void renderMainGame(){
    clear();

    camerax = player.x - scrW/2;
    cameray = player.y - scrH/2;

    attron(COLOR_PAIR(100));
    if(gameSettings.debugMode){
        if(gameSettings.debugShowPointCloud){
            FOR(i, roomNum){
                pointRender(pointCloud[i]);
                //mvprintw(i + 1,2, "%f %f", pointCloud[i]->x, pointCloud[i]->y);
            }
            mvprintw(0, 0, "%d %d %d %d", floor1.minx, floor1.miny, floor1.maxx, floor1.maxy);
        }
    }

    FOR(i, roomNum){
        renderRoom(roomList[i], camerax, cameray);
    }
    mvprintw(scrH/2, scrW/2, ". .");
    mvprintw(scrH/2 + 1, scrW/2, "-0-");
    attroff(COLOR_PAIR(100));




    refresh();
}

void exitMainGame(){

}