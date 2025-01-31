#ifndef TEXTURE
#define TEXTURE
#include "wchar.h"
#include "../Utilities/cJSON.h"

typedef struct{
    wchar_t** data;
    unsigned char** color;
    int w, h;
}CharTexture;

CharTexture* createCharTexture(int w, int h);
void fillCharTexture(CharTexture* tex, wchar_t c);
void fillColorTexture(CharTexture* tex, unsigned char c);
void mixTextures( CharTexture* t1, CharTexture* t2);
void drawCircleOnCharTexture(CharTexture* tex,float x, float y, float radius, wchar_t c);
void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, wchar_t c);
void deleteCharTexture(CharTexture* tex);
void resizeCharTexture(CharTexture** tex, int w, int h);
cJSON* saveCharTextureToJson(CharTexture* t);
cJSON* saveColorTextureToJson(CharTexture* t);

void loadCharTextureFromJson(cJSON* data, CharTexture* t);
void loadColorTextureFromJson(cJSON* data, CharTexture* t);




#endif