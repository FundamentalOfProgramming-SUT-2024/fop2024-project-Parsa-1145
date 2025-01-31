#include "Floor.h"
#include "../Globals.h"

void deleteFloor(Floor* f){
    if(f->roomNum){
        FOR(j, f->roomNum){
            deleteRoom(f->roomList[j]);
            deletePoint(f->pointCloud[j]);
            free(f->adjMat[j]);
        }
        free(f->roomList);
        free(f->pointCloud);
        free(f->adjMat);
    }
    f->adjMat = NULL;
    f->pointCloud = NULL;
    f->roomList = NULL;
    

    while(f->itemList->size){
        defaultItemDelete((f->itemList->data)[1]);
        popLinkedList(f->itemList);
    }
    free(f->itemList);

    deleteCharTexture(f->groundMesh);
    deleteCharTexture(f->visited);
    deleteCharTexture(f->featureMesh);
    deleteCharTexture(f->pathFindMesh);
    f->itemList = NULL;
}