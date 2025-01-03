#include "LinkedList.h"
#include "stdlib.h"
#include "../GlobalDefines.h"

void createLinkedList(LinkedList* linkedList, int dataSize){
    linkedList->dataSize = dataSize;
}

void** linkedListGetElement(LinkedList* list, int index){
    if(index >= list->size){
        return NULL;
    }else{
        void** out = list->data;
        FOR(i, index){
            out = *out;
        }
        return *(out + 1);
    }
}
void** getLinkedListPtr(LinkedList* list, int index){
    if(index >= list->size){
        return NULL;
    }else{
        void** out = list->data;
        FOR(i, index){
            out = *out;
        }
        return out;
    }
}
void linkedListDeleteElement(LinkedList* list, int index){
    if(index < list->size){
        if(index == 0){
            popLinkedList(list);
        }else{
            void** tmp = (getLinkedListPtr(list, index));
            *(getLinkedListPtr(list, index-1)) = *tmp;
            free(tmp);
            list->size--;
        }
    }
}
void linkedListPushBack(LinkedList* list, void* elment){
    void** tmp = malloc(sizeof(void*) + list->dataSize);
    if(list->size){
        *(getLinkedListPtr(list, list->size - 1)) = tmp;
    }else{
        list->data = tmp;
    }

    tmp[0] = NULL;
    tmp[1] = elment;
    list->size++;
}
//void insetInLinkedList(LinkedList* list, void* element, int index);
void popLinkedList(LinkedList* list){
    if(list->size){
        void** tmp = list->data;
        list->data = *(list->data);
        free(tmp);
        list->size--;
    }
}

void removeItemFromLinkedList(LinkedList* list, void* element){
    void** tmp = list->data;
    void** prev = NULL;

    FOR(i, list->size){
        if(tmp[1] == element){
            if(prev){
                prev[0] = tmp[0];
                free(tmp);
                list->size--;
            }else{
                popLinkedList(list);
            }
            break;
        }else{
            prev = tmp;
            if(i != list->size) tmp = tmp[0];
        }
    }

}