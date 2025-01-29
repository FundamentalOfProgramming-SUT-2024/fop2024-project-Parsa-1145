#ifndef TIMEDCB
#define TIMEDCB

typedef struct{
    void* obj1;
    void* obj2;
    
    void (*voidCb)();
    void (*obj1Cb)(void*);
    void (*obj2Cb)(void*, void*);

    int start, duration;
}TimedCallback;




#endif