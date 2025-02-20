#include "RollDialouge.h"

extern EngineState loadNewGame;
#include "../GameObjects/TextureManager.h"
#include "../GameObjects/Renderer.h"
#include "../Globals.h"

EngineState rollDialouge = {NULL, &updateRollDialouge, &exitRollDialouge, NULL, NULL};



CharTexture* dialougeTexture = NULL;

uint32_t enterTime, rdCurTime, startDelay, iterTime, dAmount, rdTotalTime;
uint32_t rdPrevTime[3];
uint8_t stepSize;

long long total;
int passed;
int reverse;
int start = 0;


void (*exitFunc)();

float simpleEasingFunction(float t){
    return (t*t)*(t-1)*(t-1) * 16;
}

void simpleFadeUpdate(int ch){
    if(ch=='e'){
        exitFunc();
    }else{
        rdCurTime = SDL_GetTicks();
        if(start){
            float color = simpleEasingFunction(((float)(rdCurTime - enterTime)) / rdTotalTime) * 234;
            fillColorTexture(dialougeTexture , color - (((int)color)%stepSize) + 20);
            if(rdCurTime - enterTime >= rdTotalTime){
                SDL_Delay(1000);
                exitFunc();
            }
            renderRollDialouge();
        }else if(rdCurTime - enterTime >= startDelay){
            start = 1;
            enterTime = SDL_GetTicks();
        }
    }
}

void wordFadeUpdate(int ch){
    if(ch == 'e'){
        exitFunc();
    }else{
    rdCurTime = SDL_GetTicks();
        if(start){
            if(rdCurTime - rdPrevTime[1] >= 10 + dAmount){
                if(passed != -1){
                    int x = 0, y = 0;
                    do{
                        passed++;
                        x = passed % dialougeTexture->w;
                        y = passed / dialougeTexture->w;
                    }while((passed <= total - 2) && (dialougeTexture->data[y][x] == ' '));
                    if(dAmount)dAmount = 0;
                    if(!reverse){
                        if(dialougeTexture->data[y][x] == '.'){
                            dAmount = 600;
                        }else if(dialougeTexture->data[y][x] == ','){
                            dAmount = 300;
                        }
                    }
                    rdPrevTime[1] = rdCurTime;
                }
            }
            if(rdCurTime - rdPrevTime[0] >= 50){
                int y = 0, x = 0;
                FOR(i, passed){
                    x++;
                    if(x >= dialougeTexture->w){
                        y++;
                        x = 0;
                    }
                    if(!reverse){
                        if(dialougeTexture->color[y][x] <= 254){
                            dialougeTexture->color[y][x] += randBetween(0, 20, 0);
                            if(dialougeTexture->color[y][x] >= 254){
                                dialougeTexture->color[y][x] = 254;
                                if(passed >= total - 4){
                                    passed = 0;
                                    reverse = 1;
                                }
                            }
                        }
                    }else{
                        if(dialougeTexture->color[y][x] >= 21){
                            dialougeTexture->color[y][x] -= randBetween(0, 20, 0);
                            if(dialougeTexture->color[y][x] <= 20){
                                dialougeTexture->color[y][x] = 20;
                                if(passed >= total - 4){
                                    exitFunc();
                                    return;
                                }
                            }
                        }
                    }
                }
                rdPrevTime[0] = rdCurTime;
                renderRollDialouge();
            }
        }else if(rdCurTime - enterTime >= startDelay){
            start = 1;
        }
    }
}

void staryFadeUpdate(int ch){
    if(ch == 'e'){
        exitFunc();
    }else{
        rdCurTime = SDL_GetTicks();
        if(start){
            if(rdCurTime - rdPrevTime[0] >= 50){
                int finished = 1;
                if(!reverse){
                    FOR(i, dialougeTexture->h){
                        FOR(j, dialougeTexture->w){
                            dialougeTexture->color[i][j] += randBetween(0, stepSize, 0);

                            if(dialougeTexture->color[i][j] >= 254){
                                dialougeTexture->color[i][j] = 254;
                            }
                            if(dialougeTexture->color[i][j] <= 200){
                                finished = 0;
                            }
                        }
                    }
                    if(finished){
                        reverse = 1;
                    }
                }else{
                    FOR(i, dialougeTexture->h){
                        FOR(j, dialougeTexture->w){
                            dialougeTexture->color[i][j] -= randBetween(0, stepSize, 0);

                            if(dialougeTexture->color[i][j] <= 20){
                                dialougeTexture->color[i][j] = 20;
                            }
                            if(dialougeTexture->color[i][j] >= 21){
                                finished = 0;
                            }
                        }
                    }
                    if(finished){
                        exitFunc();
                    }
                }
                rdPrevTime[0] = rdCurTime;
                renderRollDialouge();
            }
        }else if(rdCurTime - enterTime >= startDelay){
            start = 1;
            rdPrevTime[0] = rdCurTime;
        }
    }
}

void (*rollDialougeUpdates[3])(int) = {&simpleFadeUpdate, &wordFadeUpdate, &staryFadeUpdate};

void startRollingDialouge(const char * const txt, enum dialougeRollType type, uint32_t ms, uint8_t stpSize, uint32_t startOffset, int cr, int cg, int cb, void (*extFunc)()){
    if(dialougeTexture) deleteCharTexture(dialougeTexture);


    dialougeTexture = copyTexture(getTextureByName(txt)->data);

    exitFunc = extFunc;

    int k = 20;
    int totalColors = 255 - k;

    FOR(i, totalColors){
        init_color(k, (cr) * (i / 254.1), (cg) * (i / 254.1), (cb) * (i / 254.1));
        init_pair(k, k, 20);
        k++;
    }

    start = 0;
    enterTime = SDL_GetTicks();
    startDelay = startOffset;
    rdTotalTime = ms;
    iterTime = (ms / 254.9) * stpSize;
    stepSize = stpSize;
    reverse = 0;
    dAmount = 0;
    total = dialougeTexture->w * dialougeTexture->h;
    passed = 0;
    rdPrevTime[0] = enterTime;
    rdPrevTime[1] = enterTime;

    fillColorTexture(dialougeTexture, 20);


    rollDialouge.update = rollDialougeUpdates[type];

    engineState = &rollDialouge;

    renderRollDialouge();
    nodelay(stdscr, 1);
}

void updateRollDialouge(int ch){

}

void renderRollDialouge(){
    erase();
    emptyFrameBuffer(globalFrameBuffer);
    fillCharTexture(globalFrameBuffer, ' ');
    fillColorTexture(globalFrameBuffer, 21);

    renderDepthlessTexture(dialougeTexture, -dialougeTexture->w / 2 + (dialougeTexture->w & 1), -dialougeTexture->h / 2 + (dialougeTexture->h & 1), 1, &globalCamera, globalFrameBuffer);

    renderFrameBuffer(globalFrameBuffer);

    refresh();
}
void exitRollDialouge(){

}