#include<string.h>
#include <stdlib.h>

#include "Theme.h"
#include "../Globals.h"

Theme* themes;

void loadTheme(cJSON* data, Theme* t){

    t->type = copyString(cJSON_GetObjectItem(data, "type")->valuestring);
    t->id = cJSON_GetObjectItem(data, "id")->valueint;
    t->mist = cJSON_GetObjectItem(data, "mist")->valueint;
    t->color[0] = cJSON_GetObjectItem(data, "cr")->valueint;
    t->color[1] = cJSON_GetObjectItem(data, "cg")->valueint;
    t->color[2] = cJSON_GetObjectItem(data, "cb")->valueint;
    t->music = getAudioByName(cJSON_GetObjectItem(data, "backgroundMusic")->valuestring);
}
void initThemes(){
    cJSON* json = openJsonFile("../Data/RoomThemes.json");

    int n = cJSON_GetArraySize(json);

    themes = calloc(n, sizeof(Theme));

    cJSON* tmp = json->child;
    FOR(i, n){
        loadTheme(tmp, themes + i);
        tmp = tmp->next;
    }
    cJSON_free(json);
}
