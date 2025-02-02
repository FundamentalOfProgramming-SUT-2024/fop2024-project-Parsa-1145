#ifndef RENDERER
#define RENDERER
#include "Camera.h"
#include "Texture.h"

void renderLineToFrameBuffer(wchar_t c, int color, int depth, double x1, double y1, double x2, double y2, Camera* cam, CharTexture* frameBuffer);
void renderLineToTerminal(wchar_t c, int color, double x1, double y1, double x2, double y2, Camera* cam);

void drawCircleOnTexture(CharTexture* tex,float x, float y, float radius, wchar_t c);
void drawCircleOnTerminal(float x, float y, float radius, wchar_t c);


void renderTexture(CharTexture* tex, int x, int y, Camera* cam, CharTexture* frameBuffer);
void renderDepthlessTexture(CharTexture* tex,int x, int y, int depth, Camera* cam, CharTexture* frameBuffer);

void renderFrameBuffer(CharTexture* tex);
void maskFrameBuffer(CharTexture* frb,CharTexture* mask);
void colorMaskFrameBuffer(CharTexture* frb,CharTexture* mask);

#endif