#include <string.h>

#include "Account.h"
#include "Utilities/cJSON.h"
#include "Globals.h"

void loadAccount(cJSON* data, Account* a){
    a->username = copyString(cJSON_GetObjectItem(data, "username")->valuestring);
    a->password = copyString(cJSON_GetObjectItem(data, "password")->valuestring);
    a->email = copyString(cJSON_GetObjectItem(data, "email")->valuestring);
    a->firstPlayTime = cJSON_GetObjectItem(data, "firstPlayTime")->valueint;
    a->gamesFinished = cJSON_GetObjectItem(data, "finishedGames")->valueint;
    a->gamesPlayed = cJSON_GetObjectItem(data, "playedGames")->valueint;
    a->goldsCollected = cJSON_GetObjectItem(data, "totalGoldCollected")->valueint;
    a->goldRecord = cJSON_GetObjectItem(data, "goldRecord")->valueint;
    a->deaths = cJSON_GetObjectItem(data, "deaths")->valueint;
}

void saveAccount(){
    cJSON* accounts = openJsonFile(playerDbAddress);

    cJSON* tmp = accounts->child;
    cJSON* accountJ;
    while(tmp){
        if(!strcmp(account.username, cJSON_GetObjectItem(tmp, "username")->valuestring)){
            accountJ = tmp;
        }
        tmp = tmp->next;
    }
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "firstPlayTime"), account.firstPlayTime);
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "playedGames"), account.gamesPlayed);
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "finishedGames"), account.gamesFinished);
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "totalGoldCollected"), account.goldsCollected);
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "goldRecord"), account.goldRecord);
    cJSON_SetNumberValue(cJSON_GetObjectItem(accountJ, "deaths"), account.deaths);
    

    saveJsonToFile(playerDbAddress, accounts);
    cJSON_Delete(accounts);
}

void addAccount(const char * const username, const char * const password, const char * const email){
    cJSON* accounts = openJsonFile(playerDbAddress);

    cJSON* accountJ = cJSON_CreateObject();
    cJSON_AddStringToObject(accountJ, "username", username);
    cJSON_AddStringToObject(accountJ, "password", password);
    cJSON_AddStringToObject(accountJ, "email", email);
    cJSON_AddNumberToObject(accountJ, "firstPlayTime", 0);
    cJSON_AddNumberToObject(accountJ, "finishedGames", 0);
    cJSON_AddNumberToObject(accountJ, "playedGames", 0);
    cJSON_AddNumberToObject(accountJ, "totalGoldCollected", 0);
    cJSON_AddNumberToObject(accountJ, "goldRecord", 0);
    cJSON_AddNumberToObject(accountJ, "deaths", 0);

    cJSON_AddItemToArray(accounts, accountJ);

    saveJsonToFile(playerDbAddress, accounts);
    
    cJSON_Delete(accounts);
}