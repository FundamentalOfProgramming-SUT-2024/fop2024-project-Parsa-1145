#ifndef TEXTUREMANAGER
#define TEXTUREMANAGER
#include "../Utilities/LinkedList.h"
#include "../Utilities/cJSON.h"
#include "Texture.h"


typedef struct{
    CharTexture* data;
    char* name;
    char* address;
    int id;
}AsciiTexture;

typedef struct{
    int textureNum;
    AsciiTexture* textureList;
}TextureManager;

extern TextureManager textureManager;

AsciiTexture* getTextureByName(const char * const);
AsciiTexture* getTextureByName(const char * const);

void initTextureManager();


#endif