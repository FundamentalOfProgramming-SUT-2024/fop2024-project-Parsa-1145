#ifndef TEXTURE
#define TEXTURE

typedef struct{
    char** data;
    int w, h;
}CharTexture;
typedef struct{
    short** data[3];
    int w, h;
}ColorTexture;

CharTexture* createCharTexture(int w, int h);
ColorTexture* createColorTexture(int w, int h);
void fillCharTexture(CharTexture* tex, char c);
void drawCircleOnCharTexture(CharTexture* tex,float x, float y, float radius, char c);
void drawRectangleOnCharTexture(CharTexture* tex, float x, float y, float w, float h, char c);
void deleteCharTexture(CharTexture* tex);
void resizeCharTexture(CharTexture** tex, int w, int h);


#endif