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
Widget formWidget;
PopUp* liInvalidPopup;


EngineState logInMenu = {&enterLogInMenu, &updateLogInMenu, &renderLogInMenu, &exitLogInMenu};

void liCloseInvalidPopup(){
    removeItemFromLinkedList(&liUiList, liInvalidPopup->uiBase);
    deletePopUp(liInvalidPopup);
    liInvalidPopup = NULL;
}
void liReturnToMainMenu(){
    removeItemFromLinkedList(&liUiList, liInvalidPopup->uiBase);
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
                                account.username = malloc(strlen(cJSON_GetObjectItem(user, "username")->valuestring)+1);
                                account.password = malloc(strlen(cJSON_GetObjectItem(user, "password")->valuestring)+1);
                                account.email = malloc(strlen(cJSON_GetObjectItem(user, "email")->valuestring)+1);
                                strcpy(account.username, cJSON_GetObjectItem(user, "username")->valuestring);
                                strcpy(account.email, cJSON_GetObjectItem(user, "email")->valuestring);
                                strcpy(account.password, cJSON_GetObjectItem(user, "password")->valuestring);
                                account.firstPlayTime = cJSON_GetObjectItem(user, "firstGame")->valuestring;
                                account.gamesFinished = cJSON_GetObjectItem(user, "gameNum")->valuestring;
                                account.goldsCollected = cJSON_GetObjectItem(user, "golds")->valuestring;
                                account.goldRecord = cJSON_GetObjectItem(user, "record")->valuestring;

                                liInvalidPopup = createPopUp("Logged in!", NULL, 20, 20, &liReturnToMainMenu);
                                linkedListPushBack(&liUiList, liInvalidPopup->uiBase);
                                break;
                            }
                        }
                    }
                    user = user->next;
                }
            }
            if(!found){
                liInvalidPopup = createPopUp("Username password pair not found.", NULL, 20, 20, &liCloseInvalidPopup);
                linkedListPushBack(&liUiList, liInvalidPopup->uiBase);
            }
            cJSON_Delete(json);
        }else{
            liInvalidPopup = createPopUp("Cant open the players data file. consider changing the address in the settings.", NULL, 20, 20, &liCloseInvalidPopup);
            linkedListPushBack(&liUiList, liInvalidPopup->uiBase);
            return;
        }
        
    }else{
        liInvalidPopup = createPopUp("Please enter a username", NULL, 20, 20, &liCloseInvalidPopup);
        linkedListPushBack(&liUiList, liInvalidPopup->uiBase);
        return;
    }
}

void initLogInMenu(){
    createLinkedList(&liUiList, sizeof(UiBase*));
    
    createWidget(&formWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, C_BG_GRAY0);
    createButton(&limLoginBtn, &formWidget, "Log In", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&limBackBtn, &formWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 3, 2, 6);
    createTextBox(&usernameTb, &formWidget, "Username", liEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 90);
    createTextBox(&passwordTb, &formWidget, "Password", liEnteredPassword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 8, 90);

    linkedListPushBack(&liUiList, formWidget.uiBase);
    linkedListPushBack(&liUiList, limLoginBtn.uiBase);
    linkedListPushBack(&liUiList, limBackBtn.uiBase);
    linkedListPushBack(&liUiList, usernameTb.uiBase);
    linkedListPushBack(&liUiList, passwordTb.uiBase);

    limBackBtn.callBack = maineMenu.enter;
    limLoginBtn.callBack = logIn;
}

void enterLogInMenu(){

    void** tmp = liUiList.data;
    UiBase* tmp1;
    FOR(i, liUiList.size){
        tmp1 = tmp[1];
        if(tmp1->type == UI_TYPE_TEXTBOX){
            resetTextbox(tmp1->object);
        }
        tmp = tmp[0];
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
    static void** tmp;
    static UiBase* tmp1;
    

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
                            tmp = liUiList.data;
                            FOR(i, liUiList.size){
                                tmp1 = tmp[1];
                                tmp1->mouseMove(tmp1->object);
                                tmp = tmp[0];
                            }
                        }
                        break;
                    default:
                        if(liInvalidPopup){
                            popupMouseClick(liInvalidPopup);
                        }else{
                            tmp = liUiList.data;
                            FOR(i, liUiList.size){
                                tmp1 = tmp[1];
                                tmp1->mouseClick(tmp1->object);
                                tmp = tmp[0];
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
                tmp = liUiList.data;
                FOR(i, liUiList.size){
                    tmp1 = tmp[1];
                    tmp1->keyPress(tmp1->object, ch);
                    tmp = tmp[0];
                }
            }
            break;
    }
}

void renderLogInMenu(){
    static void** tmp;
    static UiBase* tmp1;

    erase();
    tmp = liUiList.data;
    FOR(i, liUiList.size){
        tmp1 = tmp[1];
        tmp1->render(tmp1->object);
        tmp = tmp[0];
    }
    refresh();
}
void exitLogInMenu(){

}