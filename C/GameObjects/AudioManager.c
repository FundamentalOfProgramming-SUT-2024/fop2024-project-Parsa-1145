#include "AudioManager.h"
#include "../Utilities/cJSON.h"
#include"../Globals.h"

AudioManager audioManager;

void loadAudioFromJson(cJSON* data, Audio* a){
    a->address = copyString(cJSON_GetObjectItem(data, "address")->valuestring);
    a->data = Mix_LoadWAV(a->address);
    a->id = cJSON_GetObjectItem(data, "id")->valueint;
    a->name = copyString(cJSON_GetObjectItem(data, "name")->valuestring);
}

void initAudioManager(){
    cJSON* json = openJsonFile("../Data/AudioData.json");

    audioManager.audioListNum = cJSON_GetArraySize(json);
    audioManager.audioList = calloc(audioManager.audioListNum, sizeof(Audio));

    cJSON* data = json->child;
    FOR(i, audioManager.audioListNum){
        loadAudioFromJson(data, audioManager.audioList+i);
        data = data->next;
    }

    cJSON_free(json);
}

void changeAudio(Audio* target, unsigned int ms){
    if(!audioManager.current){
        target->data->volume = 32;
        target->channel = Mix_FadeInChannel(-1, target->data, -1, ms);
        audioManager.current = target;
    }else if(target != audioManager.current){
        Mix_FadeOutChannel(audioManager.current->channel, ms);
        target->data->volume = 32;
        target->channel = Mix_FadeInChannel(-1, target->data, -1, ms);

        audioManager.current = target;
    }
}

Audio* getAudioByName(const char * const name){
    FOR(i, audioManager.audioListNum){
        if(!strcmp(audioManager.audioList[i].name, name)){
            return audioManager.audioList + i;
        }
    }
    return NULL;
}
void freeAudioManager(){
    FOR(i, audioManager.audioListNum){
        Mix_FreeChunk(audioManager.audioList[i].data);
    }
    Mix_CloseAudio();
    SDL_Quit();
}
void playEffectByName(const char * const name){
    Audio* a = getAudioByName(name);
    a->data->volume = 64;
    Mix_PlayChannel(-1, a->data, 0);
}
void playEffect(Audio* a){
    a->data->volume = 64;
    Mix_PlayChannel(-1, a->data, 0);
}