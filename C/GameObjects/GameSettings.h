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

    int maxHealth;

    int debugMode;
    int debugShowPointCloud;
    int noClip;
} GameSettings;



#endif