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
#include "../UiElements/ComboBox.h"
 
Button ngmStart;
Button ngmBackBtn;
Widget ngmSideBar;

TextWidget ngmUserTextWidget;
TextWidget ngmPlayerColorTextWidget;
Widget ngmPlayerColorWidget;
TextBox ngmPlayerCrTextBox;
TextBox ngmPlayerCgTextBox;
TextBox ngmPlayerCbTextBox;

TextWidget ngmGameSettingsLabel;
Widget ngmDifficultyWidget;
TextWidget ngmDifficaultyLabel;
ComboBox ngmDifficultyCombo;

LinkedList ngmUiList;

EngineState newGameMenu = {&enterNewGameMenu, &updateNewGameMenu, &renderNewGameMenu, &exitNewGameMenu};


void** ngmTmpIterPtr;
UiBase* ngmIterPtr;
void initNewGameMenu(){
    createLinkedList(&ngmUiList, sizeof(UiBase*));

    createWidget(&ngmSideBar, NULL, ABSOLUTE, RELATIVE, ALIGN_LEFT, ALIGN_CENTER, 0, 0, 40, 100, C_BG_BLACK);
    ngmSideBar.layoutType = VERTICAL_LAYOUT;
    ngmSideBar.bordered = 1;

    createButton(&ngmStart, &ngmSideBar, "Start", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&ngmBackBtn, &ngmSideBar, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 1, 1, 6);

    {
        createTextWidget(&ngmUserTextWidget, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 3, "Playing as:");
        createTextWidget(&ngmPlayerColorTextWidget, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 3, "Charachter color:");

        createWidget(&ngmPlayerColorWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 1, 0, 100, 6, NULL);
        ngmPlayerColorWidget.bordered = 0;
        {
            createNumberInput(&ngmPlayerCrTextBox, &ngmPlayerColorWidget, "Red", player.color, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 1, 0, 12);
            createNumberInput(&ngmPlayerCgTextBox, &ngmPlayerColorWidget, "Green", player.color+1, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 13, 0, 12);
            createNumberInput(&ngmPlayerCbTextBox, &ngmPlayerColorWidget, "Blue", player.color+2, 5, 0, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 25, 0, 12);
            
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCrTextBox.uiBase);
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCgTextBox.uiBase);
            linkedListPushBack(ngmPlayerColorWidget.children, ngmPlayerCbTextBox.uiBase);
        }

        createTextWidget(&ngmGameSettingsLabel, &ngmSideBar, ALIGN_LEFT, WITH_PARENT, 1, 1, "Game settings:");
        createWidget(&ngmDifficultyWidget, &ngmSideBar, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 2, 0, 100, 1, NULL);
        {
            createTextWidget(&ngmDifficaultyLabel, &ngmDifficultyWidget, ABSOLUTE, ABSOLUTE, 0, 0, "Difficulty");
            createComboBox(&ngmDifficultyCombo, &ngmDifficultyWidget, ABSOLUTE, ABSOLUTE, ABSOLUTE, ABSOLUTE, 11, 0, 15, 1, C_BG_GRAY0);
            {
                comboBoxAddOption(&ngmDifficultyCombo, "Easy");
                comboBoxAddOption(&ngmDifficultyCombo, "Nurmal");
                comboBoxAddOption(&ngmDifficultyCombo, "Hard");
                comboBoxAddOption(&ngmDifficultyCombo, "Nightmare");
            }
            linkedListPushBack(ngmDifficultyWidget.children, ngmDifficaultyLabel.uiBase);
            linkedListPushBack(ngmDifficultyWidget.children, ngmDifficultyCombo.uiBase);
        }

        linkedListPushBack(ngmSideBar.children, ngmUserTextWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmPlayerColorTextWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmPlayerColorWidget.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmGameSettingsLabel.uiBase);
        linkedListPushBack(ngmSideBar.children, ngmDifficultyWidget.uiBase);
    }



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

    player.color[0] = 5;
    player.color[1] = 5;
    player.color[2] = 5;

    changeTextWidget(&ngmPlayerColorTextWidget, "Character color: %i@%O", player.color, player.color+1, player.color+2);

    if(account.username){
        changeTextWidget(&ngmUserTextWidget, "Playing as: %S", account.username);
    }else{
        changeTextWidget(&ngmUserTextWidget, "Playing as: Guest");
    }
    

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
                    case KEY_MOUSE_MOVE:
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
            ngmTmpIterPtr = ngmUiList.data;
            while(ngmTmpIterPtr){
                ngmIterPtr = ngmTmpIterPtr[1];
                ngmIterPtr->keyPress(ngmIterPtr->object, ch);
                ngmTmpIterPtr = ngmTmpIterPtr[0];
            }
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