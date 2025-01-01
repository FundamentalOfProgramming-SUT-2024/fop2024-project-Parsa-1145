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
#include "../Utilities/cJSON.h"



char liEnteredUsername[50];
char liEnteredPassword[50];

TextBox usernameTb, passwordTb;
TextBox* logInMenuTextBoxList[2] = {&usernameTb, &passwordTb};
Button limLoginBtn, limBackBtn;
Button* logInMenuButtonList[2] = {&limLoginBtn, &limBackBtn};
Widget formWidget;
PopUp* liInvalidPopup;

EngineState logInMenu = {&enterLogInMenu, &updateLogInMenu, &renderLogInMenu, &exitLogInMenu};

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
    createWidget(&formWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, C_BG_GRAY0);
    createButton(&limLoginBtn, &formWidget, "Log In", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&limBackBtn, &formWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 3, 2, 6);
    createTextBox(&usernameTb, &formWidget, "Username", liEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 90);
    createTextBox(&passwordTb, &formWidget, "Password", liEnteredPassword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 8, 90);

    limBackBtn.callBack = maineMenu.enter;
    limLoginBtn.callBack = logIn;
}

void enterLogInMenu(){
    FOR(i, 2){
        resetTextbox(logInMenuTextBoxList[i]);
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
                        mousex = mEvent.x;
                        mousey = mEvent.y;
                        if(!liInvalidPopup){
                            FOR(i, 2){
                                buttonMouseMoveCallback(logInMenuButtonList[i]);
                            }
                            FOR(i, 2){
                                textBoxMouseMoveCallback(logInMenuTextBoxList[i]);
                            }
                        }else{
                            buttonMouseMoveCallback(liInvalidPopup->close);
                        }
                        break;
                    default:
                        if(!liInvalidPopup){
                            FOR(i, 2){
                                buttonMouseClickEvent(logInMenuButtonList[i]);
                            }
                            FOR(i, 2){
                                textBoxMouseClickCallback(logInMenuTextBoxList[i]);
                            }
                        }else{
                            buttonMouseClickEvent(liInvalidPopup->close);
                        }
                        break;
                }
            }
            break;
        case ERR:
            break;
        default:
            FOR(i, 2){
                textBoxKeyPressCallback(logInMenuTextBoxList[i], ch);
            }
            break;
            
    }
}

void renderLogInMenu(){
    erase();
    renderWidget(&formWidget);
    FOR(i, 2){
        renderButton(logInMenuButtonList[i]);
    }
    FOR(i, 2){
        renderTextBox(logInMenuTextBoxList[i]);
    }
    if(liInvalidPopup){
        renderPopup(liInvalidPopup);
    }
    refresh();
}
void exitLogInMenu(){

}