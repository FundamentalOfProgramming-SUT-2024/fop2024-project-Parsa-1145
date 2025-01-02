#ifndef PLAYERACCOUNT
#define PLAYERACCOUNT
#include <stdio.h>

typedef struct{
    char* username;
    char* password;
    char* email;

    long long firstPlayTime;
    int gamesFinished;
    int goldsCollected;
    int goldRecord;
}Account;


#endif