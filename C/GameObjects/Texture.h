#ifndef TEXTURE
#define TEXTURE
#include "wchar.h"
#include "../Utilities/cJSON.h"

typedef struct{
    wchar_t** data;
    char** depth;
    char** colorDepth;
    int** color;

    int hasDepth, hasColor;
    int w, h;
}CharTexture;

CharTexture* createCharTexture(int w, int h, int hasDepth, int hasColor);
void deleteCharTexture(CharTexture* tex);

void resizeCharTexture(CharTexture** tex, int w, int h);

void fillCharTexture(CharTexture* tex, wchar_t c);
void fillColorTexture(CharTexture* tex, int c);
void fillDepthTexture(CharTexture* tex, char c);

void mixTextures( CharTexture* t1, CharTexture* t2);

cJSON* saveCharTextureToJson(CharTexture* t);
CharTexture* loadCharTextureFromJson(cJSON* data);

#endif