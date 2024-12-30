#include <ncurses.h>
#include <stdlib.h>
#include <math.h>

#include "../ProgramStates/MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"

#include "Renderer.h"

void renderLine(char c, float x1, float y1, float x2, float y2, Camera* cam, CharTexture* frameBuffer, ColorTexture* colorBuffer){
    if(cam == NULL) cam = &mainCamera;

    x1 = x1 - cam->x;
    x2 = x2 - cam->x;
    y1 = y1 - cam->y;
    y2 = y2 - cam->y;

    if(abs(x1 - x2) < 1){
        if(!((x1 >= 0) && (x1 < cam->w))) return;

        if(y1 > y2){
            float tmp = y1;
            y1 = y2;
            y2 = tmp;
        }

        y1 = max(0, y1);
        y2 = min(cam->h - 1, y2);

        for(y1; y1 <= y2;y1++){
            mvaddch(y1, x1, c);
        }
    }else if(abs(y1 - y2) < 1){
        if(!((y1 >= 0) && (y1 < + cam->h))) return;

        if(x1 > x2){
            float tmp = x1;
            x1 = x2;
            x2 = tmp;
        }

        x1 = max(0, x1);
        x2 = min(cam->w - 1, x2);

        for(x1; x1 <= x2; x1++){
            mvaddch(y1, x1, c);
        }
    }else{
        float m = (y2 - y1) / (x2 - x1);
        float b = (y1 - x1 * m);

        x1 = max(x1, 0);
        x1 = min(x1, cam->w - 1);
        x2 = max(x2, 0);
        x2 = min(x2, cam->w-1);



        y1 = x1 * m + b;
        y2 = x2 * m + b;

        //mvprintw(2, 0, "%f %f %f %f %f %f", x1, y1, x2, y2, m, b);



        if((((y1 >= 0) && (y1 < cam->h))) || (((y2 >= 0) && (y2 <cam->h)))){
            y1 = max(y1, 0);
            y1 = min(y1, cam->h - 1);
            y2 = max(y2, 0);
            y2 = min(y2, cam->h - 1);

            x1 = (y1 - b) / m;
            x2 = (y2 - b) / m;
            
            
            if(abs(m) < 1){
                if(x1 > x2){
                    float tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                    tmp = y1;
                    y1 = y2;
                    y2 = tmp;
                } 
                x1 = round(x1);
                for(x1; x1 <= x2; x1++){
                    mvaddch(y1, x1, c);
                    y1+=m;
                }
            }else{
                if(y1 > y2){
                    float tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                    tmp = y1;
                    y1 = y2;
                    y2 = tmp;
                }
                y1 = round(y1);
                m = 1/m;
                for(y1; y1 <= y2; y1++){
                    mvaddch(y1,x1, c);
                    x1+=m;
                }
            }
            
        }
    }
    
}

void renderTexture(CharTexture* tex, ColorTexture* color, float x, float y, Camera* cam, CharTexture* frameBuffer, ColorTexture* colorBuffer){
    if(cam == NULL) cam = &mainCamera;


    float x1, y1, x2, y2;

    x -= cam->x;
    y -= cam->y;

    x1 = max(0, x);
    if(x > cam->w - 1) return;

    if(x + tex->w - 1 < 0) return;
    x2 = min(x + tex->w - 1, cam->w - 1);

    if(y > cam->h - 1) return;
    y1 = max(y, 0);

    if(y + tex->h - 1 < 0) return;
    y2 = min(y + tex->h - 1, cam->h - 1);

    
    for(y1; y1 <= y2; y1++){
        for(int i = x1; i <= x2; i++){
            if(tex->data[(int)(y1 - y)][(int)(i - x)] != '\0'){
                frameBuffer->data[(int)y1][i] = (tex->data[(int)(y1 - y)][(int)(i - x)]);
            }
        }
    }
}

void renderFrameBuffer(CharTexture* tex){
    FOR(i, tex->h){
        move(i, 0);
        FOR(j, tex->w){
            if(tex->data[i][j]){
                addch(tex->data[i][j]);
            }else{
                addch(' ');
            }
            
        }
    }
}
void maskFrameBuffer(CharTexture* frb, ColorTexture* clb, CharTexture* mask){
    if(mask->h > frb->h || mask->w > frb->w) return;

    FOR(i, mask->h){
        FOR(j, mask->w){
            if(!(mask->data[i][j])) frb->data[i][j] = 0;
        }
    }
}