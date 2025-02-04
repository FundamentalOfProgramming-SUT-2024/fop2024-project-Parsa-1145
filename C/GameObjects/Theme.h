#ifndef THEME
#define THEME
#include "../Utilities/cJSON.h"
#include "AudioManager.h"

typedef struct{
    char* type;
    char id;
    int color[3];
    int mist;

    Audio* music;


}Theme;

extern Theme* themes;

void initThemes();

#endif