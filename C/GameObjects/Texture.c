#include <stdlib.h>
#include <math.h>

#include "Texture.h"
#include "../Globals.h"

CharTexture* createCharTexture(int w, int h){
    CharTexture* out = malloc(sizeof(CharTexture));
    out->w = w;
    out->h = h;

    out->data = malloc(sizeof(char*) * h);
    FOR(i, h){
        out->data[i] = calloc(w , sizeof(char));
    }

    return out;
    
}
void fillCharTexture(CharTexture* tex, char c){
    FOR(i, tex->h){
        FOR(j, tex->w){
            tex->data[i][j] = c;
        }
    }
}
void deleteCharTexture(CharTexture* tex){
    if(tex){
        FOR(i, tex->h){
            free(tex->data[i]);
        }
        free(tex->data);

    }
}
void resizeCharTexture(CharTexture** tex, int w, int h){
    deleteCharTexture(*tex);
    free(*tex);
    *tex = createCharTexture(scrW, scrH);
    fillCharTexture(*tex, '\0');
}
void drawCircleOnCharTexture(CharTexture* tex,float x, float y, float radius, char c){
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
void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, char c);
ColorTexture* createColorTexture(int w, int h);