#ifndef PLAYERACCOUNT
#define PLAYERACCOUNT
#include <stdio.h>
#include "Utilities/cJSON.h"

typedef struct{
    char* username;
    char* password;
    char* email;

    long long firstPlayTime;
    int gamesFinished;
    int goldsCollected;
    int totalScore;
    int goldRecord;
    int gamesPlayed;
    int deaths;
}Account;

void loadAccount(cJSON* data, Account* a);
void saveAccount();
void addAccount(const char * const username, const char * const password, const char * const email);

#endif