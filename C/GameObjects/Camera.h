#ifndef CAMERA
#define CAMERA

typedef struct{
    int x, y;
    int w, h;
}Camera;

extern Camera globalCamera;
#endif