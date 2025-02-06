#ifndef IMAGEBOX
#define IMAGEBOX

#include "../GameObjects/Texture.h"
#include "../Globals.h"
#include "Widget.h"

typedef struct {
    UiBase* uiBase;
    Widget* widget;

    CharTexture* image;
    int dynamic;

    int bgColor;
}ImageBox;

void renderImageBox(ImageBox* iB);
void createImageBox(ImageBox* iB, Widget* parent, CharTexture* t, int color, int alignmentX, int alignmentY, int x, int y);
void deleteImageBox(ImageBox* iB);
void updateImageBox(ImageBox* iB);

#endif