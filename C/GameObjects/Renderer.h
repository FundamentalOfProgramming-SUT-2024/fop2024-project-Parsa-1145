#ifndef RENDERER
#define RENDERER
#include "Camera.h"
#include "Texture.h"

void renderLine(wchar_t c, float x1, float y1, float x2, float y2, Camera* cam, CharTexture* frameBuffer);
void renderTexture(CharTexture* tex, float x, float y, Camera* cam, CharTexture* frameBuffer);
void renderFrameBuffer(CharTexture* tex);
void maskFrameBuffer(CharTexture* frb,CharTexture* mask);
void colorMaskFrameBuffer(CharTexture* frb,CharTexture* mask);

#define mrenderLine(z, x1,y1,x2,y2, frb, clb) renderLine(z, x1, y1, x2, y2, NULL, frb, clb);
#define mrenderHLine(z, x, y, l, frb, clb) renderLine(z, x, y,x + l-1, y, NULL, frb, clb);
#define mrenderVLine(z, x, y, l, frb, clb) renderLine(z, x, y, x, y + l-1, NULL, frb, clb);
#define renderHLine(z, x, y, l, c, frb, clb) renderLine(z, x, y,x + l-1, y, c, frb, clb);
#define renderVLine(z, x, y, l, c, frb, clb) renderLine(z, x, y, x, y + l-1, c, frb, clb);
#define mrenderTexture(a, b, c, d, frb, clb) renderTexture(a, b, c, d, NULL, frb, clb);

#endif