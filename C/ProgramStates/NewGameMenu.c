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
Widget ngmSideBar;

LinkedList ngmUiList;

EngineState newGameMenu = {&enterNewGameMenu, &updateNewGameMenu, &renderNewGameMenu, &exitNewGameMenu};


void** ngmTmpIterPtr;
UiBase* ngmIterPtr;
void initNewGameMenu(){
    createLinkedList(&ngmUiList, sizeof(UiBase*));

    createWidget(&ngmSideBar, NULL, ABSOLUTE, RELATIVE, ALIGN_LEFT, ALIGN_CENTER, 0, 0, 40, 100, C_BG_BLACK);
    ngmSideBar.bordered = 1;

    createButton(&ngmStart, &ngmSideBar, "Start", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 60);
    createButton(&ngmBackBtn, &ngmSideBar, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 6);

    linkedListPushBack(ngmSideBar.children, ngmStart.uiBase);
    linkedListPushBack(ngmSideBar.children, ngmBackBtn.uiBase);

    linkedListPushBack(&ngmUiList, ngmSideBar.uiBase);


    
    ngmBackBtn.callBack = maineMenu.enter;
    ngmStart.callBack = mainGame.enter;
}

void enterNewGameMenu(){
    clear();
    refresh();
    engineState = &newGameMenu;

    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->update(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }
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

                        ngmTmpIterPtr = ngmUiList.data;
                        while(ngmTmpIterPtr){
                            ngmIterPtr = ngmTmpIterPtr[1];
                            ngmIterPtr->mouseMove(ngmIterPtr->object);
                            ngmTmpIterPtr = ngmTmpIterPtr[0];
                        }
                        break;
                    default:
                        ngmTmpIterPtr = ngmUiList.data;
                        while(ngmTmpIterPtr){
                            ngmIterPtr = ngmTmpIterPtr[1];
                            ngmIterPtr->mouseClick(ngmIterPtr->object);
                            ngmTmpIterPtr = ngmTmpIterPtr[0];
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
    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->update(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }

    ngmTmpIterPtr = ngmUiList.data;
    while(ngmTmpIterPtr){
        ngmIterPtr = ngmTmpIterPtr[1];
        ngmIterPtr->render(ngmIterPtr->object);
        ngmTmpIterPtr = ngmTmpIterPtr[0];
    }

    refresh();
}
void exitNewGameMenu(){

}