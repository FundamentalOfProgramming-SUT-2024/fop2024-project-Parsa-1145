#ifndef GAMESETTINGS
#define GAMESETTINGS
#include "../Utilities/cJSON.h"

typedef struct{
    int difficulty;
    char* difficultyName;

    int minRoomSize;
    int maxRoomSize;
    int minRoomNum;
    int maxRoomNum;
    int minFloorNum;
    int maxFloorNum;
    int roomSpread;
    int roomThemeNum;
    float* roomThemeProb;

    float visionRadius;
    int fullVision;
    int rememberItems;

    float lootRarityMultiplier;

    int baseMaxHealth;
    int baseHealthRegenTime;
    int baseHealthRegenAmount;
    int baseHungerTime;
    int baseStrength;
    int baseSpeed;
    int baseLuck;
    int maxFullness;

    int debugMode;
    int debugShowPointCloud;
    int noClip;
    int debugItemInfo;
    int debugSeeAll;
    int debugMapGenerationStepped;
    int debugMapGenerationLayer;
} GameSettings;

cJSON* saveGameSettingsToJson(GameSettings* g);
GameSettings* loadGameSettingsFromJson(cJSON* data);

#endif