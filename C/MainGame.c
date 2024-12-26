#include "MainGame.h"
#include <ncurses.h>
#include "Player.h"
#include "stdlib.h"
#include "GlobalDefines.h"
#include "Globals.h"
#include "Utilities/LinkedList.h"
#include "GameObjects/Room.h"
#include "Utilities/PointCloud.h"

EngineState mainGame = {&enterMainGame, &updateMainGame, &renderMainGame, &exitMainGame};

Player* player;
LinkedList roomList;
Point** pointCloud;

int maxRoomNum = 10;
int roomSpread = 2;


void initMainGame(){
    init_pair(100, COLOR_BLACK, COLOR_WHITE);
    createLinkedList(&roomList, sizeof(Room*));

    pointCloud = malloc(10 * sizeof(Point*));
    FOR(i, 10){
        pointCloud[i] = malloc(sizeof(Point));
        pointCloud[i]->x = 0;
        pointCloud[i]->y = 0;
        pointCloud[i]->radius = 5;
        pointCloud[i]->locked = 0;
    }
    pointCloud[0]->locked = 1;
}

void createFloor(){
    rand();
}
void addRoom(){

}
void roomValid(){

}
void createPaths(){

}
void enterMainGame(){
    
    clear();
    refresh();

    player = malloc(sizeof(Player));

    player->x = 0;
    player->y = 0;

    engineState = &mainGame;    int x ,y;

    leaveok(stdscr, FALSE);
    timeout(1000);
    nodelay(stdscr, FALSE);
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
            if(ch == KEY_UP){
                player->y--;
                iteratePointCloud(pointCloud, 10, roomSpread);
            }else if(ch == KEY_LEFT){
                roomSpread--;
            }else if(ch == KEY_RIGHT){
                roomSpread++;
            }
            break;
            
    }
}

void renderMainGame(){
    clear();
    attron(COLOR_PAIR(100));
    FOR(i, 10){
        pointRender(pointCloud[i]);
        mvprintw(i + 1,2, "%f %f", pointCloud[i]->x, pointCloud[i]->y);
    }
    mvprintw(12, 2, "%d", roomSpread);
    attroff(COLOR_PAIR(100));

    refresh();
}

void exitMainGame(){

}