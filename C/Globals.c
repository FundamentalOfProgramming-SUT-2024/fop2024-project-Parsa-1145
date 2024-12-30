#include "Globals.h"
#include <string.h>
#include<stdlib.h>
#include <time.h>

#include "Utilities/LinkedList.h"
 
EngineState* engineState;
GameSettings gameSettings;
int scrW;
int scrH;
int mousex;
int mousey;
int terminate = 0;
int colorPairNum = 5;

MEVENT mEvent;

char* playerDbAddress = "../Data/Players.txt";

float max(float a, float b){
    if(a > b) return a;
    else return b;
}
float min(float a, float b){
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

int randBetween(int min, int max, int seed){
    srand(time(NULL) + seed);

    return ((rand()) % (max - min)) + min;
}

int randIndexWithProb(int n, float* prob,int seed){
    srand(time(NULL) + seed);

    float r = ((float)(rand()) / RAND_MAX);

    float tmp = 0;
    for(int i = 0 ; i < n; i++){
        if((r > tmp) && (r < tmp + prob[i])){
            return i;
        }else{
            tmp += prob[i];
        }
    }
}
void wrapText(char* txt, int w){
    int n = 0;
    int j = 0;
    
    int size = strlen(txt);
    
    FOR(i, size){
        if(n == w){
            txt[j] = '\n';
            n = i - j;
        }else{
            if(txt[i] == ' '){
                j = i;
                n++;
            }else{
                n++;
            }
        }
    }
}
