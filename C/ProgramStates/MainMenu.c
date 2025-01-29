#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "MainMenu.h"
#include "NewGameMenu.h"
#include "LogInMenu.h"
#include "SignupMenu.h"
#include "MainGame.h"
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
    createWidget(&menu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 15, NULL);
    menu.bordered = 1;

    createButton(&mmNewGameBtn, &menu, "New Game"   , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 2, 100);
    createButton(&mmLoadGameBtn, &menu, "Load Game" , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 4, 100);
    createButton(&mmSettingsBtn, &menu, "Scoreboard", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 6, 100);
    createButton(&mmSocreboardBtn, &menu, "Settings", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 8, 100);
    createButton(&mmLoginBtn, &menu, "Log in", RELATIVE, ALIGN_LEFT, ALIGN_TOP, 2, 10, 41);
    createButton(&mmNewCharacterBtn, &menu, "Sign up", RELATIVE, ALIGN_RIGHT, ALIGN_TOP, 2, 10, 41);
    createButton(&mmLogOutBtn, &menu, "Log out", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 10, 100);
    mmLogOutBtn.widget->isVisible = 0;
    createButton(&mmExitBtn, &menu, "Exit"          , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 12, 80);


    linkedListPushBack(&mmUiList, menu.uiBase);
    linkedListPushBack(menu.children, mmNewGameBtn.uiBase);
    linkedListPushBack(menu.children, mmLoadGameBtn.uiBase);
    linkedListPushBack(menu.children, mmSettingsBtn.uiBase);
    linkedListPushBack(menu.children, mmLoginBtn.uiBase);
    linkedListPushBack(menu.children, mmNewCharacterBtn.uiBase);
    linkedListPushBack(menu.children, mmLogOutBtn.uiBase);
    linkedListPushBack(menu.children, mmExitBtn.uiBase);
    linkedListPushBack(menu.children, mmSocreboardBtn.uiBase);
    
    mmExitBtn.callBack = &exitMainMenu;
    mmNewGameBtn.callBack = &mmStartGame;
    mmLoginBtn.callBack = logInMenu.enter;
    mmNewCharacterBtn.callBack = signUpMenu.enter;
    mmLogOutBtn.callBack = &logOut;
    mmLoadGameBtn.contextCallback = &loadGame;
    mmLoadGameBtn.contextObject = "outa.json";


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
                //mvprintw(4, 4, "asd %lu %lu %lu", mEvent.bstate, BUTTON5_PRESSED, BUTTON4_PRESSED);
                switch(mEvent.bstate){
                    case KEY_MOUSE_MOVE:
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
    updateWidgetChildren(&menu);


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