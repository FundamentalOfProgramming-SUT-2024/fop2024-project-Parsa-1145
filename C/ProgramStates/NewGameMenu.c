#include <stdlib.h>
#include <ncurses.h>

#include "MainMenu.h"
#include "MainGame.h"
#include "NewGameMenu.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../UiElements/Widget.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Button.h"


 
Button ngmStart;
Button ngmBackBtn;
Button* newGameMenuButtonList[2] = {&ngmStart, &ngmBackBtn};

Widget sideBar;

EngineState newGameMenu = {&enterNewGameMenu, &updateNewGameMenu, &renderNewGameMenu, &exitNewGameMenu};

void initNewGameMenu(){
    createWidget(&sideBar, NULL, ABSOLUTE, RELATIVE, ALIGN_LEFT, ALIGN_CENTER, 0, 0, 40, 100, C_BG_GRAY0);

    createButton(&ngmStart, &sideBar, "Start", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&ngmBackBtn, &sideBar, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 3, 2, 6);
    
    ngmBackBtn.callBack = maineMenu.enter;
    ngmStart.callBack = mainGame.enter;
}

void enterNewGameMenu(){
    clear();
    refresh();
    engineState = &newGameMenu;
    curs_set(0);
    keypad(stdscr, TRUE);       
    noecho();
    //cbreak();
    //mouseinterval(1);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    //timeout(1000);
    printf("\033[?1003h\n");

}
void updateNewGameMenu(){
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
                        FOR(i, 2){
                            buttonMouseMoveCallback(newGameMenuButtonList[i]);
                        }
                        break;
                    default:
                        FOR(i, 2){
                            buttonMouseClickEvent(newGameMenuButtonList[i]);
                        }
                        break;
                }
            }
            break;
        case ERR:
            break;
        default:
            break;
            
    }
}
void renderNewGameMenu(){
    renderWidget(&sideBar);
    FOR(i, 2){
        renderButton(newGameMenuButtonList[i]);
    }

    refresh();
}
void exitNewGameMenu(){

}