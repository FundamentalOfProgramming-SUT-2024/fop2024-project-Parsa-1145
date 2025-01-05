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
#include "../UiElements/UiBase.h"
#include "../Utilities/LinkedList.h"


Button mmExitBtn;
Button mmNewGameBtn;
Button mmLoadGameBtn;
Button mmSettingsBtn;
Button mmSocreboardBtn;
Button mmLoginBtn;
Button mmNewCharacterBtn;
Button mmLogOutBtn;
Widget menu;

LinkedList mmUiList;

EngineState maineMenu = {&enterMainMenu, &updateMainMenu, &renderMainMenu, &exitMainMenu};

void exitMainMenu(){
    terminate = 1;
}

void mmStartGame(){
    // if(account.username){
    //     enterNewGameMenu();
    // }
    enterNewGameMenu();
}
void logOut(){
    free(account.username);
    free(account.email);
    free(account.password);

    account.username = NULL;

    mmNewCharacterBtn.widget->isVisible = 1;
    mmLoginBtn.widget->isVisible = 1;
    mmLogOutBtn.widget->isVisible = 0;
}
void initMainMenu(){
    createLinkedList(&mmUiList, sizeof(UiBase*));
    createWidget(&menu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 10, 10, 30, 12, C_BG_BLACK );

    createButton(&mmNewGameBtn, &menu, "New Game"   , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 0, 100);
    createButton(&mmLoadGameBtn, &menu, "Load Game" , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 2, 100);
    createButton(&mmSettingsBtn, &menu, "Scoreboard", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 100);
    createButton(&mmSocreboardBtn, &menu, "Settings", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 6, 100);
    createButton(&mmLoginBtn, &menu, "Log in", RELATIVE, ALIGN_LEFT, ALIGN_TOP, 0, 8, 48);
    createButton(&mmNewCharacterBtn, &menu, "Sign up", RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 8, 48);
    createButton(&mmLogOutBtn, &menu, "Log out", RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 0, 8, 100);
    mmLogOutBtn.widget->isVisible = 0;
    createButton(&mmExitBtn, &menu, "Exit"          , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 0, 10, 80);

    linkedListPushBack(&mmUiList, menu.uiBase);
    linkedListPushBack(&mmUiList, mmNewGameBtn.uiBase);
    linkedListPushBack(&mmUiList, mmLoadGameBtn.uiBase);
    linkedListPushBack(&mmUiList, mmSettingsBtn.uiBase);
    linkedListPushBack(&mmUiList, mmLoginBtn.uiBase);
    linkedListPushBack(&mmUiList, mmNewCharacterBtn.uiBase);
    linkedListPushBack(&mmUiList, mmLogOutBtn.uiBase);
    linkedListPushBack(&mmUiList, mmExitBtn.uiBase);
    linkedListPushBack(&mmUiList, mmSocreboardBtn.uiBase);
    
    mmExitBtn.callBack = &exitMainMenu;
    mmNewGameBtn.callBack = &mmStartGame;
    mmLoginBtn.callBack = logInMenu.enter;
    mmNewCharacterBtn.callBack = signUpMenu.enter;
    mmLogOutBtn.callBack = &logOut;


}
void enterMainMenu(){
    clear();
    engineState = &maineMenu;
    if(account.username){
        mmNewCharacterBtn.widget->isVisible = 0;
        mmLoginBtn.widget->isVisible = 0;
        mmLogOutBtn.widget->isVisible = 1;
    }

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
                void** tmp = mmUiList.data;

                switch(mEvent.bstate){
                    case KEY_MOUSE_MOVE:
                        mousex = mEvent.x;
                        mousey = mEvent.y;
                        FOR(i, mmUiList.size){
                            ((UiBase*)tmp[1])->mouseMove(((UiBase*)tmp[1])->object);
                            tmp = *tmp;
                        }
                        break;
                    default:
                        FOR(i, mmUiList.size){
                            ((UiBase*)tmp[1])->mouseClick(((UiBase*)tmp[1])->object);
                            tmp = *tmp;
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

    void** tmp = mmUiList.data;
    FOR(i, mmUiList.size){
        ((UiBase*)tmp[1])->render(((UiBase*)tmp[1])->object);
        tmp = *tmp;
    }

    if(account.username){
        mvprintw(2, 2, "Logged in as: %s", account.username);
    }else{
        mvprintw(2, 2, "Not logged in");
    }
    refresh();
}