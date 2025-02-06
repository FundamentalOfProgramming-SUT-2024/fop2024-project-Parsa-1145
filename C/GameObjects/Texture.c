#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include <ncurses.h>


#include "Texture.h"
#include "../Globals.h"

CharTexture* globalFrameBuffer;

CharTexture* createCharTexture(int w, int h, int hasDepth, int hasColor){
    CharTexture* out = calloc(1, sizeof(CharTexture));
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
    out->hasAttr = 0;

    out->cx = 0;
    out->cy = 0;
    out->x = 0;
    out->y = 0;

    return out;
}
CharTexture* createFrameBuffer(int w, int h){
    CharTexture* out = createCharTexture(w, h, 1, 1);
    out->attr = malloc(sizeof(unsigned int*) * h);
    FOR(i, h){
        out->attr[i] = calloc(w, sizeof(unsigned int));
    }
    out->hasAttr = 1;
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
        if(tex->attr){
            FOR(i, tex->h){
                free(tex->attr[i]);
            }
            free(tex->attr);
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
void fillAttrTexture(CharTexture* tex, unsigned int a){
    FOR(i, tex->h){
        FOR(j, tex->w){
            tex->attr[i][j] = a;
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
void resizeFrameBuffer(CharTexture** tex, int w, int h){
    deleteCharTexture(*tex);
    free(tex[0]);
    tex[0] = createFrameBuffer(scrW, scrH);
    
    fillCharTexture(tex[0], 0);
    fillColorTexture(tex[0], 0);
    fillDepthTexture(tex[0], 0);
    fillAttrTexture(tex[0], 0);
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
    cJSON* data = cJSON_GetArrayItem(cJSON_GetObjectItem(json, "data"), 0);
    cJSON* color = color = cJSON_GetObjectItem(json, "color");
    cJSON* depth = depth = cJSON_GetObjectItem(json, "depth");

    CharTexture* t = createCharTexture(cJSON_GetObjectItem(json, "width")->valueint, cJSON_GetObjectItem(json, "height")->valueint,
        depth, color);

    FOR(j, t->h){
        cJSON* tmp2 = cJSON_GetArrayItem(data, 0);

        FOR(k, t->w){
            t->data[j][k] = tmp2->valueint;
            tmp2 = tmp2->next;
        }

        data = data->next;
    }

    if(color){
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

    if(depth){
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

    return t;
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
CharTexture* loadCharTextureFromTxt(const char * const address){
    FILE* f = fopen(address, "rb");

    CharTexture* new = calloc(1, sizeof(CharTexture));

    int w = 0, h = 1, tmp = 0;

    int ch;
    while(!feof(f)){
        ch = fgetwc(f);
        if((ch == '\n') || (ch == '\0')){
            h++;
            if(tmp > w) w = tmp;
            tmp = 0;
        }else{
            tmp++;
        }
    }
    w++;

    fseek(f, 0, SEEK_SET);

    wchar_t **arr = calloc(h, sizeof(wchar_t*));
    FOR(i, h){
        arr[i] = calloc(w, sizeof(wchar_t));
    }
    fclose(f);
    f = fopen(address, "r");
    FOR(i, h){
        FOR(j, w){
            arr[i][j] = fgetwc(f);
            if((arr[i][j] == '\n') || (arr[i][j] == EOF)){
                arr[i][j] = 0;
                break;
            }else if(arr[i][j] == ' '){
                arr[i][j] = 0;
            }
        }
    }
    fclose(f);

    new = createCharTexture(w - 1, h, 0, 1);
    FOR(i, h){
        FOR(j, w-1){
            new->data[i][j] = arr[i][j];
        }
    }

    return new;
}

void emptyFrameBuffer(CharTexture* f){
    FOR(i, f->h){
        FOR(j, f->w){
            f->data[i][j] = 0;
            f->color[i][j] = 0;
            f->colorDepth[i][j] = 0;
            f->depth[i][j] = 0;
            f->attr[i][j] = 0;
        }
    }
}