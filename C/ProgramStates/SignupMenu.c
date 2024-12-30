#include <string.h>

#include "SignupMenu.h"

#include "MainMenu.h"
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
        int passSize = 0;
        int hasCap = 0;
        int hasLow = 0;
        int hasNum = 0;
        passSize = strlen(suEnteredPssword);
        FOR(i, passSize){
            if((suEnteredPssword[i] >= 'A') &&  (suEnteredPssword[i] <= 'Z')) hasCap = 1;
            else if((suEnteredPssword[i] >= 'a') &&  (suEnteredPssword[i] <= 'z')) hasLow = 1;
            else if((suEnteredPssword[i] >= '0') &&  (suEnteredPssword[i] <= '9')) hasNum = 1;
        }

        if((passSize >= 7) && hasLow && hasCap && hasNum){
            passSize = strlen(suEnteredEmail);
            int valid = 1;
            int partSize = 0;
            int seenAtsign = 0;
            int seenDot = 0;

            FOR(i, passSize){
                if(suEnteredEmail[i] == '@'){
                    if((!partSize) || (seenAtsign)){
                        valid = 0;
                        break;
                    }
                    seenAtsign = 1;
                    partSize = 0;
                }else if(suEnteredEmail[i] == '.'){
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
            if((!seenAtsign) || (!seenDot) || (!partSize)){
                valid = 0;
            }

            if(valid){
                FILE* playerdb = fopen(playerDbAddress, "r");
                if(playerdb != NULL){
                    int playernum = 0;
                    fscanf(playerdb, "%d", &playernum);
                    
                    int exists = 0;
                    char tmp[100];

                    while(!feof(playerdb)){
                        fscanf(playerdb, "%s", tmp);

                        if(!strcmp(tmp, "#")){
                            fscanf(playerdb, "%s", tmp);
                            if(!strcmp(tmp, suEnteredUsername)){
                                suInvalidPopup = createPopUp("Username exists.", NULL, 20, 20, &closeInvalidPopup);
                                fclose(playerdb);

                                return;
                            }
                        }
                        mvprintw(0, 0, "%s", tmp);
                    }
                    fclose(playerdb);

                    FILE* playerdb = fopen(playerDbAddress, "a");

                    fprintf(playerdb, "# %s\n", suEnteredUsername);
                    fclose(playerdb);
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
void initSignUpMenu(){
    createWidget(&suFormWidget, NULL, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 30, 30, C_BG_GRAY0);
    suFormWidget.layoutType = ABSOLUTE;

    createButton(&suBackButton, &suFormWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 2, 2, 6);
    createButton(&suSignupButton, &suFormWidget, "Sign Up", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 11);
    createButton(&suRandomPasswordBtn, &suFormWidget, "Random", ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 17, 11);

    
    createTextBox(&suUserNameTb, &suFormWidget, "Username", suEnteredUsername, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 6, 90);
    createTextBox(&suPasswordTb, &suFormWidget, "Password", suEnteredPssword, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 11, 90);
    createTextBox(&suEmailTb, &suFormWidget, "Email", suEnteredEmail, RELATIVE, ABSOLUTE, ALIGN_CENTER, ABSOLUTE, 0, 19, 90);

    suBackButton.callBack = maineMenu.enter;
    suSignupButton.callBack = &signUp;
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