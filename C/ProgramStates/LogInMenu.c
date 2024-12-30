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



char enteredUsername[30];
char enteredPassword[30];

TextBox usernameTb, passwordTb;
TextBox* logInMenuTextBoxList[2] = {&usernameTb, &passwordTb};
Button limLoginBtn, limBackBtn;
Button* logInMenuButtonList[2] = {&limLoginBtn, &limBackBtn};
Widget formWidget;

EngineState logInMenu = {&enterLogInMenu, &updateLogInMenu, &renderLogInMenu, &exitLogInMenu};

void logIn(){
    FILE* playerDb = fopen(playerDbAddress, "r");

    if(playerDb != NULL){
        move(0, 0);
        printw("%s ", playerDbAddress);
        refresh();
        char tmp[100];
        while(!feof(playerDb)){
            freadBinStr(playerDb, tmp);
            if(!strcmp(tmp, "#")){
                freadBinStr(playerDb, NULL);
                freadBinStr(playerDb, tmp);

                if(!strcmp(tmp, enteredUsername)){
                    freadBinStr(playerDb, NULL);
                    fread(tmp,1,  strlen(enteredUsername), playerDb);
                    
                    char* tmpHash = hashWithKey(enteredUsername, enteredPassword);

                    if(!strncmp(tmp, tmpHash, strlen(enteredUsername))){
                        maineMenu.enter();
                        free(tmpHash);
                        break;
                    }
                    free(tmpHash);
                }
            }
        }
    }
    fclose(playerDb);
}

void initLogInMenu(){
    createWidget(&formWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 50, 20, C_BG_GRAY0);
    createButton(&limLoginBtn, &formWidget, "Log In", RELATIVE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 1, 60);
    createButton(&limBackBtn, &formWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 3, 2, 6);
    createTextBox(&usernameTb, &formWidget, "Username", enteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 4, 90);
    createTextBox(&passwordTb, &formWidget, "Password", enteredPassword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ALIGN_TOP, 0, 8, 90);

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
                        FOR(i, 2){
                            buttonMouseMoveCallback(logInMenuButtonList[i]);
                        }
                        FOR(i, 2){
                            textBoxMouseMoveCallback(logInMenuTextBoxList[i]);
                        }
                        break;
                    default:
                        FOR(i, 2){
                            buttonMouseClickEvent(logInMenuButtonList[i]);
                        }
                        FOR(i, 2){
                            textBoxMouseClickCallback(logInMenuTextBoxList[i]);
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
    refresh();
}
void exitLogInMenu(){

}