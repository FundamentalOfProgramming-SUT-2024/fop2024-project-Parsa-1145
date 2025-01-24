#ifndef TEXTURE
#define TEXTURE
#include "wchar.h"

typedef struct{
    wchar_t** data;
    int w, h;
}CharTexture;
typedef struct{
    unsigned char** data;
    int w, h;
}ColorTexture;

CharTexture* createCharTexture(int w, int h);
ColorTexture* createColorTexture(int w, int h);
void fillCharTexture(CharTexture* tex, wchar_t c);
void drawCircleOnCharTexture(CharTexture* tex,float x, float y, float radius, wchar_t c);
void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, wchar_t c);
void deleteCharTexture(CharTexture* tex);
void resizeCharTexture(CharTexture** tex, int w, int h);


#endif