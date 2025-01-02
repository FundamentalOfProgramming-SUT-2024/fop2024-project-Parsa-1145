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
    Point** pointCloud;
    LinkedList* itemList;
    int roomNum;
    
    CharTexture* groundMesh;
    CharTexture* visited;
    ColorTexture* groundMeshColor;
}Floor;

#endif