#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>
#include <pthread.h>

#include "ProgramStates/MainMenu.h"
#include "ProgramStates/NewGameMenu.h"
#include "ProgramStates/LogInMenu.h"
#include "ProgramStates/MainGame.h"
#include "ProgramStates/SignupMenu.h"
#include "ProgramStates/Scoreboard.h"
#include "SecondaryThread.h"
#include "UiElements/PopUp.h"
#include "GlobalDefines.h"
#include "Globals.h"


int main(){
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_color(10, 150, 150, 150);
    init_color(12, 100, 100, 100);
    init_pair(C_BG_GRAY0, COLOR_WHITE, 10);
    init_pair(C_BG_GRAY1, COLOR_WHITE, 12);
    init_pair(C_BG_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_BG_WHITE, COLOR_BLACK, COLOR_WHITE);


    int k = 20;
    float z = 1000 / 6;
    FOR(i, 6){
        FOR(j, 6){
            FOR(t, 6){
                init_color(k, z * i, z * j, z * t);
                init_pair(k, k, 0);
                rgb[i][j][t] = k;
                k++;
            }
        }
    }
    FOR(i, 6){
        FOR(j, 6){
            FOR(t, 6){
                FOR(x, 6){
                    FOR(y, 6){
                        FOR(l, 6){
                            if((x || y) || l){
                                init_pair(k, rgb[i][j][t], rgb[x][y][l]);
                                bgRgb[rgb[i][j][t]][rgb[x][y][l]] = k;
                                k++;
                            }
                        }
                    }
                }
            }
        }
    }
    getmaxyx(stdscr, scrH, scrW);

    initMainMenu();
    initNewGameMenu();
    initLogInMenu();
    initMainGame();
    initSignUpMenu();
    initScoreBoard();

    initSecondaryThread();
    
    char* tmp;
    fileToStr("../Data/Items.json", &tmp);
    itemsJson = cJSON_Parse(tmp);


    engineState = &maineMenu;

    (*(engineState->enter))();

    while(!terminate){
        (*(engineState->update))();
        (*(engineState->render))();
    }

    pthread_join(secondaryThread, NULL);

    endwin();
}