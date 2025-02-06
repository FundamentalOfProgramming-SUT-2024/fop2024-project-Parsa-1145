#include "GameSettings.h"
#include "../Globals.h"


cJSON* saveGameSettingsToJson(GameSettings* g){
    cJSON* gameSettingsJ = cJSON_CreateObject();

    cJSON_AddNumberToObject(gameSettingsJ, "visionRadius", g->visionRadius);
    cJSON_AddNumberToObject(gameSettingsJ, "fullVision", g->fullVision);
    cJSON_AddNumberToObject(gameSettingsJ, "rememberItems", g->rememberItems);
    cJSON_AddNumberToObject(gameSettingsJ, "baseMaxHealth", g->baseMaxHealth);
    cJSON_AddNumberToObject(gameSettingsJ, "baseHealthRegenTime", g->baseHealthRegenTime);
    cJSON_AddNumberToObject(gameSettingsJ, "baseHealthRegenAmount", g->baseHealthRegenAmount);
    cJSON_AddNumberToObject(gameSettingsJ, "baseHungerTime", g->baseHungerTime);
    cJSON_AddNumberToObject(gameSettingsJ, "baseStrength", g->baseStrength);
    cJSON_AddNumberToObject(gameSettingsJ, "baseSpeed", g->baseSpeed);

    cJSON_AddNumberToObject(gameSettingsJ, "baseLuck", g->baseLuck);
    cJSON_AddNumberToObject(gameSettingsJ, "maxFullness", g->maxFullness);
    cJSON_AddNumberToObject(gameSettingsJ, "debugMode", g->debugMode);

    cJSON_AddNumberToObject(gameSettingsJ, "debugShowPointCloud", g->debugShowPointCloud);
    cJSON_AddNumberToObject(gameSettingsJ, "noClip", g->noClip);
    cJSON_AddNumberToObject(gameSettingsJ, "debugItemInfo", g->debugItemInfo);

    cJSON_AddNumberToObject(gameSettingsJ, "debugSeeAll", g->debugSeeAll);
    cJSON_AddNumberToObject(gameSettingsJ, "debugMapGenerationStepped", g->debugMapGenerationStepped);
    cJSON_AddNumberToObject(gameSettingsJ, "debugMapGenerationLayer", g->debugMapGenerationLayer);

    return gameSettingsJ;
}

GameSettings* loadGameSettingsFromJson(cJSON* data){
    gameSettings.visionRadius = cJSON_GetObjectItem(data, "visionRadius")->valuedouble;
    gameSettings.fullVision = cJSON_GetObjectItem(data, "fullVision")->valueint;
    gameSettings.rememberItems = cJSON_GetObjectItem(data, "rememberItems")->valueint;
    gameSettings.baseMaxHealth = cJSON_GetObjectItem(data, "baseMaxHealth")->valueint;
    gameSettings.baseHealthRegenTime = cJSON_GetObjectItem(data, "baseHealthRegenTime")->valueint;
    gameSettings.baseHealthRegenAmount = cJSON_GetObjectItem(data, "baseHealthRegenAmount")->valueint;
    gameSettings.baseHungerTime = cJSON_GetObjectItem(data, "baseHungerTime")->valueint;
    gameSettings.baseStrength = cJSON_GetObjectItem(data, "baseStrength")->valueint;
    gameSettings.baseSpeed = cJSON_GetObjectItem(data, "baseSpeed")->valueint;

    gameSettings.baseLuck = cJSON_GetObjectItem(data, "baseLuck")->valueint;
    gameSettings.maxFullness = cJSON_GetObjectItem(data, "maxFullness")->valueint;
    gameSettings.debugMode = cJSON_GetObjectItem(data, "debugMode")->valueint;

    gameSettings.debugShowPointCloud = cJSON_GetObjectItem(data, "debugShowPointCloud")->valueint;
    gameSettings.noClip = cJSON_GetObjectItem(data, "noClip")->valueint;
    gameSettings.debugItemInfo = cJSON_GetObjectItem(data, "debugItemInfo")->valueint;

    gameSettings.debugSeeAll = cJSON_GetObjectItem(data, "debugSeeAll")->valueint;
    gameSettings.debugMapGenerationStepped = cJSON_GetObjectItem(data, "debugMapGenerationStepped")->valueint;
    gameSettings.debugMapGenerationLayer = cJSON_GetObjectItem(data, "debugMapGenerationLayer")->valueint;
}
