#ifndef POINTCLOUD
#define POINTCLOUD

typedef struct{
    float x, y;
    float radius;
    int locked;
    float xForce, yForce;
}Point;

int iteratePointCloud(Point** points, int n, int spread);
float pointGetDistance(Point* p1, Point* p2);
void pointRender(Point* p);
float pointForceFunction(float d, float thresh);
void deletePoint(Point* point);
#endif