#include <stdlib.h>
#include <math.h>

#include "../Globals.h"
#include "../GlobalDefines.h"
#include "../UiElements/Widget.h"


struct{
    int x1, y1, x2, y2;
    int cut;
    int wrap;
}RenderBound;


void renderLineToFrameBuffer(wchar_t c, int color, int depth, double x1, double y1, double x2, double y2, Camera* cam, CharTexture* frameBuffer){
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
                dir = 0;
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
                dir = 1;
            }
        }else{
            return;
        }
    }

    int x, y;


    if(frameBuffer->hasDepth){
        if(frameBuffer->hasColor && (color != -1)){
            if(c){
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->depth[y][x] <= depth){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }else if(!frameBuffer->data[y][x]){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }
                            if(frameBuffer->colorDepth[y][x] <= depth){
                                frameBuffer->color[y][x] = color;
                                frameBuffer->colorDepth[y][x] = depth;
                            }
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->depth[y][x] <= depth){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }else if(!frameBuffer->data[y][x]){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }
                            if(frameBuffer->colorDepth[y][x] <= depth){
                                frameBuffer->color[y][x] = color;
                                frameBuffer->colorDepth[y][x] = depth;
                            }
                            x1+=m;
                        }
                        break;
                }
            }else{
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->colorDepth[y][x] <= depth){
                                frameBuffer->color[y][x] = color;
                                frameBuffer->colorDepth[y][x] = depth;
                            }
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->colorDepth[y][x] <= depth){
                                frameBuffer->color[y][x] = color;
                                frameBuffer->colorDepth[y][x] = depth;
                            }
                            x1+=m;
                        }
                        break;
                }
            }
        }else{
            if(c){
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->depth[y][x] <= depth){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }else if(!frameBuffer->data[y][x]){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            if(frameBuffer->depth[y][x] <= depth){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }else if(!frameBuffer->data[y][x]){
                                frameBuffer->data[y][x] = c;
                                frameBuffer->depth[y][x] = depth;
                            }
                            x1+=m;
                        }
                        break;
                }
            }
        }
    }else{
        if(frameBuffer->hasColor && (color != -1)){
            if(c){
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            frameBuffer->data[y][x] = c;
                            frameBuffer->color[y][x] = color;
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            frameBuffer->data[y][x] = c;
                            frameBuffer->color[y][x] = color;
                            x1+=m;
                        }
                        break;
                }
            }else{
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            frameBuffer->color[y][x] = color;
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            frameBuffer->color[y][x] = color;
                            x1+=m;
                        }
                        break;
                }
            }
        }else{
            if(c){
                switch(dir){
                    case 0:
                        for(x1; x1 <= x2 + 0.5; x1++){
                            x = x1;
                            y = y1;
                            frameBuffer->data[y][x] = c;
                            y1+=m;
                        }
                        break;
                    case 1:
                        for(y1; y1 <= y2 + 0.5; y1++){
                            x = x1;
                            y = y1;
                            frameBuffer->data[y][x] = c;
                            x1+=m;
                        }
                        break;
                }
            }
        }
    }
}
void renderLineToTerminal(wchar_t c, int color, double x1, double y1, double x2, double y2, Camera* cam){
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
                dir = 0;
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
                dir = 1;
            }
        }else{
            return;
        }
    }

    int x, y;


    if(color != -1){
        if(c){
            switch(dir){
                attr_set(0, color, NULL);
                case 0:
                    for(x1; x1 <= x2 + 0.5; x1++){
                        x = x1;
                        y = y1;
                        mvprintw(y, x, "%lc", c);
                        y1+=m;
                    }
                    break;
                case 1:
                    for(y1; y1 <= y2 + 0.5; y1++){
                        x = x1;
                        y = y1;
                        mvprintw(y, x, "%lc", c);
                        x1+=m;
                    }
                    break;
                attr_set(0, 0, NULL);
            }
        }else{
            switch(dir){
                case 0:
                    for(x1; x1 <= x2 + 0.5; x1++){
                        x = x1;
                        y = y1;
                        mvprintw(y, x, "%lc", c);
                        y1+=m;
                    }
                    break;
                case 1:
                    for(y1; y1 <= y2 + 0.5; y1++){
                        x = x1;
                        y = y1;
                        mvprintw(y, x, "%lc", c);
                        x1+=m;
                    }
                    break;
            }
        }
    }
}
void drawCircleOnTexture(CharTexture* tex,float x, float y, float radius, wchar_t c){
    int tx, ty;
    for(int i = -radius; i <= radius; i++){
        for(int j = -radius; j <= radius; j++){
            tx = x + j;
            ty = y + i;
            if((tx >= 0)&&(tx < tex->w) && (ty >=0) && (ty < tex->h)){
                if(hypot(i, j) <= radius){
                    tex->data[ty][tx] = c;
                }
            }
            
        }
    }
}
void drawCircleOnTerminal(float x, float y, float radius, wchar_t c){
    int tx, ty;
    for(int i = -radius; i <= radius; i++){
        for(int j = -radius; j <= radius; j++){
            tx = x + j;
            ty = y + i;
            if((tx >= 0)&&(tx < scrW) && (ty >=0) && (ty < scrH)){
                if(hypot(i, j) <= radius){
                    mvprintw(ty, tx, "%lc", c);
                }
            }
            
        }
    }
}
void renderTexture(CharTexture* tex, int x, int y, Camera* cam, CharTexture* frameBuffer){
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
        if(frameBuffer->hasColor && tex->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && ((!frameBuffer->data[y1][i]) || (frameBuffer->depth[y1][i] <= tex->depth[v][u]))){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = tex->depth[v][u];
                    }
                    if((frameBuffer->colorDepth[y1][i] <= tex->colorDepth[v][u])){
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                        frameBuffer->colorDepth[y1][i] = tex->colorDepth[v][u];
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && ((!frameBuffer->data[y1][i]) || (frameBuffer->depth[y1][i] <= tex->depth[v][u]))){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = tex->depth[v][u];
                    }
                }
            }
        }
    }else{
        if(frameBuffer->hasColor && tex->hasColor){
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
        if(frameBuffer->hasColor && tex->hasColor){
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && ((!frameBuffer->data[y1][i]) || (frameBuffer->depth[y1][i] <= depth))){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = depth;
                    }
                    if((frameBuffer->colorDepth[y1][i] <= depth)){
                        frameBuffer->color[y1][i] = (tex->color[v][u]);
                        frameBuffer->colorDepth[y1][i] = depth;
                    }
                }
            }
        }else{
            for(y1; y1 <= y2; y1++, v++){
                u = x1 - x;
                for(int i = x1; i <= x2; i++, u++){
                    if((tex->data[v][u] != '\0') && ((!frameBuffer->data[y1][i]) || (frameBuffer->depth[y1][i] <= depth))){
                        frameBuffer->data[y1][i] = (tex->data[v][u]);
                        frameBuffer->depth[y1][i] = depth;
                    }
                }
            }
        }
    }else{
        if(frameBuffer->hasColor && tex->hasColor){
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
void renderFrameBuffer(CharTexture* frameBuffer){
    if(frameBuffer->hasColor){
        FOR(i, frameBuffer->h){
            move(i, 0);
            FOR(j, frameBuffer->w){
                if(frameBuffer->data[i][j]){
                    attr_set(frameBuffer->attr[i][j], frameBuffer->color[i][j], NULL);
                    printw("%lc", frameBuffer->data[i][j]);
                }else{
                    move(i, j+1);
                }
            }
        }
        attr_set(0, 0, NULL);
    }
}
void maskFrameBuffer(CharTexture* frameBuffer, CharTexture* mask){
    if(mask->h > frameBuffer->h || mask->w > frameBuffer->w) return;
    if(frameBuffer->hasColor){
        if(frameBuffer->hasDepth){
            FOR(i, mask->h){
                FOR(j, mask->w){
                    if(!(mask->data[i][j])){
                        frameBuffer->data[i][j] = '\0';
                        frameBuffer->color[i][j] = 0;
                        frameBuffer->depth[i][j] = 0;
                    }
                }
            }
        }else{
            FOR(i, mask->h){
                FOR(j, mask->w){
                    if(!(mask->data[i][j])){
                        frameBuffer->data[i][j] = '\0';
                        frameBuffer->color[i][j] = 0;
                    }
                }
            }
        }
    }else{
        if(frameBuffer->hasDepth){
            FOR(i, mask->h){
                FOR(j, mask->w){
                    if(!(mask->data[i][j])){
                        frameBuffer->data[i][j] = '\0';
                        frameBuffer->depth[i][j] = 0;
                    }
                }
            }
        }else{
            FOR(i, mask->h){
                FOR(j, mask->w){
                    if(!(mask->data[i][j])){
                        frameBuffer->data[i][j] = '\0';
                    }
                }
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

void addWchToFrameBuffer(CharTexture* frameBuffer , wchar_t c, int depth, int color, int attr){
    if(c){
        if(RenderBound.cut){
            if(RenderBound.wrap && (frameBuffer->cx >= RenderBound.x2)){
                frameBuffer->cx = RenderBound.x1;
                frameBuffer->cy++;
            }
            if((frameBuffer->cx >= 0) && (frameBuffer->cy >= 0) && (frameBuffer->cx < frameBuffer->w) && (frameBuffer->cy < frameBuffer->h) &&
                (frameBuffer->cx >= RenderBound.x1) && (frameBuffer->cy >= RenderBound.y1) && (frameBuffer->cx <= RenderBound.x2) && 
                (frameBuffer->cy <= RenderBound.y2)){
                if(depth >= frameBuffer->depth[frameBuffer->cy][frameBuffer->cx]){
                    frameBuffer->data[frameBuffer->cy][frameBuffer->cx] = c;
                    frameBuffer->depth[frameBuffer->cy][frameBuffer->cx] = depth;
                    attr_get(frameBuffer->attr[frameBuffer->cy] + frameBuffer->cx, frameBuffer->color[frameBuffer->cy] + frameBuffer->cx,  NULL);
                }
            }
            frameBuffer->cx++;
        }else{
            if(RenderBound.wrap && (frameBuffer->cx >= frameBuffer->w)){
                frameBuffer->cx = 0;
                frameBuffer->cy++;
            }
            if((frameBuffer->cx >= 0) && (frameBuffer->cy >= 0) && (frameBuffer->cx < frameBuffer->w) && (frameBuffer->cy < frameBuffer->h)){
                if(depth >= frameBuffer->depth[frameBuffer->cy][frameBuffer->cx]){
                    frameBuffer->data[frameBuffer->cy][frameBuffer->cx] = c;
                    frameBuffer->depth[frameBuffer->cy][frameBuffer->cx] = depth;
                    attr_get(frameBuffer->attr[frameBuffer->cy] + frameBuffer->cx, frameBuffer->color[frameBuffer->cy] + frameBuffer->cx,  NULL);
                }
            }
            frameBuffer->cx++;
        }
    }else{
        frameBuffer->cx++;
    }
}
void framBufferPrintW(CharTexture* frameBuffer, int depth, const char * format, ...){
    char* formatCopy = format;
    int n = 0;
    while(formatCopy[0]){
        if(formatCopy[0] == '%'){
            if(formatCopy[1] == '%') formatCopy++;
            else n++;
        }
        formatCopy++;
    }
    va_list args;
    va_start(args, n);

    char* tmp = vWriteLog(format, args);
    char* tmpCopy = tmp;

    va_end(args);

    if(RenderBound.cut){
        while(tmp[0]){
            if(RenderBound.wrap && (frameBuffer->cx >= RenderBound.x2)){
                frameBuffer->cx = RenderBound.x1;
                frameBuffer->cy++;
            }
            if((frameBuffer->cx >= 0) && (frameBuffer->cy >= 0) && (frameBuffer->cx < frameBuffer->w) && (frameBuffer->cy < frameBuffer->h) &&
                (frameBuffer->cx >= RenderBound.x1) && (frameBuffer->cy >= RenderBound.y1) && (frameBuffer->cx <= RenderBound.x2) && 
                (frameBuffer->cy <= RenderBound.y2)){
                if(depth >= frameBuffer->depth[frameBuffer->cy][frameBuffer->cx]){
                    frameBuffer->depth[frameBuffer->cy][frameBuffer->cx] = depth;
                    frameBuffer->data[frameBuffer->cy][frameBuffer->cx] = tmp[0];
                    attr_get(frameBuffer->attr[frameBuffer->cy] + frameBuffer->cx, frameBuffer->color[frameBuffer->cy] + frameBuffer->cx, NULL);
                }
            }
            frameBuffer->cx++;
            tmp++;
        }
    }else{
        while(tmp[0]){
            if(RenderBound.wrap && (frameBuffer->cx >= frameBuffer->w)){
                frameBuffer->cx = 0;
                frameBuffer->cy++;
            }
            if((frameBuffer->cx >= 0) && (frameBuffer->cy >= 0) && (frameBuffer->cx < frameBuffer->w) && (frameBuffer->cy < frameBuffer->h)){
                if(depth >= frameBuffer->depth[frameBuffer->cy][frameBuffer->cx]){
                    frameBuffer->depth[frameBuffer->cy][frameBuffer->cx] = depth;
                    frameBuffer->data[frameBuffer->cy][frameBuffer->cx] = tmp[0];
                    attr_get(frameBuffer->attr[frameBuffer->cy] + frameBuffer->cx, frameBuffer->color[frameBuffer->cy] + frameBuffer->cx, NULL);
                }
            }
            frameBuffer->cx++;
            tmp++;
        }
    }
    
    free(tmpCopy);
}

void moveInFrameBuffer(CharTexture* frameBuffer , int y, int x){
    frameBuffer->cx = x;
    frameBuffer->cy = y;
}

void setRenderBoundingWidget(struct Widget* w){
}
void setRenderBoundingBox(int x1, int y1, int x2, int y2){
    RenderBound.x1 = x1;
    RenderBound.y1 = y1;
    RenderBound.x2 = x2;
    RenderBound.y2 = y2;
    RenderBound.cut = 1;
}
void setWrapMode(int m){
    RenderBound.wrap = m;
}
void resetRenderBound(){
    RenderBound.cut = 0;
}