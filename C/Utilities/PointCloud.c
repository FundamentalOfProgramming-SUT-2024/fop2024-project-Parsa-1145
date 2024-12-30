#include "PointCloud.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

int iteratePointCloud(Point** points, int n, int spread){
    float distance, diffrence, desiredDistance;
    float xDir, yDir;
    float force = 0;

    int flag = 1;
    for(int i = 0; i < n; i++){
        points[i]->xForce = 0;
        points[i]->yForce = 0;
        if((!points[i]->locked)){
            for(int j = 0; j < n; j++){
                if(i != j){
                    distance = pointGetDistance(points[i], points[j]);

                    srand(time(NULL) + i + j);
                    desiredDistance = points[i]->radius + points[j]->radius + spread;
                    diffrence = desiredDistance - distance;

                    if(distance < 1.5){
                        srand(time(NULL) + i + j + 1);
                        xDir = (((float)rand()) / RAND_MAX) * 2 - 1;
                        srand(time(NULL) + i + j + 2);
                        yDir = (((float)rand()) / RAND_MAX) * 2 - 1;
                        
                    }else{
                        xDir = (points[i]->x - points[j]->x) / distance;
                        yDir = (points[i]->y - points[j]->y) / distance;
                    }

                    force = pointForceFunction(diffrence, desiredDistance/2) * diffrence;
                    
                    if(diffrence > 0){
                        points[i]->xForce += force * xDir;
                        points[i]->yForce += force * yDir;
                    }

                    if(diffrence > 4){
                        flag = 0;
                    }
                }
            }
        }
    }
    if(flag)return 1;
    else{
        FOR(i, n){
            points[i]->x += points[i]->xForce / n;
            points[i]->y += points[i]->yForce / n;
        }
        return 0;
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
float pointForceFunction(float d, float thresh){
    if(d > thresh) return thresh/2;
    return (d) * (d)/ (powf(thresh, 2));
}