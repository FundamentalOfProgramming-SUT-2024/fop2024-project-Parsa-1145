#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "MainMenu.h"
#include "NewGameMenu.h"
#include "LogInMenu.h"
#include "SignupMenu.h"
#include "Scoreboard.h"
#include "SettingsMenu.h"
#include "MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../UiElements/Button.h"
#include "../UiElements/Widget.h"
#include "../UiElements/UiBase.h"
#include "../Utilities/LinkedList.h"
#include "../GameObjects/Renderer.h"

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

EngineState maineMenu = {&enterMainMenu, &updateMainMenu, &renderMainMenu, &exitMainMenu, &mmUiList};

CharTexture* mmBackGround;
CharTexture* mmBackGround2;
CharTexture* mmFrameBuffer;
Camera mmCamera;

void exitGame(){
    terminate = 1;
}

void exitMainMenu(){
    
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
void mmLoadGame(const char * const address){
    cJSON* json = openJsonFile(address);

    cJSON* accountJ = cJSON_GetObjectItem(json, "account");
    int valid = 1;
    if(accountJ){
        if((!(account.username)) || (strcmp(account.username, accountJ->valuestring))){
            createPopUp("You should load the game with the same account.", NULL, 15, 20, NULL, &mmUiList);
            valid = 0;
        }
    }
    if(valid){
        loadGame(json);
    }
}
int startW, startH;
void initMainMenu(){
    createLinkedList(&mmUiList, sizeof(UiBase*));
    createWidget(&menu, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 15, C_BG_BLACK);
    menu.bordered = 1;

    createButton(&mmNewGameBtn, &menu, "New Game"   , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 2, 100);
    createButton(&mmLoadGameBtn, &menu, "Load Game" , RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 4, 100);
    createButton(&mmSocreboardBtn, &menu, "Scoreboard", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 6, 100);
    createButton(&mmSettingsBtn, &menu, "Settings", RELATIVE, ALIGN_CENTER, ALIGN_TOP, 2, 8, 100);
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
    
    mmExitBtn.callBack = &exitGame;
    mmNewGameBtn.callBack = &mmStartGame;
    mmLoginBtn.callBack = logInMenu.enter;
    mmNewCharacterBtn.callBack = signUpMenu.enter;
    mmSettingsBtn.callBack = enterSettingsMenu;
    mmLogOutBtn.callBack = &logOut;
    mmLoadGameBtn.contextCallback = &mmLoadGame;
    mmLoadGameBtn.contextObject = "outa.json";
    mmSocreboardBtn.callBack = &enterScoreBoard;

    mmBackGround = loadCharTextureFromTxt("../Data/Ascii/mainMenu11.txt");
    mmBackGround2 = loadCharTextureFromTxt("../Data/Ascii/mainMenu9.txt");
    mmFrameBuffer = createFrameBuffer(scrW, scrH);

    getmaxyx(stdscr, startH, startW);

}
void enterMainMenu(){
    clear();
    engineState = &maineMenu;
    if(account.username){
        mmNewCharacterBtn.widget->isVisible = 0;
        mmLoginBtn.widget->isVisible = 0;
        mmLogOutBtn.widget->isVisible = 1;
    }

    getmaxyx(stdscr, scrH, scrW);
    resizeFrameBuffer(&mmFrameBuffer, scrW, scrH);


    mmCamera.x = -scrW/2;
    mmCamera.y = -scrH/2;
    mmCamera.w = scrW;
    mmCamera.h = scrH;

    curs_set(0);
    keypad(stdscr, TRUE);       
    noecho();
    //cbreak();
    mouseinterval(1);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    timeout(100);
    printf("\033[?1003h\n");
    changeAudio(getAudioByName("title"), 2000);
}
void updateMainMenu(int ch){
    if(ch == KEY_RESIZE){
        getmaxyx(stdscr, scrH, scrW);
        mmCamera.x = -scrW / 2;
        mmCamera.y = -scrH / 2;
        mmCamera.w = scrW;
        mmCamera.h = scrH;
        resizeFrameBuffer(&mmFrameBuffer, scrW, scrH);
    }
}

void renderMainMenu(){
    erase();
    emptyFrameBuffer(mmFrameBuffer);
    emptyFrameBuffer(uiFrameBuffer);

    void** tmp = mmUiList.data;
    FOR(i, mmUiList.size){
        ((UiBase*)tmp[1])->render(((UiBase*)tmp[1])->object);
        tmp = *tmp;
    }

    renderDepthlessTexture(mmBackGround2, (scrW / 2) - mmBackGround2->w + 40 + (scrW & 1) , (scrH / 2) - mmBackGround2->h + (scrH & 1), 0, &mmCamera, mmFrameBuffer);
    renderDepthlessTexture(mmBackGround, -(scrW / 2) - (scrW & 1) - 20 , (scrH / 2) - mmBackGround->h + (scrH & 1), 10, &mmCamera, mmFrameBuffer);

    renderFrameBuffer(mmFrameBuffer);
    renderFrameBuffer(uiFrameBuffer);


    if(account.username){
        mvprintw(2, 2, "Logged in as: %s", account.username);
    }else{
        mvprintw(2, 2, "Not logged in");

    }
    refresh();
}