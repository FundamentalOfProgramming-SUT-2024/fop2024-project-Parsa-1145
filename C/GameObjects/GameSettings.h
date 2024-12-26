#ifndef GAMESETTINGS
#define GAMESETTINGS

typedef struct{
    int difficaulity;
    int minRoomSize;
    int maxRoomSize;
    int minRoomNumber;
    int maxRoomNumber;
    int roomSpread;

    int debugMode;
    int debugShowPointCloud;
} GameSettings;


#endif