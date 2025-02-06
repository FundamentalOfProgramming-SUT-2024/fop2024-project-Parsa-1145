#ifndef RENDERER
#define RENDERER
#include "Camera.h"
#include "Texture.h"

struct Widget;




void renderLineToFrameBuffer(wchar_t c, int color, int depth, double x1, double y1, double x2, double y2, Camera* cam, CharTexture* frameBuffer);
void renderLineToTerminal(wchar_t c, int color, double x1, double y1, double x2, double y2, Camera* cam);

void drawCircleOnTexture(CharTexture* tex,float x, float y, float radius, wchar_t c);
void drawCircleOnTerminal(float x, float y, float radius, wchar_t c);


void renderTexture(CharTexture* tex, int x, int y, Camera* cam, CharTexture* frameBuffer);
void renderDepthlessTexture(CharTexture* tex,int x, int y, int depth, Camera* cam, CharTexture* frameBuffer);

void renderFrameBuffer(CharTexture* tex);
void maskFrameBuffer(CharTexture* frb,CharTexture* mask);
void colorMaskFrameBuffer(CharTexture* frb,CharTexture* mask);

void addWchToFrameBuffer(CharTexture* frameBuffer , wchar_t c, int depth, int color, int attr);
void moveInFrameBuffer(CharTexture* frameBuffer , int y, int x);
void framBufferPrintW(CharTexture* frameBuffer, int depth, const char * format, ...);

void setRenderBoundingWidget(struct Widget* w);
void setRenderBoundingBox(int x1, int y1, int x2, int y2);
void resetRenderBound();
void setWrapMode(int m);

#define mvAddWchToFrameBuffer(f , y, x, c, depth, color, attr) moveInFrameBuffer(f, y, x); addWchToFrameBuffer(f, c, depth, color, attr);
#define mvFramBufferPrintW(f , y, x, depth, format, ...) moveInFrameBuffer(f, y, x); framBufferPrintW(f, depth, format, ##__VA_ARGS__);

#endif