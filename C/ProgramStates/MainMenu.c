#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "MainMenu.h"
#include "NewGameMenu.h"
#include "LogInMenu.h"
#include "SignupMenu.h"
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
    createWidget(&menu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 10, 10, 30, 12, C_BG_BLACK );


    createButton(&mmNewGameBtn, &menu, "New Game"   , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 0, 100);
    createButton(&mmLoadGameBtn, &menu, "Load Game" , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 2, 100);
    createButton(&mmSettingsBtn, &menu, "Scoreboard", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 100);
    createButton(&mmSocreboardBtn, &menu, "Settings", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 6, 100);
    createButton(&mmLoginBtn, &menu, "Log in", RELATIVE, ALIGN_LEFT, ALIGN_TOP, 0, 8, 48);
    createButton(&mmNewCharacterBtn, &menu, "New Character", RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 8, 48);
    createButton(&mmExitBtn, &menu, "Exit"          , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 10, 80);
    
    mmExitBtn.callBack = &exitMainMenu;
    mmNewGameBtn.callBack = (newGameMenu.enter);
    mmLoginBtn.callBack = logInMenu.enter;
    mmNewCharacterBtn.callBack = signUpMenu.enter;


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
                            buttonMouseClickEvent(mainMenuButtonList[i]);
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
    erase();
    renderWidget(&menu);
    FOR(i, 7){
        renderButton(mainMenuButtonList[i]);
    }
    refresh();
}