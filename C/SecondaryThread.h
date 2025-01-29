#ifndef SECONDARYTHREAD
#define SECONDARYTHREAD
#include <pthread.h>


#include "Utilities/LinkedList.h"
#include "Utilities/TimedCallback.h"

extern LinkedList callbacks;
extern pthread_t secondaryThread;
extern pthread_mutex_t callbackMutex;


void addTimedVoidCallback(void (*voidcb)(void*), int duration);
void addTimed1ObjCallbackCallback(void (*obj1Cb)(void*), void* obj1, int duration);
void addTimed2ObjCallbackCallback(void (*obj2Cb)(void*, void*), void* obj1, void* obj2, int duration);

void initSecondaryThread();
void* updateSecondaryThread();

#endif