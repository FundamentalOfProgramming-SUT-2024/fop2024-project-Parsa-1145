#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "LogInMenu.h"
#include "MainMenu.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "../UiElements/Widget.h"
#include "../UiElements/Button.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/PopUp.h"
#include "../UiElements/UiBase.h"
#include "../Utilities/LinkedList.h"
#include "../Utilities/cJSON.h"



char liEnteredUsername[50];
char liEnteredPassword[50];

TextBox usernameTb, passwordTb;
Button limLoginBtn, limBackBtn;
LinkedList liUiList;
LinkedList liTextBoxList;
Widget formWidget;
PopUp* liInvalidPopup;


EngineState logInMenu = {&enterLogInMenu, &updateLogInMenu, &renderLogInMenu, &exitLogInMenu};

void** liTmpIterPtr;
UiBase* liIterPtr;

void liCloseInvalidPopup(){
    deletePopUp(liInvalidPopup);
    liInvalidPopup = NULL;
}
void liReturnToMainMenu(){
    deletePopUp(liInvalidPopup);
    liInvalidPopup = NULL;

    enterMainMenu();
}

void logIn(){
    if(liEnteredUsername[0]){
        char* usersData;
        if(fileToStr(playerDbAddress, &usersData)){
            cJSON* json = cJSON_Parse(usersData);
            cJSON* user;
            char* buffer;
            int found = 0;
            if(json){
                user = cJSON_GetArrayItem(json, 0);
                while(user){
                    buffer = cJSON_GetObjectItem(user, "username")->valuestring;
                    if(buffer){
                        if(!strcmp(liEnteredUsername, buffer)){
                            if(!strcmp(liEnteredPassword, cJSON_GetObjectItem(user, "password")->valuestring)){
                                found = 1;
                                loadAccount(user, &account);

                                liInvalidPopup = createPopUp("Logged in!", NULL, 20, 20, &liReturnToMainMenu);
                        
                                break;
                            }
                        }
                    }
                    user = user->next;
                }
            }
            if(!found){
                liInvalidPopup = createPopUp("Username password pair not found.", NULL, 20, 20, &liCloseInvalidPopup);
        
            }
            cJSON_Delete(json);
        }else{
            liInvalidPopup = createPopUp("Cant open the players data file. consider changing the address in the settings.", NULL, 20, 20, &liCloseInvalidPopup);
    
            return;
        }
        
    }else{
        liInvalidPopup = createPopUp("Please enter a username", NULL, 20, 20, &liCloseInvalidPopup);

        return;
    }
}

void initLogInMenu(){
    createLinkedList(&liUiList, sizeof(UiBase*));
    createLinkedList(&liTextBoxList, sizeof(TextBox*));
    
    createWidget(&formWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, NULL);
    formWidget.bordered = 1;
    createButton(&limLoginBtn, &formWidget, "Log In", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 60);
    createButton(&limBackBtn, &formWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 6);
    createTextBox(&usernameTb, &formWidget, "Username", liEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 1, 4, 90);
    createTextBox(&passwordTb, &formWidget, "Password", liEnteredPassword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 1, 8, 90);

    linkedListPushBack(&liUiList, formWidget.uiBase);
    linkedListPushBack(formWidget.children, limLoginBtn.uiBase);
    linkedListPushBack(formWidget.children, limBackBtn.uiBase);
    linkedListPushBack(formWidget.children, usernameTb.uiBase);
    linkedListPushBack(formWidget.children, passwordTb.uiBase);

    linkedListPushBack(&liTextBoxList, &usernameTb);
    linkedListPushBack(&liTextBoxList, &passwordTb);


    limBackBtn.callBack = maineMenu.enter;
    limLoginBtn.callBack = logIn;
}

void enterLogInMenu(){

    liTmpIterPtr = liTextBoxList.data;
    FOR(i, liTextBoxList.size){
        liIterPtr = liTmpIterPtr[1];
        resetTextbox(liIterPtr);
        liTmpIterPtr = liTmpIterPtr[0];
    }

    clear();
    refresh();
    engineState = &logInMenu;

    curs_set(0);   
    noecho();
    cbreak();
    mouseinterval(1);
    timeout(100);

}

void updateLogInMenu(){
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
                        if(liInvalidPopup){
                            popupMouseMove(liInvalidPopup);
                        }else{
                            liTmpIterPtr = liUiList.data;
                            FOR(i, liUiList.size){
                                liIterPtr = liTmpIterPtr[1];
                                liIterPtr->mouseMove(liIterPtr->object);
                                liTmpIterPtr = liTmpIterPtr[0];
                            }
                        }
                        break;
                    default:
                        if(liInvalidPopup){
                            popupMouseClick(liInvalidPopup);
                        }else{
                            liTmpIterPtr = liUiList.data;
                            FOR(i, liUiList.size){
                                liIterPtr = liTmpIterPtr[1];
                                liIterPtr->mouseClick(liIterPtr->object);
                                liTmpIterPtr = liTmpIterPtr[0];
                            }
                        }
                        break;
                }
            }
            break;
        case ERR:
            break;
        default:
            if(!liInvalidPopup){
                liTmpIterPtr = liUiList.data;
                FOR(i, liUiList.size){
                    liIterPtr = liTmpIterPtr[1];
                    liIterPtr->keyPress(liIterPtr->object, ch);
                    liTmpIterPtr = liTmpIterPtr[0];
                }
            }
            break;
    }
}

void renderLogInMenu(){
    erase();
    updateWidgetChildren(&formWidget);
    liTmpIterPtr = liUiList.data;
    FOR(i, liUiList.size){
        liIterPtr = liTmpIterPtr[1];
        liIterPtr->render(liIterPtr->object);
        liTmpIterPtr = liTmpIterPtr[0];
    }
    if(liInvalidPopup) renderPopup(liInvalidPopup);
    refresh();
}
void exitLogInMenu(){

}