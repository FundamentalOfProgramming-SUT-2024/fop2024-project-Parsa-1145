#ifndef GAMESETTINGS
#define GAMESETTINGS

typedef struct{
    int difficaulity;

    int minRoomSize;
    int maxRoomSize;
    int minRoomNumber;
    int maxRoomNumber;
    int roomSpread;
    int roomThemeNum;
    float* roomThemeProb;

    int debugMode;
    int debugShowPointCloud;
    int noClip;
} GameSettings;


#endif