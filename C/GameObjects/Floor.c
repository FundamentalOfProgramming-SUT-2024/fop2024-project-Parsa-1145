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

cJSON* saveFloorToJson(Floor* f){
    cJSON* fJ = cJSON_CreateObject();

    cJSON_AddNumberToObject(fJ, "width", f->w);
    cJSON_AddNumberToObject(fJ, "height", f->h);

    cJSON_AddItemToObject(fJ, "groundMesh", saveCharTextureToJson(f->groundMesh));
    cJSON_AddItemToObject(fJ, "featureMesh", saveCharTextureToJson(f->featureMesh));
    cJSON_AddItemToObject(fJ, "visited", saveCharTextureToJson(f->visited));

    cJSON* itemsJ = cJSON_CreateArray();
    {
        void** tmpPtr = f->itemList->data;
        ItemBase* ptr;

        while(tmpPtr){
            ptr = tmpPtr[1];
            tmpPtr = tmpPtr[0];

            cJSON_AddItemToArray(itemsJ, itemToJson(ptr));
        }
    }
    cJSON_AddItemToObject(fJ, "items", itemsJ);

    return fJ;
}
void loadFloorFromJson(cJSON* data, Floor* f){
    f->w = cJSON_GetObjectItem(data, "width")->valueint;
    f->h = cJSON_GetObjectItem(data, "height")->valueint;

    f->groundMesh = loadCharTextureFromJson(cJSON_GetObjectItem(data, "groundMesh"));
    f->featureMesh = loadCharTextureFromJson(cJSON_GetObjectItem(data, "featureMesh"));
    f->visited = loadCharTextureFromJson(cJSON_GetObjectItem(data, "visited"));
    f->pathFindMesh = createCharTexture(f->w, f->h, 0, 0);
    f->visionMesh = createCharTexture(f->w, f->h, 0, 0);

    f->itemList = calloc(1, sizeof(LinkedList));
    createLinkedList(f->itemList, sizeof(ItemBase*));

    {
        cJSON* item = cJSON_GetObjectItem(data, "items")->child;

        while(item){
            linkedListPushBack(f->itemList, loadItemFromJson(item));
            item = item->next;
        }
    }

    return f;

}