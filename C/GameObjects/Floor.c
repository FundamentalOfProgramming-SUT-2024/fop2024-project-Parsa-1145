#include "Floor.h"
#include "../Globals.h"

void deleteFloor(Floor* f){
    FOR(j, f->roomNum){
        deleteRoom(f->roomList[j]);
        deletePoint(f->pointCloud[j]);
        free(f->adjMat[j]);
    }
    free(f->roomList);
    free(f->pointCloud);
    free(f->adjMat);
    f->adjMat = NULL;
    f->pointCloud = NULL;
    f->roomList = NULL;
    

    ItemBase* tmp;
    FOR(j, f->itemList->size){
        tmp = linkedListGetElement(f->itemList, 0);
        tmp->deleteObject(tmp);
        popLinkedList(f->itemList);
    }
    free(f->itemList);

    deleteCharTexture(f->groundMesh);
    deleteCharTexture(f->visited);
    deleteCharTexture(f->featureMesh);
    f->itemList = NULL;
}