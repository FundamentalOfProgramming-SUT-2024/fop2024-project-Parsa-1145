#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "MainMenu.h"
#include "NewGameMenu.h"
#include "LogInMenu.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../UiElements/Button.h"
#include "../UiElements/Widget.h"

Button mmExitBtn;
Button mmNewGameBtn;
Button mmLoadGameBtn;
Button mmSettingsBtn;
Button mmSocreboardBtn;
Button mmLoginBtn;
Button mmNewCharacterBtn;
Button* mainMenuButtonList[7] = {&mmNewGameBtn, &mmLoadGameBtn, &mmSocreboardBtn, &mmSettingsBtn, &mmExitBtn, &mmLoginBtn, &mmNewCharacterBtn};

Widget menu;

EngineState maineMenu = {&enterMainMenu, &updateMainMenu, &renderMainMenu, &exitMainMenu};

void exitMainMenu(){
    terminate = 1;
}
void initMainMenu(){
    createWidget(&menu, NULL, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 10, 10, 50, 12, COLOR_BLACK, COLOR_BLACK );


    createButton(&mmNewGameBtn, &menu, "New Game"   , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 0, 100, COLOR_WHITE, COLOR_BLACK, COLOR_GREEN, COLOR_WHITE);
    createButton(&mmLoadGameBtn, &menu, "Load Game" , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 2, 100, COLOR_WHITE, COLOR_BLACK, COLOR_GREEN, COLOR_WHITE);
    createButton(&mmSettingsBtn, &menu, "Scoreboard", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 100, COLOR_WHITE, COLOR_BLACK, COLOR_BLUE, COLOR_WHITE);
    createButton(&mmSocreboardBtn, &menu, "Settings", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 6, 100, COLOR_WHITE, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK);
    createButton(&mmLoginBtn, &menu, "Log in", RELATIVE, ALIGN_LEFT, ALIGN_TOP, 0, 8, 48, COLOR_WHITE, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK);
    createButton(&mmNewCharacterBtn, &menu, "New Character", RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 8, 48, COLOR_WHITE, COLOR_BLACK, COLOR_YELLOW, COLOR_BLACK);
    createButton(&mmExitBtn, &menu, "Exit"          , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 10, 80, COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_WHITE);
    
    mmExitBtn.callBack = &exitMainMenu;
    mmNewGameBtn.callBack = (newGameMenu.enter);
    mmLoginBtn.callBack = logInMenu.enter;


}
void enterMainMenu(){
    clear();
    engineState = &maineMenu;

    curs_set(0);
    keypad(stdscr, TRUE);       
    noecho();
    //cbreak();
    mouseinterval(1);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    timeout(100);
    printf("\033[?1003h\n");
    
}
void updateMainMenu(){
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
                        FOR(i, 7){
                            buttonMouseMoveCallback(mainMenuButtonList[i]);
                        }
                        break;
                    default:
                        FOR(i, 7){
                            buttonMouseClickEvent(mainMenuButtonList[i], mEvent);
                        }
                        break;
                }
            }
            refresh();
            break;
        case ERR:
            break;
        default:
            break;
            
    }
}

void renderMainMenu(){
    renderWidget(&menu);
    FOR(i, 7){
        renderButton(mainMenuButtonList[i]);
    }
    refresh();
}