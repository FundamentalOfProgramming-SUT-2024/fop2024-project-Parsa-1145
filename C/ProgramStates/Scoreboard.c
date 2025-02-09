#include <stdlib.h>
#include <string.h>


#include "Scoreboard.h"
#include "MainMenu.h"
#include "../UiElements/TextWidget.h"
#include "../UiElements/Button.h"
#include "../Globals.h"

LinkedList sbUiList;

EngineState scoreboard = {&enterScoreBoard, &updateScoreBoard, &renderScoreBoard, &exitScoreBoard, &sbUiList};

Widget sbMainWidget;
Widget sbScrollArea;

Button sbBackBtn;


void** sbTmpIterPtr;
UiBase* sbIterPtr;
void initScoreBoard(){
    createLinkedList(&sbUiList, sizeof(UiBase*));

    createWidget(&sbMainWidget, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 90, 90, NULL);
    createWidget(&sbScrollArea, &sbMainWidget, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_TOP, 1, 5, 100, 100, NULL);
    sbScrollArea.scrollOn = 1;
    sbScrollArea.layoutType = VERTICAL_LAYOUT;
    sbScrollArea.layoutPadding = 1;



    sbMainWidget.bordered = 1;
    createButton(&sbBackBtn, &sbMainWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_TOP, 1, 1, 4);
    sbBackBtn.callBack = &enterMainMenu;

    linkedListPushBack(sbMainWidget.children, sbBackBtn.uiBase);
    linkedListPushBack(sbMainWidget.children, sbScrollArea.uiBase);

    TextWidget* tmp = malloc(sizeof(TextWidget));
    char* format = writeLog("%3s. %-42s %-7s %-6s %-7s %-17s","No", "Name", "Golds", "Games", "score", "Experience");
    createTextWidget(tmp, &sbMainWidget, ALIGN_LEFT, ABSOLUTE, 1, 3, format, 5, 5, 0);
    tmp->wrap = 0;
    free(format);
    linkedListPushBack(sbMainWidget.children, tmp->uiBase);


    linkedListPushBack(&sbUiList, sbMainWidget.uiBase);
}
void enterScoreBoard(){
    engineState = &scoreboard;

    cJSON* json = openJsonFile(playerDbAddress);
    int n = cJSON_GetArraySize(json);
    Account** accounts = malloc(sizeof(Account*) * n);

    {   
        cJSON* tmp = json->child;
        int i = 0;
        while(tmp){
            accounts[i] = calloc(1, sizeof(Account));
            loadAccount(tmp, accounts[i]);
            i++;
            tmp = tmp->next;
        }
    }

    emptyWidget(&sbScrollArea);

    FOR(i, n){
        for(int j = i+1; j < n; j++){
            if(accounts[i]->totalScore < accounts[j]->totalScore){
                Account* tmp = accounts[i];
                accounts[i] = accounts[j];
                accounts[j] = tmp;
                
            }
        }
    }
    FOR(i, n){
        TextWidget* tmp = malloc(sizeof(TextWidget));
        char* format;

        long long duration = time(NULL) - accounts[i]->firstPlayTime;

        int minutes = duration / 60;
        int hours = minutes / 60;
        int days = hours / 24;
        int months = days / 30;
        int years = months / 12;

        char* timeSinceFirstGame;
        if(accounts[i]->gamesPlayed){
            if(years){
                timeSinceFirstGame = writeLog("%-2d %-3s %-2d %-3s", years, years > 1 ? "Yrs":"Yr", months%12, months%12>1 ? "Mos":"Mo");
            }else if(months){
                timeSinceFirstGame = writeLog("%-2d %-3s %-2d %-3s", months, months>1 ? "Mos":"Mo", days%30, days%30 > 1?"Dys":"Dy");
            }else if(days){
                timeSinceFirstGame = writeLog("%-2d %-3s %-2d %-3s", days, days > 1?"Dys":"Dy", hours%24, hours%24>1?"Hrs":"Hr");
            }else if(hours){
                timeSinceFirstGame = writeLog("%-2d %-3s", hours, hours > 1?"Hrs":"Hr");
            }else{
                timeSinceFirstGame = writeLog("%-2d %-3s %-2d %-3s", minutes, minutes > 1?"Mns":"Mn", duration%24, duration%24>1?"Scs":"Sc");
            }
        }
        else{
            timeSinceFirstGame = writeLog("-");
        }
        if(i == 0){
            char* longName = writeLog("%s The No-Cap King", accounts[i]->username);
            format = writeLog("%3d. %%o%%U %-40s%%O %-7d %-6d %-7d %-17s", i+1, longName, accounts[i]->goldsCollected,
            accounts[i]->gamesPlayed, accounts[i]->totalScore, timeSinceFirstGame);

            createTextWidget(tmp, &sbScrollArea, ALIGN_LEFT, WITH_PARENT, 0, 0, format, 5, 5, 0, 10051);
            free(longName);
        }else if(i == 1){
            char* longName = writeLog("The Legandary %s", accounts[i]->username);
            format = writeLog("%3d. %%o%%U %-40s%%O %-7d %-6d %-7d %-17s", i+1, longName, accounts[i]->goldsCollected,
            accounts[i]->gamesPlayed, accounts[i]->totalScore, timeSinceFirstGame);

            createTextWidget(tmp, &sbScrollArea, ALIGN_LEFT, WITH_PARENT, 0, 0, format, 5, 5, 4, 6821);
            free(longName);
        }else if(i == 2){
            char* longName = writeLog("The Almost Legandary %s", accounts[i]->username);
            format = writeLog("%3d. %%o%%U %-40s%%O %-7d %-6d %-7d %-17s", i+1, longName, accounts[i]->goldsCollected,
            accounts[i]->gamesPlayed, accounts[i]->totalScore, timeSinceFirstGame);

            createTextWidget(tmp, &sbScrollArea, ALIGN_LEFT, WITH_PARENT, 0, 0, format, 5, 3, 0, 6819);
            free(longName);
        }else{
            format = writeLog("%3d. %-42s %-7d %-6d %-7d %-17s", i+1, accounts[i]->username, accounts[i]->goldsCollected,
            accounts[i]->gamesPlayed, accounts[i]->totalScore, timeSinceFirstGame);

            createTextWidget(tmp, &sbScrollArea, ALIGN_LEFT, WITH_PARENT, 0, 0, format);
        }
        free(format);
        free(timeSinceFirstGame);
        linkedListPushBack(sbScrollArea.children, tmp->uiBase);

        if(account.username && !strcmp(account.username, accounts[i]->username)){
            tmp = malloc(sizeof(TextWidget));

            createTextWidget(tmp, &sbScrollArea, ALIGN_LEFT, WITH_PARENT, 0, -1, "     %U", 5579);
            linkedListPushBack(sbScrollArea.children, tmp->uiBase);
        }
        tmp->wrap = 0;

    }

    FOR(i, n){
        free(accounts[i]);
    }
    free(accounts);
    cJSON_free(json);
    
}
void updateScoreBoard(int ch){
}
void renderScoreBoard(){
    erase();
    emptyFrameBuffer(uiFrameBuffer);

    sbTmpIterPtr = sbUiList.data;
    FOR(i, sbUiList.size){
        sbIterPtr = sbTmpIterPtr[1];
        sbIterPtr->render(sbIterPtr->object);
        sbTmpIterPtr = sbTmpIterPtr[0];
    }
    renderFrameBuffer(uiFrameBuffer);
    refresh();
}
void exitScoreBoard(){

}