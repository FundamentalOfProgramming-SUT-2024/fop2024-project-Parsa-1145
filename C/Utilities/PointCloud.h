#ifndef POINTCLOUD
#define POINTCLOUD

typedef struct{
    float x, y;
    float radius;
    int locked;
}Point;

void iteratePointCloud(Point** points, int n, int spread);
float pointGetDistance(Point* p1, Point* p2);
void pointRender(Point* p);
#endif