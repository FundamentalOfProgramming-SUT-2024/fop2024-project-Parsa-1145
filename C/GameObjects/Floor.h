#ifndef FLOOR
#define FLOOR

#include"Room.h"
#include "Texture.h"
#include "./Items/ItemBase.h"
#include "../Utilities/PointCloud.h"
#include "../Utilities/LinkedList.h"

typedef struct{
    int minx, miny, maxx, maxy;

    Room** roomList;
    Room* rootRoom;
    Point** pointCloud;
    LinkedList* itemList;
    int** adjMat;
    int roomNum;
    int index;
    
    CharTexture* groundMesh;
    CharTexture* featureMesh;
    CharTexture* visited;
    ColorTexture* groundMeshColor;
}Floor;
void deleteFloor(Floor* f);

#endif