#include <string.h>

#include "SignupMenu.h"
#include "MainMenu.h"

#include "../Utilities/cJSON.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/Button.h"
#include "../UiElements/PopUp.h"
#include "../UiElements/Widget.h"
#include "../Globals.h"


Widget suFormWidget;

Button suBackButton;
Button suSignupButton;
Button suRandomPasswordBtn;
Button* suButtonList[3] = {&suBackButton, &suSignupButton, &suRandomPasswordBtn};
int suButtonNum = 3;

TextBox suUserNameTb;
TextBox suEmailTb;
TextBox suPasswordTb;
TextBox* suTextboxList[3] = {&suUserNameTb, &suPasswordTb, &suEmailTb};
int suTextboxNum = 3;

PopUp* suInvalidPopup;

char suEnteredUsername[40];
char suEnteredPssword[40];
char suEnteredEmail[40];


EngineState signUpMenu = {&enterSignUpMenu, &updateSignUpMenu, &renderSignUpMenu, &exitSignUpMenu};

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
                    cJSON* json = cJSON_Parse(usersData);
                    int new = 1;
                    cJSON* user;
                    char* buffer;
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
                   
                    if(new){
                        user = cJSON_CreateObject();
                        cJSON_AddStringToObject(user, "username", suEnteredUsername);
                        cJSON_AddStringToObject(user, "password", suEnteredPssword);
                        cJSON_AddStringToObject(user, "email", suEnteredEmail);

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
    createWidget(&suFormWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, C_BG_GRAY0);
    suFormWidget.layoutType = ABSOLUTE;

    createButton(&suBackButton, &suFormWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 6);
    createButton(&suSignupButton, &suFormWidget, "Sign Up", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 11);
    createButton(&suRandomPasswordBtn, &suFormWidget, "Random", ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 16, 11);

    
    createTextBox(&suUserNameTb, &suFormWidget, "Username", suEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 6, 90);
    createTextBox(&suPasswordTb, &suFormWidget, "Password", suEnteredPssword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 11, 90);
    createTextBox(&suEmailTb, &suFormWidget, "Email", suEnteredEmail, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 18, 90);

    suBackButton.callBack = maineMenu.enter;
    suSignupButton.callBack = &signUp;
    suRandomPasswordBtn.callBack = &randomPassword;
}

void enterSignUpMenu(){
    FOR(i, suTextboxNum){
        resetTextbox(suTextboxList[i]);
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
                            FOR(i, suButtonNum){
                            buttonMouseMoveCallback(suButtonList[i]);
                            }
                            FOR(i, suTextboxNum){
                                textBoxMouseMoveCallback(suTextboxList[i]);
                            }
                        }
                        
                        
                        break;
                    default:
                        if(suInvalidPopup != NULL){
                            buttonMouseClickEvent(suInvalidPopup->close);
                        }else{
                            FOR(i, suButtonNum){
                                buttonMouseClickEvent(suButtonList[i]);
                            }
                            FOR(i, suTextboxNum){
                                textBoxMouseClickCallback(suTextboxList[i]);
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
                FOR(i, suTextboxNum){
                    textBoxKeyPressCallback(suTextboxList[i], ch);
                }
            }
            break;
            
    }
}
void renderSignUpMenu(){
    //erase();
    renderWidget(&suFormWidget);
    FOR(i, suButtonNum){
        renderButton(suButtonList[i]);
    }
    FOR(i, suTextboxNum){
        renderTextBox(suTextboxList[i]);
    }
    if(suInvalidPopup != NULL){
        renderPopup(suInvalidPopup);
    }
    refresh();
    
}
void exitSignUpMenu(){

}