#include <stdlib.h>
#include <math.h>
#include <wchar.h>


#include "Texture.h"
#include "../Globals.h"

CharTexture* createCharTexture(int w, int h, int hasDepth, int hasColor){
    CharTexture* out = malloc(sizeof(CharTexture));
    out->w = w;
    out->h = h;

    out->data = malloc(sizeof(wchar_t*) * h);
    FOR(i, h){
        out->data[i] = calloc(w , sizeof(wchar_t));
    }

    if(hasColor){
        out->color = malloc(sizeof(int*) * h);
        FOR(i, h){
            out->color[i] = calloc(w, sizeof(int));
        }
    }

    if(hasDepth){
        out->depth = malloc(sizeof(char*) * h);
        FOR(i, h){
            out->depth[i] = calloc(w, sizeof(char));
        }
        out->colorDepth = malloc(sizeof(char*) * h);
        FOR(i, h){
            out->colorDepth[i] = calloc(w, sizeof(char));
        }
    }

    out->hasColor = hasColor;
    out->hasDepth = hasDepth;

    return out;
    
}
void fillColorTexture(CharTexture* tex, int c){
    if(tex->hasColor){
        FOR(i, tex->h){
            FOR(j, tex->w){
                tex->color[i][j] = c;
            }
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
        if(tex->hasColor){
            FOR(i, tex->h){
                free(tex->color[i]);
            }
            free(tex->color);
        }
        if(tex->hasDepth){
            FOR(i, tex->h){
                free(tex->depth[i]);
            }
            free(tex->depth);
        }
    }
}
void fillDepthTexture(CharTexture* tex, char c){
    if(tex->hasDepth){
        FOR(i, tex->h){
            FOR(j, tex->w){
                tex->depth[i][j] = c;
            }
        }
        FOR(i, tex->h){
            FOR(j, tex->w){
                tex->colorDepth[i][j] = c;
            }
        }
    }
}
void resizeCharTexture(CharTexture** tex, int w, int h){
    deleteCharTexture(*tex);
    free(tex[0]);
    tex[0] = createCharTexture(scrW, scrH, tex[0]->hasColor, tex[0]->hasColor);
    
    fillCharTexture(*tex, '\0');

    if(tex[0]->hasColor){
        fillColorTexture(tex[0], 0);
    }
    if(tex[0]->hasDepth){
        fillDepthTexture(tex[0], 0);
    }
}

void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, wchar_t c);

cJSON* saveCharTextureToJson(CharTexture* t){
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "width", t->w);
    cJSON_AddNumberToObject(json, "height", t->h);

    cJSON* data = cJSON_CreateArray();
    FOR(j, t->h){
        cJSON* tmp1 = cJSON_CreateArray();
        
        FOR(k, t->w){
            cJSON_AddItemToArray(tmp1, cJSON_CreateNumber(t->data[j][k]));
        }
        cJSON_AddItemToArray(data, tmp1);
    }
    cJSON_AddItemToObject(json, "data", data);

    if(t->hasColor){
        cJSON* color = cJSON_CreateArray();
        FOR(j, t->h){
            cJSON* tmp1 = cJSON_CreateArray();
            
            FOR(k, t->w){
                cJSON_AddItemToArray(tmp1, cJSON_CreateNumber(t->color[j][k]));
            }
            cJSON_AddItemToArray(color, tmp1);
        }
        cJSON_AddItemToObject(json, "color", color);
    }
    if(t->hasDepth){
        cJSON* depth = cJSON_CreateArray();
        FOR(j, t->h){
            cJSON* tmp1 = cJSON_CreateArray();
            
            FOR(k, t->w){
                cJSON_AddItemToArray(tmp1, cJSON_CreateNumber(t->depth[j][k]));
            }
            cJSON_AddItemToArray(depth, tmp1);
        }
        cJSON_AddItemToObject(json, "depth", depth);
    }
    return json;
}
CharTexture* loadCharTextureFromJson(cJSON* json){
    CharTexture* t = calloc(1, sizeof(CharTexture));

    t->w = cJSON_GetObjectItem(json, "width")->valueint;
    t->h = cJSON_GetObjectItem(json, "height")->valueint;

    cJSON* data = cJSON_GetArrayItem(cJSON_GetObjectItem(json, "data"), 0);
    FOR(j, t->h){
        cJSON* tmp2 = cJSON_GetArrayItem(data, 0);

        FOR(k, t->w){
            t->data[j][k] = tmp2->valueint;
            tmp2 = tmp2->next;
        }

        data = data->next;
    }
    
    cJSON* color;
    if(color = cJSON_GetObjectItem(json, "color")){
        t->hasColor = 1;
        color = color->child;
        FOR(j, t->h){
            cJSON* tmp2 = cJSON_GetArrayItem(color, 0);

            FOR(k, t->w){
                t->color[j][k] = tmp2->valueint;
                tmp2 = tmp2->next;
            }

            color = color->next;
        }
    }

    cJSON* depth;
    if(depth = cJSON_GetObjectItem(json, "depth")){
        t->hasDepth = 1;
        depth = depth->child;
        FOR(j, t->h){
            cJSON* tmp2 = cJSON_GetArrayItem(depth, 0);

            FOR(k, t->w){
                t->depth[j][k] = tmp2->valueint;
                tmp2 = tmp2->next;
            }

            depth = depth->next;
        }
    }
}

void mixTextures( CharTexture* t1, CharTexture* t2){
    FOR(i, t1->w){
        FOR(j, t2->h){
            if(!t1->data[j][i] && t2->data[j][i]){
                t1->data[j][i] = t2->data[j][i];
            }
        }
    }
}