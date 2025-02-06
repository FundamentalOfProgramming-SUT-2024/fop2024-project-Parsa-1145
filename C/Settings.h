#ifndef SETTINGS
#define SETTINGS

typedef enum{
    terraria,
    hollowKnigth
}Soundtrack;

typedef struct{
    int musicVolume, effectsVolume;
    Soundtrack Soundtrack;
}Settings;


void initSettings();


#endif