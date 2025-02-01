#ifndef RENDERER
#define RENDERER
#include "Camera.h"
#include "Texture.h"

void renderLine(wchar_t c, int color, float length, double x1, double y1, double x2, double y2, Camera* cam, CharTexture* frameBuffer);
void renderTexture(CharTexture* tex, float x, float y, Camera* cam, CharTexture* frameBuffer);
void renderFrameBuffer(CharTexture* tex);
void maskFrameBuffer(CharTexture* frb,CharTexture* mask);
void colorMaskFrameBuffer(CharTexture* frb,CharTexture* mask);

#endif