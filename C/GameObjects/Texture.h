#ifndef TEXTURE
#define TEXTURE
#include "wchar.h"
#include "../Utilities/cJSON.h"
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/curses.h>

typedef struct{
    wchar_t** data;
    char** depth;
    char** colorDepth;
    int** color;
    unsigned int** attr;

    int hasDepth, hasColor, hasAttr;
    int w, h;
    int x, y;
    int cx, cy;
}CharTexture;

CharTexture* createCharTexture(int w, int h, int hasDepth, int hasColor);
CharTexture* createFrameBuffer(int w, int h);

void deleteCharTexture(CharTexture* tex);

void resizeCharTexture(CharTexture** tex, int w, int h);
void resizeFrameBuffer(CharTexture** tex, int w, int h);


void fillCharTexture(CharTexture* tex, wchar_t c);
void fillColorTexture(CharTexture* tex, int c);
void fillDepthTexture(CharTexture* tex, char c);
void fillAttrTexture(CharTexture* tex, unsigned int a);

void emptyFrameBuffer(CharTexture* tex);

void mixTextures( CharTexture* t1, CharTexture* t2);

cJSON* saveCharTextureToJson(CharTexture* t);
CharTexture* loadCharTextureFromJson(cJSON* data);
CharTexture* loadCharTextureFromTxt(const char * const address);


extern CharTexture* globalFrameBuffer;

#endif