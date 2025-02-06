#include "TextureManager.h"
#include "../Globals.h"


TextureManager textureManager;

void loadTextureFromJson(cJSON* data, AsciiTexture* a){
    a->address = copyString(cJSON_GetObjectItem(data, "address")->valuestring);
    a->data = loadCharTextureFromTxt(a->address);
    a->id = cJSON_GetObjectItem(data, "id")->valueint;
    a->name = copyString(cJSON_GetObjectItem(data, "name")->valuestring);
}

AsciiTexture* getTextureByName(const char * const name){
    FOR(i, textureManager.textureNum){
        if(!strcmp(textureManager.textureList[i].name, name)){
            return textureManager.textureList + i;
        }
    }
    return NULL;
}
void initTextureManager(){
    cJSON* json = openJsonFile("../Data/TextureData.json");

    textureManager.textureNum = cJSON_GetArraySize(json);
    textureManager.textureList = calloc(textureManager.textureNum, sizeof(AsciiTexture));

    cJSON* data = json->child;
    FOR(i, textureManager.textureNum){
        loadTextureFromJson(data, textureManager.textureList+i);
        data = data->next;
    }

    cJSON_free(json);
}
