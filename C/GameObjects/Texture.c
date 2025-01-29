#include <stdlib.h>
#include <math.h>
#include <wchar.h>


#include "Texture.h"
#include "../Globals.h"

CharTexture* createCharTexture(int w, int h){
    CharTexture* out = malloc(sizeof(CharTexture));
    out->w = w;
    out->h = h;

    out->data = malloc(sizeof(wchar_t*) * h);
    FOR(i, h){
        out->data[i] = calloc(w , sizeof(wchar_t));
    }
    out->color = malloc(sizeof(unsigned char*) * h);
    FOR(i, h){
        out->color[i] = calloc(w, sizeof(unsigned char*));
    }

    return out;
    
}
void fillColorTexture(CharTexture* tex, unsigned char c){
    FOR(i, tex->h){
        FOR(j, tex->w){
            tex->color[i][j] = c;
        }
    }
}
void fillCharTexture(CharTexture* tex, wchar_t c){
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
        FOR(i, tex->h){
            free(tex->color[i]);
        }
        free(tex->color);
    }
}
void resizeCharTexture(CharTexture** tex, int w, int h){
    deleteCharTexture(*tex);
    free(*tex);
    *tex = createCharTexture(scrW, scrH);
    fillCharTexture(*tex, '\0');
    fillColorTexture(*tex, 0);
}
void drawCircleOnCharTexture(CharTexture* tex,float x, float y, float radius, wchar_t c){
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
void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, wchar_t c);

cJSON* saveCharTextureToJson(CharTexture* t){
    cJSON* json = cJSON_CreateArray();
    FOR(j, t->h){
        cJSON* tmp1 = cJSON_CreateArray();

        FOR(k, t->w){
            cJSON_AddItemToArray(tmp1, cJSON_CreateNumber(t->data[j][k]));
        }
        cJSON_AddItemToArray(json, tmp1);
    }
    return json;
}
cJSON* saveColorTextureToJson(CharTexture* t){
    cJSON* json = cJSON_CreateArray();
    FOR(j, t->h){
        cJSON* tmp1 = cJSON_CreateArray();

        FOR(k, t->w){
            cJSON_AddItemToArray(tmp1, cJSON_CreateNumber(t->color[j][k]));
        }
        cJSON_AddItemToArray(json, tmp1);
    }
    return json;
}
void loadCharTextureFromJson(cJSON* data, CharTexture* t){
    cJSON* tmp1 = cJSON_GetArrayItem(data, 0);
    FOR(j, t->h){
        cJSON* tmp2 = cJSON_GetArrayItem(tmp1, 0);

        FOR(k, t->w){
            t->data[j][k] = tmp2->valueint;
            tmp2 = tmp2->next;
        }
        tmp1 = tmp1->next;
    }
}
void loadColorTextureFromJson(cJSON* data, CharTexture* t){
    cJSON* tmp1 = cJSON_GetArrayItem(data, 0);
    FOR(j, t->h){
        cJSON* tmp2 = cJSON_GetArrayItem(tmp1, 0);

        FOR(k, t->w){
            t->color[j][k] = tmp2->valueint;
            tmp2 = tmp2->next;
        }
        tmp1 = tmp1->next;
    }
}