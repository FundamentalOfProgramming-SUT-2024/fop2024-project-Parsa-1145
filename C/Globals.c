#include "Globals.h"
#include <string.h>
#include<stdlib.h>
 
 
EngineState* engineState;
int scrW;
int scrH;
int mousex;
int mousey;
int terminate = 0;
int colorPairNum = 5;
MEVENT mEvent;

char* playerDbAddress = "../Data/Players.txt";

int max(int a, int b){
    if(a > b) return a;
    else return b;
}
int min(int a, int b){
    if(a < b) return a;
    else return b;
}
char* hashWithKey(char* str, char* key){
    int strLength, keyLen;

    strLength = strlen(str);
    keyLen = strlen(key);

    char* dest = (char*) malloc(strLength);

    FOR(i, strLength){
        dest[i] = str[i] ^ key[i % keyLen];
    }

    return dest;
}
int freadBinStr(FILE* file, char* dest){
    int i = 0;
    if(dest != NULL){
        while(!feof(file)){
            fread(dest + i, 1, 1, file);
            if(dest[i] <= 32){
                dest[i] = '\0';
                break;
            }
            i++;
        }
    }else{
        while(1){
            char c;
            fread(&c, 1, 1, file);
            if(c <= 32){
                break;
            }
        }
    }

}