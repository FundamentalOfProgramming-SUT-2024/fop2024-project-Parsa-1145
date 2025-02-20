#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>
#include <pthread.h>

#include "ProgramStates/MainMenu.h"
#include "ProgramStates/NewGameMenu.h"
#include "ProgramStates/LogInMenu.h"
#include "ProgramStates/MainGame.h"
#include "ProgramStates/SignupMenu.h"
#include "ProgramStates/Scoreboard.h"
#include "ProgramStates/RollDialouge.h"
#include "ProgramStates/SettingsMenu.h"
#include "SecondaryThread.h"
#include "UiElements/PopUp.h"
#include "GlobalDefines.h"
#include "Globals.h"


int main(){
    setlocale(LC_ALL, "");

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(88200, MIX_DEFAULT_FORMAT, 2, 1024);

    initscr();
    start_color();
    init_extended_color(10, 150, 150, 150);
    init_extended_color(12, 100, 100, 100);
    init_extended_pair(C_BG_GRAY0, COLOR_WHITE, 10);
    init_extended_pair(C_BG_GRAY1, COLOR_WHITE, 12);
    init_extended_pair(C_BG_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_extended_pair(C_BG_WHITE, COLOR_BLACK, COLOR_WHITE);

    resetRgbColors();


    getmaxyx(stdscr, scrH, scrW);
    globalCamera.h = scrH;
    globalCamera.w = scrW;
    globalCamera.x = -scrW / 2;
    globalCamera.y = -scrH / 2;

    uiFrameBuffer = createFrameBuffer(scrW, scrH);
    globalFrameBuffer = createFrameBuffer(scrW, scrH);

    initSettings();
    initMainMenu();
    initSettingsMenu();
    initNewGameMenu();
    initLogInMenu();
    initMainGame();
    initSignUpMenu();
    initScoreBoard();

    initSecondaryThread();
    
    {   
        itemsJson = openJsonFile("../Data/Items.json");
        itemTypes = openJsonFile("../Data/ItemTypes.json");
    }

    gameSettings.difficultyName = NULL;

    engineState = &maineMenu;

    changeAudio(getAudioByName("curroption"), 1000);
    startRollingDialouge("rougeLogo", simpleFade, 9000, 5, 1000, 1000, 400, 400, &enterMainMenu);

    int ch;
    void** tmp;
    UiBase* ptr;
    while(!terminate){
        ch = getch();
        switch (ch){
        case KEY_RESIZE:
            getmaxyx(stdscr, scrH, scrW);
            resizeFrameBuffer(&uiFrameBuffer, scrW, scrH);
            resizeFrameBuffer(&globalFrameBuffer, scrW, scrH);

            globalCamera.h = scrH;
            globalCamera.w = scrW;
            globalCamera.x = -scrW / 2 + (scrW & 1);
            globalCamera.y = -scrH / 2+ (scrH & 1);
            clear();
            break;
        case KEY_MOUSE:
            if(engineState->uiList){

                if(hoveredElement && (!hoveredElement->isHovered(hoveredElement))){
                    hoveredElement = NULL;
                    hoveredZ = 0;
                }
                tmp = engineState->uiList->data;
                while(tmp){
                    ptr = tmp[1];
                    tmp = tmp[0];
                    ptr->isHovered(ptr);
                    if(ptr->type == UI_TYPE_WIDGET){
                        updateChildrenHovered(ptr);
                    }
                }
                if(getmouse(&mEvent) == OK){
                    switch(mEvent.bstate){
                        case KEY_MOUSE_MOVE:
                            tmp = engineState->uiList->data;
                            while(tmp){
                                ptr = tmp[1];
                                tmp = tmp[0];
                                ptr->mouseMove(ptr->object);
                            }
                            break;
                        default:
                            tmp = engineState->uiList->data;
                            while(tmp){
                                ptr = tmp[1];
                                tmp = tmp[0];
                                ptr->mouseClick(ptr->object);
                            }
                            break;
                    }
                }
            }
            break;
        case ERR:
            break;
        default:
            if(engineState->uiList){
                if(hoveredElement && (!hoveredElement->isHovered(hoveredElement))){
                    hoveredElement = NULL;
                    hoveredZ = 0;
                }
                tmp = engineState->uiList->data;
                while(tmp){
                    ptr = tmp[1];
                    tmp = tmp[0];
                    ptr->isHovered(ptr);
                    if(ptr->type == UI_TYPE_WIDGET){
                        updateChildrenHovered(ptr);
                    }
                }
                tmp = engineState->uiList->data;
                while(tmp){
                    ptr = tmp[1];
                    tmp = tmp[0];
                    ptr->keyPress(ptr->object, ch);
                }
            }
            break;
        }
        (*(engineState->update))(ch);

        if(engineState->uiList){
            tmp = engineState->uiList->data;
            while(tmp){
                ptr = tmp[1];
                tmp = tmp[0];
                ptr->update(ptr->object);
            }
        }

        (*(engineState->render))();

    }

    pthread_join(secondaryThread, NULL);
    freeAudioManager();



    refresh();
    endwin();
}
