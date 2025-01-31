#ifndef FLOOR
#define FLOOR

#include"Room.h"
#include "Texture.h"
#include "./Items/ItemBase.h"
#include "../Utilities/PointCloud.h"
#include "../Utilities/LinkedList.h"

typedef struct{
    int w, h;

    Room** roomList;
    Room* rootRoom;
    Point** pointCloud;
    LinkedList* itemList;
    int** adjMat;
    int roomNum;
    int index;
    int stairRooms[2];
    
    CharTexture* groundMesh;
    CharTexture* featureMesh;
    CharTexture* visited;
    CharTexture* visionMesh;
    CharTexture* pathFindMesh;
}Floor;
void deleteFloor(Floor* f);

#endif