#include "PointCloud.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

void iteratePointCloud(Point** points, int n, int spread){
    float distance, spreadAmount, newDistance, minDistance = 0;
    int minDisIndex = 0;
    for(int i = 0; i < n; i++){
        for(int j = i+1; j < n; j++){
            if(j != i){
                distance = pointGetDistance(points[i], points[j]);

                if(distance < points[i]->radius + points[j]->radius + spread){
                    if(distance < 1){
                        float xDir, yDir;
                        srand(time(NULL) + i + j);
                        xDir = (((float)rand()) / RAND_MAX) * 2 - 1;
                        srand(time(NULL) + i + j + 1);
                        yDir = (((float)rand()) / RAND_MAX) * 2 - 1;
                        srand(time(NULL) + i + j + 2);
                        spreadAmount = (((float)rand()) / RAND_MAX) * spread;
                        newDistance = spreadAmount + points[i]->radius + points[j]->radius;
                        srand(time(NULL) + i + j + 3);
                        if(!points[i]->locked){
                            points[i]->x = (newDistance) * (xDir) + points[j]->x;
                            points[i]->y = (newDistance) * (yDir) + points[j]->y ;
                        }else if(!points[j]->locked){
                            points[j]->x = (newDistance) * (xDir) + points[i]->x;
                            points[j]->y = (newDistance) * (yDir) + points[i]->y;
                        }
                    }else{
                        srand(time(NULL) + i + j);
                        spreadAmount = spread;
                        newDistance = spreadAmount + points[i]->radius + points[j]->radius;

                        if(!points[i]->locked){
                            points[i]->x = (newDistance/distance) * (points[i]->x - points[j]->x) + points[j]->x;
                            points[i]->y = (newDistance/distance) * (points[i]->y - points[j]->y) + points[j]->y ;
                        }else if(!points[j]->locked){
                            points[j]->x = (newDistance/distance) * (points[j]->x - points[i]->x) + points[i]->x;
                            points[j]->y = (newDistance/distance) * (points[j]->y - points[i]->y) + points[i]->y;
                        }
                    }
                    
                    distance = pointGetDistance(points[i], points[j]) - points[i]->radius - points[j]->radius;

                    if(minDistance == 0) minDistance = distance;
                    else if(minDistance > distance){
                        minDisIndex = j;
                        minDistance = distance;
                    }
                }

                if(minDistance > spread){
                    distance = minDistance + points[i]->radius + points[minDisIndex]->radius;
                    newDistance = minDistance * 0.1 + points[i]->radius + points[minDisIndex]->radius;

                    if(!points[i]->locked){
                        points[i]->x = (newDistance/distance) * (points[i]->x - points[minDisIndex]->x) + points[minDisIndex]->x;
                        points[i]->y = (newDistance/distance) * (points[i]->y - points[minDisIndex]->y) + points[minDisIndex]->y ;
                    }else if(!points[minDisIndex]->locked){
                        points[minDisIndex]->x = (newDistance/distance) * (points[minDisIndex]->x - points[i]->x) + points[i]->x;
                        points[minDisIndex]->y = (newDistance/distance) * (points[minDisIndex]->y - points[i]->y) + points[i]->y;
                    }
                }
            }
        
        }
    }
}

float pointGetDistance(Point* p1, Point* p2){
    return hypotf(p1->x - p2->x, p1->y- p2->y);
}

void pointRender(Point* p){
    for(int i = (int)(-p->radius) + 1; i < (int)p->radius;i++){
        if(((scrH / 2 + p->y + i) < 0) || ((scrH / 2 + p->y + i) > scrH)) continue;
        for(int j = (int)(-p->radius) + 1; j < (int)p->radius;j++){
            if(((scrW / 2 + p->x + j) < 0) || ((scrW / 2 + p->x + j) > scrW)) continue;
            else if(hypot(j, i) <= p->radius){
                mvaddch(scrH / 2 + p->y + i,scrW / 2 + p->x + j, '.');
            }
        }
    }
}