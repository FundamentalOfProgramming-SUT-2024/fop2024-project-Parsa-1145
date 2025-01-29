#include<time.h>
#include <unistd.h>
#include <stdlib.h>

#include "SecondaryThread.h"
#include "Globals.h"

LinkedList callbacks;
pthread_t secondaryThread;
pthread_mutex_t callbackMutex;
//struct timespec curTime;
unsigned long long curTime;


void addTimedVoidCallback(void (*voidcb)(void*), int duration){
    TimedCallback* new = calloc(1, sizeof(TimedCallback));
    new->start = time(NULL);
    new->duration = duration;
    new->voidCb = voidcb;

    pthread_mutex_lock(&callbackMutex);
    linkedListPushBack(&callbacks, new);
    pthread_mutex_unlock(&callbackMutex);

}
void addTimed1ObjCallbackCallback(void (*obj1Cb)(void*), void* obj1, int duration){
    TimedCallback* new = calloc(1, sizeof(TimedCallback));
    new->start = time(NULL);
    new->duration = duration;
    new->obj1Cb = obj1Cb;
    new->obj1 = obj1;

    pthread_mutex_lock(&callbackMutex);
    linkedListPushBack(&callbacks, new);
    pthread_mutex_unlock(&callbackMutex);
}
void addTimed2ObjCallbackCallback(void (*obj2Cb)(void*, void*), void* obj1, void* obj2, int duration){
    TimedCallback* new = calloc(1, sizeof(TimedCallback));
    new->start = time(NULL);
    new->duration = duration;
    new->obj2Cb = obj2Cb;
    new->obj1 = obj1;
    new->obj2 = obj2;


    pthread_mutex_lock(&callbackMutex);
    linkedListPushBack(&callbacks, new);
    pthread_mutex_unlock(&callbackMutex);
}

void initSecondaryThread(){
    createLinkedList(&callbacks, sizeof(TimedCallback*));
    pthread_create(&secondaryThread, NULL, &updateSecondaryThread, NULL);
}
void* updateSecondaryThread(){
    while(!terminate){
        if(callbacks.size){
            pthread_mutex_lock(&callbackMutex);
            void** tmpPtr = callbacks.data;
            TimedCallback* ptr;

            //timespec_get(&curTime, TIME_UTC);
            curTime = time(NULL);
            while(tmpPtr){
                ptr = tmpPtr[1];
                tmpPtr = tmpPtr[0];
                if(curTime - ptr->start >= ptr->duration){
                    if(ptr->voidCb) ptr->voidCb();
                    else if(ptr->obj1Cb) ptr->obj1Cb(ptr->obj1);
                    else if(ptr->obj2Cb) ptr->obj2Cb(ptr->obj1, ptr->obj2);
                    free(ptr);
                    removeItemFromLinkedList(&callbacks, ptr);
                }
            }
            pthread_mutex_unlock(&callbackMutex);
        }
        sleep(1);
    }
}