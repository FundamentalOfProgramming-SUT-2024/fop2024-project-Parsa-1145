#include <ncurses.h>
#include <stdlib.h>
#include <math.h>

#include "../ProgramStates/MainGame.h"
#include "../Globals.h"
#include "../GlobalDefines.h"

#include "Renderer.h"

void renderLine(wchar_t c, int color, int depth, double x1, double y1, double x2, double y2, Camera* cam, CharTexture* frameBuffer){
    if(cam == NULL) cam = &mainCamera;
    static char drawTypes[2][2] = {{0, 1}, {2, 3}};
    char drawType = drawTypes[c != 0][color != -1];

    x1 = x1 - cam->x + 0.5;
    x2 = x2 - cam->x + 0.5;
    y1 = y1 - cam->y + 0.5;
    y2 = y2 - cam->y + 0.5;

    double m, b;
    int dir;

    if(abs(x1 - x2) < 1){
        if(!((x1 >= 0) && (x1 < cam->w))) return;

        if(y1 > y2){
            double tmp = y1;
            y1 = y2;
            y2 = tmp;
        }

        y1 = max(0, y1);
        y2 = min(cam->h - 1, y2);

        dir = 1;
        m = 0;
    }else if(abs(y1 - y2) < 1){
        if(!((y1 >= 0) && (y1 < + cam->h))) return;

        if(x1 > x2){
            double tmp = x1;
            x1 = x2;
            x2 = tmp;
        }

        x1 = max(0, x1);
        x2 = min(cam->w - 1, x2);

        dir = 0;
        m = 0;
    }else{
        m = (y2 - y1) / (x2 - x1);
        b = (y1 - x1 * m);

        x1 = max(x1, 0);
        x1 = min(x1, cam->w - 1);
        x2 = max(x2, 0);
        x2 = min(x2, cam->w-1);

        y1 = x1 * m + b;
        y2 = x2 * m + b;

        if((((y1 >= 0) && (y1 < cam->h))) || (((y2 >= 0) && (y2 <cam->h)))){
            y1 = max(y1, 0);
            y1 = min(y1, cam->h - 1);
            y2 = max(y2, 0);
            y2 = min(y2, cam->h - 1);

            x1 = (y1 - b) / m;
            x2 = (y2 - b) / m;
            
            if(abs(m) < 1){
                if(x1 > x2){
                    double tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                    tmp = y1;
                    y1 = y2;
                    y2 = tmp;
                }
            }else{
                if(y1 > y2){
                    double tmp = x1;
                    x1 = x2;
                    x2 = tmp;
                    tmp = y1;
                    y1 = y2;
                    y2 = tmp;
                }
                m = 1/m;
            }
        }else{
            return;
        }
    }

    if(frameBuffer->hasDepth){
        switch(dir){
            case 0:
                for(x1; x1 <= x2 + 0.5; x1++){
                    if(frameBuffer->depth[(int)(y1)][(int)x1] < depth){
                        switch (drawType){
                            case 1:
                                frameBuffer->color[(int)(y1)][(int)x1] = color;
                                break;
                            case 2:
                                frameBuffer->data[(int)(y1)][(int)x1] = c;
                                break;
                            case 3:
                                frameBuffer->color[(int)(y1)][(int)x1] = color;
                                frameBuffer->data[(int)(y1)][(int)x1] = c;
                                break;
                            default:
                                break;
                        }
                        frameBuffer->depth[(int)(y1)][(int)x1] = depth;
                    }
                    y1+=m;
                }
                break;
            case 1:
                for(y1; y1 <= y2 + 0.5; y1++){
                    if(frameBuffer->depth[(int)(y1)][(int)x1] < depth){
                        switch (drawType){
                            case 1:
                                frameBuffer->color[(int)(y1)][(int)x1] = color;
                                break;
                            case 2:
                                frameBuffer->data[(int)(y1)][(int)x1] = c;
                                break;
                            case 3:
                                frameBuffer->color[(int)(y1)][(int)x1] = color;
                                frameBuffer->data[(int)(y1)][(int)x1] = c;
                                break;
                            default:
                                break;
                        }
                        frameBuffer->depth[(int)(y1)][(int)x1] = depth;
                    }
                    x1+=m;
                }
                break;
        }
    }else{
        switch(dir){
            case 0:
                for(x1; x1 <= x2 + 0.5; x1++){
                    switch (drawType){
                        case 1:
                            frameBuffer->color[(int)(y1)][(int)x1] = color;
                            break;
                        case 2:
                            frameBuffer->data[(int)(y1)][(int)x1] = c;
                            break;
                        case 3:
                            frameBuffer->color[(int)(y1)][(int)x1] = color;
                            frameBuffer->data[(int)(y1)][(int)x1] = c;
                            break;
                        default:
                            break;
                    }
                    y1+=m;
                }
                break;
            case 1:
                for(y1; y1 <= y2 + 0.5; y1++){
                    switch (drawType){
                        case 1:
                            frameBuffer->color[(int)(y1)][(int)x1] = color;
                            break;
                        case 2:
                            frameBuffer->data[(int)(y1)][(int)x1] = c;
                            break;
                        case 3:
                            frameBuffer->color[(int)(y1)][(int)x1] = color;
                            frameBuffer->data[(int)(y1)][(int)x1] = c;
                            break;
                        default:
                            break;
                    }
                    x1+=m;
                }
                break;
        }
    }
    
    
}

void renderTexture(CharTexture* tex, int x, int y, Camera* cam, CharTexture* frameBuffer){
    if(cam == NULL) cam = &mainCamera;


    int x1, y1, x2, y2;

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


    int u = x1 - x;
    int v = y1 - y;

    if(frameBuffer->hasDepth){
        if(frameBuffer->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && (frameBuffer->depth[y1][i] <= tex->depth[v][u])){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                        frameBuffer->depth[y1][i] = tex->depth[v][u];
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && (frameBuffer->depth[y1][i] <= tex->depth[v][u])){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = tex->depth[v][u];
                    }
                }
            }
        }
    }else{
        if(frameBuffer->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0')){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0')){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                    }
                }
            }
        }
    }
}
void renderDepthlessTexture(CharTexture* tex, int x, int y, int depth, Camera* cam, CharTexture* frameBuffer){
    if(cam == NULL) cam = &mainCamera;


    int x1, y1, x2, y2;

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


    int u = x1 - x;
    int v = y1 - y;

    if(frameBuffer->hasDepth){
        if(frameBuffer->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && (frameBuffer->depth[y1][i] <= depth)){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                        frameBuffer->depth[y1][i] = depth;
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && (frameBuffer->depth[y1][i] <= depth)){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = depth;
                    }
                }
            }
        }
    }else{
        if(frameBuffer->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0')){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0')){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                    }
                }
            }
        }
    }
}
void renderFrameBuffer(CharTexture* tex){
    int prevColor = 0, lastColor = 0;
    FOR(i, tex->h){
        move(i, 0);
        FOR(j, tex->w){
            if(tex->data[i][j]){
                if(tex->color[i][j] != prevColor){
                    attroff(COLOR_PAIR(prevColor));
                    attron(COLOR_PAIR(tex->color[i][j]));
                    lastColor = tex->color[i][j];
                    prevColor = lastColor;
                }
                printw("%lc", tex->data[i][j]);
            }else{
                addch(' ');
            }
        }
    }
    if(lastColor != 0){
        attroff(COLOR_PAIR(lastColor));
    }
}
void maskFrameBuffer(CharTexture* frb, CharTexture* mask){
    if(mask->h > frb->h || mask->w > frb->w) return;

    FOR(i, mask->h){
        FOR(j, mask->w){
            if(!(mask->data[i][j])){
                frb->data[i][j] = '\0';
            }
        }
    }
}
void colorMaskFrameBuffer(CharTexture* frb,CharTexture* mask){
    if(mask->h > frb->h || mask->w > frb->w) return;

    FOR(i, mask->h){
        FOR(j, mask->w){
            if(!(mask->data[i][j])){
                frb->color[i][j] = rgb[1][1][1];
            }
        }
    }
}