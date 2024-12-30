#ifndef PLAYERACCOUNT
#define PLAYERACCOUNT
#include <stdio.h>

typedef struct{
    char* username;
    char* usernameHash;
    char* email;

    unsigned long long firstPlayTime;
    int gamesFinished;
    int goldsCollected;
    int goldRecord;
}PlayerAccount;


#endif