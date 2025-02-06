#ifndef AUDIOMANAGER
#define AUDIOMANAGER
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../Utilities/LinkedList.h"


typedef struct{
    Mix_Chunk* data;
    char* name;
    char* address;
    int id;

    int channel;
    int volume;
}Audio;

typedef struct{
    int audioListNum;
    Audio* audioList;

    Audio* current;
}AudioManager;

extern AudioManager audioManager;

void changeAudio(Audio* target, unsigned int ms);
void playEffectByName(const char * const name);
void playEffect(Audio* a);
Audio* getAudioByName(const char * const);
void initAudioManager();
void freeAudioManager();
void stopCurrentMuisc(unsigned int ms);
void resumeCurrentMusic(unsigned int ms);
void lowerCurrentMusic(unsigned int ms);
void resetCurrentMusicVolume(unsigned int ms);
void setMusicVolume();

#endif