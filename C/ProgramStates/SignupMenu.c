#include <string.h>

#include "SignupMenu.h"
#include "MainMenu.h"

#include "../Utilities/cJSON.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Button.h"
#include "../UiElements/PopUp.h"
#include "../UiElements/Widget.h"
#include "../UiElements/UiBase.h"
#include "../Utilities/LinkedList.h"
#include "../Globals.h"


Widget suFormWidget;

Button suBackButton;
Button suSignupButton;
Button suRandomPasswordBtn;

TextBox suUserNameTb;
TextBox suEmailTb;
TextBox suPasswordTb;

PopUp* suInvalidPopup;

char suEnteredUsername[40];
char suEnteredPssword[40];
char suEnteredEmail[40];

LinkedList suUiList;
LinkedList suTextBoxList;


EngineState signUpMenu = {&enterSignUpMenu, &updateSignUpMenu, &renderSignUpMenu, &exitSignUpMenu};

void** suTmpIterPtr;
UiBase* suIterPtr;

void closeInvalidPopup(){
    deletePopUp(suInvalidPopup);
    suInvalidPopup = NULL;
}
int isEmailValid(char* email){
    int size = strlen(email);
    int valid = 1;
    int partSize = 0;
    int seenAtsign = 0;
    int seenDot = 0;

    FOR(i, size){
        if(email[i] == '@'){
            if((!partSize) || (seenAtsign)){
                valid = 0;
                break;
            }
            seenAtsign = 1;
            partSize = 0;
        }else if(email[i] == '.'){
            if((!partSize) || (!seenAtsign) || (seenDot)){
                valid = 0;
                break;
            }
            seenDot = 1;
            partSize = 0;
        }else{
            partSize++;
        }
    }
    return ((seenAtsign) && (seenDot) && (partSize));
}
int isPasswordValid(char* pass){
    int passSize = 0;
    int hasCap = 0;
    int hasLow = 0;
    int hasNum = 0;
    passSize = strlen(pass);
    FOR(i, passSize){
        if((pass[i] >= 'A') &&  (pass[i] <= 'Z')) hasCap = 1;
        else if((pass[i] >= 'a') &&  (pass[i] <= 'z')) hasLow = 1;
        else if((pass[i] >= '0') &&  (pass[i] <= '9')) hasNum = 1;
    }
    return ((passSize >= 7) && hasLow && hasCap && hasNum);
}
void returnToMainMenu(){
    removeItemFromLinkedList(&suUiList, suInvalidPopup->uiBase);
    deletePopUp(suInvalidPopup);
    suInvalidPopup = NULL;

    enterMainMenu();
}
void signUp(){
    if(suEnteredUsername[0] == '\0'){
        suInvalidPopup = createPopUp("Please Enter a username for your acount.", NULL, 20, 20, &closeInvalidPopup);
        return;
    }else if(suEnteredPssword[0] == '\0'){
        suInvalidPopup = createPopUp("Please Enter a password for your acount.", NULL, 20, 20, &closeInvalidPopup);
        return;
    }else if(suEnteredEmail[0] == '\0'){
        suInvalidPopup = createPopUp("Please Enter a email for your acount.", NULL, 20, 20, &closeInvalidPopup);

        return;
    }else{
        if(isPasswordValid(suEnteredPssword)){
            if(isEmailValid(suEnteredEmail)){
                char* usersData;

                if(fileToStr(playerDbAddress, &usersData)){
                    cJSON* json;
                    int new = 1;
                    cJSON* user;
                    char* buffer;
                    if(usersData[0]){
                        json = cJSON_Parse(usersData);
                        if(json){
                            user = cJSON_GetArrayItem(json, 0);
                            while(user){
                                buffer = cJSON_GetObjectItem(user, "username")->valuestring;
                                if(buffer){
                                    if(!strcmp(suEnteredUsername, buffer)){
                                        new = 0;
                                        break;
                                    }
                                }
                                user = user->next;
                            }   
                        }
                    }else{
                        json = cJSON_CreateArray();
                    }
                    if(new){
                        user = cJSON_CreateObject();
                        cJSON_AddStringToObject(user, "username", suEnteredUsername);
                        cJSON_AddStringToObject(user, "password", suEnteredPssword);
                        cJSON_AddStringToObject(user, "email", suEnteredEmail);
                        cJSON_AddNumberToObject(user, "gameNum", 0);
                        cJSON_AddNumberToObject(user, "firstGame", 0);
                        cJSON_AddNumberToObject(user, "golds", 0);
                        cJSON_AddNumberToObject(user, "record", 0);

                        cJSON_AddItemToArray(json, user);
                        saveJsonToFile(playerDbAddress, json);
                        suInvalidPopup = createPopUp("User created. you need to log in.", NULL, 20, 20, &returnToMainMenu);


                    }else{
                        suInvalidPopup = createPopUp("Username exists.", NULL, 20, 20, &closeInvalidPopup);

                    }
                    cJSON_free(json);

                }else{
                    suInvalidPopup = createPopUp("Cant open the players data file. consider changing the address in the settings.", NULL, 20, 20, &closeInvalidPopup);
                    return;
                }

            }else{
                suInvalidPopup = createPopUp("Please enter a valid email.", NULL, 20, 20, &closeInvalidPopup);
                return;
            }
        }else{
            suInvalidPopup = createPopUp("Password should at least be 7 charachters, and contain an uppercase, a lowercase and a digit.", NULL, 20, 20, &closeInvalidPopup);
            return;
        }
    }
}
void randomPassword(){
    resetTextbox(&suPasswordTb);
    int tmp1 = 0, tmp2 = 0;

    suEnteredPssword[0] = randBetween('A', 'Z'+1, 1);
    suEnteredPssword[1] = randBetween('a', 'z'+1, 2);
    suEnteredPssword[2] = randBetween('0', '9'+1, 3);

    for(int i = 3; i < 17; i++){
        suEnteredPssword[i] = randBetween(33, 'z'+1, i);
    }

    tmp1 = randBetween(1, 17, 0);

    char buffer[17];

    FOR(i, 17){
        buffer[((i + 1) * tmp1)%17] = suEnteredPssword[i];
    }
    FOR(i, 17){
        suEnteredPssword[i] = buffer[i];
    }
    suEnteredPssword[17] = '\0';
}
void initSignUpMenu(){
    createLinkedList(&suUiList, sizeof(UiBase*));
    createLinkedList(&suTextBoxList, sizeof(TextBox*));

    createWidget(&suFormWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, NULL);
    suFormWidget.layoutType = ABSOLUTE;
    suFormWidget.bordered = 1;

    createButton(&suBackButton, &suFormWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 6);
    createButton(&suSignupButton, &suFormWidget, "Sign Up", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 11);
    createButton(&suRandomPasswordBtn, &suFormWidget, "Random", ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 16, 11);

    
    createTextBox(&suUserNameTb, &suFormWidget, "Username", suEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 6, 90);
    createTextBox(&suPasswordTb, &suFormWidget, "Password", suEnteredPssword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 11, 90);
    createTextBox(&suEmailTb, &suFormWidget, "Email", suEnteredEmail, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 18, 90);

    linkedListPushBack(&suUiList, suFormWidget.uiBase);
    linkedListPushBack(suFormWidget.children, suBackButton.uiBase);
    linkedListPushBack(suFormWidget.children, suSignupButton.uiBase);
    linkedListPushBack(suFormWidget.children, suRandomPasswordBtn.uiBase);
    linkedListPushBack(suFormWidget.children, suUserNameTb.uiBase);
    linkedListPushBack(suFormWidget.children, suPasswordTb.uiBase);
    linkedListPushBack(suFormWidget.children, suEmailTb.uiBase);

    linkedListPushBack(&suTextBoxList, &suEmailTb);
    linkedListPushBack(&suTextBoxList, &suPasswordTb);
    linkedListPushBack(&suTextBoxList, &suUserNameTb);




    suBackButton.callBack = maineMenu.enter;
    suSignupButton.callBack = &signUp;
    suRandomPasswordBtn.callBack = &randomPassword;
}

void enterSignUpMenu(){
    suTmpIterPtr = suTextBoxList.data;
    FOR(i, suTextBoxList.size){
        suIterPtr = suTmpIterPtr[1];
        resetTextbox(suIterPtr);
        suTmpIterPtr = suTmpIterPtr[0];
    }

    clear();
    refresh();
    
    engineState = &signUpMenu;

    curs_set(0);   
    noecho();
    cbreak();
}
void updateSignUpMenu(){
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
                        mousex = mEvent.x;
                        mousey = mEvent.y;
                        if(suInvalidPopup != NULL){
                            buttonMouseMoveCallback(suInvalidPopup->close);
                        }else{
                            suTmpIterPtr = suUiList.data;
                            FOR(i, suUiList.size){
                                suIterPtr = suTmpIterPtr[1];
                                suIterPtr->mouseMove(suIterPtr->object);
                                suTmpIterPtr = suTmpIterPtr[0];
                            }
                        }
                        
                        
                        break;
                    default:
                        if(suInvalidPopup != NULL){
                            buttonMouseClickEvent(suInvalidPopup->close);
                        }else{
                            suTmpIterPtr = suUiList.data;
                            FOR(i, suUiList.size){
                                suIterPtr = suTmpIterPtr[1];
                                suIterPtr->mouseClick(suIterPtr->object);
                                suTmpIterPtr = suTmpIterPtr[0];
                            }
                        }
                        break;
                }
            }
            break;
        case ERR:
            break;
        default:
            if(suInvalidPopup == NULL){
                suTmpIterPtr = suUiList.data;
                FOR(i, suUiList.size){
                    suIterPtr = suTmpIterPtr[1];
                    suIterPtr->keyPress(suIterPtr->object, ch);
                    suTmpIterPtr = suTmpIterPtr[0];
                }
            }
            break;
            
    }
}
void renderSignUpMenu(){

    erase();
    updateWidgetChildren(&suFormWidget);
    suTmpIterPtr = suUiList.data;
    FOR(i, suUiList.size){
        suIterPtr = suTmpIterPtr[1];
        suIterPtr->render(suIterPtr->object);
        suTmpIterPtr = suTmpIterPtr[0];
    }
    if(suInvalidPopup) renderPopup(suInvalidPopup);
    refresh();
    
}
void exitSignUpMenu(){

}