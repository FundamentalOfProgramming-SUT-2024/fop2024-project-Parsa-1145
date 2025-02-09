#include "RollDialouge.h"

extern EngineState loadNewGame;

#include <SDL2/SDL.h>

#include "LoadNewGame.h"
#include "MainGame.h"
#include "../GameObjects/Texture.h"
#include "../GameObjects/Renderer.h"
#include "../Globals.h"

EngineState rollDialouge = {NULL, &updateRollDialouge, &renderRollDialouge, NULL, NULL};



CharTexture* dialougeTexture = NULL;

uint32_t enterTime, prevTime2, curTime2, dAmount;
long long total;
int passed;
int op;
int reverse;

enum dialougeRollType{
    simpleFade,
    wordFade
};

void simpleFade(int ch){

}

void wordFade(int ch){

}

void (*rollDialougeUpdates[2])() = {&simpleFade, &wordFade};

void startRollingDialouge(const char * const txt, enum dialougeRollType type, uint32_t ms, uint32_t startOffset, int cr, int cg, int cb){
    if(dialougeTexture) deleteCharTexture(dialougeTexture);

    dialougeTexture = getTextureByName(txt);

    int k = 20;
    int totalColors = 255 - k;
    int br, bg, bb, color, background;
    {
        pair_content(0, &color, &background);
        color_content(background, &br, &bg, &bb);

        init_color(k, br, bg, bb);
    }

    FOR(i, totalColors){
        init_pair(k, k, 20);
        k++;
        init_color(k, (cr-br) * (i / 254.1), (cg-bg) * (i / 254.1), (cb-bb) * (i / 254.1));
    }

    rollDialouge.update = rollDialougeUpdates[type];

    engineState = &rollDialouge;

    nodelay(stdscr, 1);
}

void enterNewGameLoading(){
    dialougeTexture = loadCharTextureFromTxt("../Data/Ascii/Dialouges/openningText.txt");

    int k = 20;
    int tot = 256 - k - 1;
    init_color(k, 0, 0, 0);
    init_pair(k, 20, 20);
    k++;
    FOR(i, tot){
        init_color(k, i * 1000 / tot, i * 800 / tot, i * 800 / tot);
        init_pair(k, k, 20);
        k++;
    }

    enterTime = SDL_GetTicks();
    prevTime2 = enterTime;
    op = 20;

    engineState = &loadNewGame;
    passed = 0;
    reverse = 0;
    total = dialougeTexture->w * dialougeTexture->h;
    fillColorTexture(dialougeTexture, 20);

    renderNewGameLoading();
    SDL_Delay(2000);
    nodelay(stdscr, 1);
}
void updateNewGameLoading(int ch){
    if(ch == 'e'){
        startNewGame();
    }
    curTime2 = SDL_GetTicks();
    if(curTime2 - prevTime2 >= 10 + dAmount){
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
            prevTime2 = curTime2;
        }
    }
    if(curTime2 - enterTime >= 50){
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
                            startNewGame();
                            return;
                        }
                    }
                }
            }
        }
        enterTime = curTime2;
        
        erase();
        emptyFrameBuffer(globalFrameBuffer);
        fillCharTexture(globalFrameBuffer, ' ');
        fillColorTexture(globalFrameBuffer, 20);

        renderDepthlessTexture(dialougeTexture, -dialougeTexture->w / 2, -dialougeTexture->h / 2, 1, &globalCamera, globalFrameBuffer);

        renderFrameBuffer(globalFrameBuffer);

        refresh();
    }
}
void renderNewGameLoading(){
    erase();
    emptyFrameBuffer(globalFrameBuffer);
    fillCharTexture(globalFrameBuffer, ' ');
    fillColorTexture(globalFrameBuffer, 20);

    renderDepthlessTexture(dialougeTexture, -dialougeTexture->w / 2, -dialougeTexture->h / 2, 1, &globalCamera, globalFrameBuffer);

    renderFrameBuffer(globalFrameBuffer);

    refresh();
}
void exitNewGameLoading(){

}