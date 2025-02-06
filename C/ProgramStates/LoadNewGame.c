#include <SDL2/SDL.h>

#include "LoadNewGame.h"
#include "MainGame.h"
#include "../GameObjects/Texture.h"
#include "../GameObjects/Renderer.h"
#include "../Globals.h"

EngineState loadNewGame;



CharTexture* dialougeTexture;

void fadeIn(int* color, int ms){
    
}

EngineState loadNewGame = {NULL, &updateNewGameLoading, &renderNewGameLoading, NULL, NULL};

uint32_t enterTime, prevTime2, curTime2, dAmount;
long long total;
int passed;
int op;
int reverse;
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
    nodelay(stdscr, 1);
}
void updateNewGameLoading(int ch){
    if(ch == 'e'){
        enterMainGame();
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
                            enterMainGame();
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
    
}
void exitNewGameLoading(){

}