#ifndef GAMESETTINGS
#define GAMESETTINGS

typedef struct{
    int difficaulity;

    int minRoomSize;
    int maxRoomSize;
    int minRoomNum;
    int maxRoomNum;
    int minFloorNum;
    int maxFloorNum;
    int roomSpread;
    int roomThemeNum;
    float* roomThemeProb;

    int baseMaxHealth;
    int baseHealthRegenTime;
    int baseHealthRegenAmount;
    int baseHungerTime;
    int baseStrength;
    int baseSpeed;
    int maxFullness;

    int debugMode;
    int debugShowPointCloud;
    int noClip;
    int debugItemInfo;
    int debugSeeAll;
    int debugMapGenerationStepped;
    int debugMapGenerationLayer;
} GameSettings;



#endif