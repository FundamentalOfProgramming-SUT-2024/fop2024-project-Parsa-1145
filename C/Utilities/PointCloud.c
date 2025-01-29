#include "PointCloud.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

int iteratePointCloud(Point** points, int** adjMat, int n, int spread){
    float distance, diffrence, desiredDistance;
    float xDir, yDir;
    float force = 0;
    float xSum = 0, ySum = 0;

    int flag = 1;
    for(int i = 0; i < n; i++){
        points[i]->xForce = 0;
        points[i]->yForce = 0;
        xSum += points[i]->x;
        ySum += points[i]->y;
        if((!points[i]->locked)){
            for(int j = 0; j < n; j++){//you could shuffle the list, then iterate half of the cloud. this should solve the lock-to-infinite problem
                if(i != j){
                    distance = pointGetDistance(points[i], points[j]);

                    srand(time(NULL) + i + j);
                    desiredDistance = points[i]->radius + points[j]->radius + points[i]->spread + points[j]->spread;
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
                    if(diffrence > 0){
                        points[i]->xForce += diffrence * xDir / 2;
                        points[i]->yForce += diffrence * yDir / 2;
                    }else{
                        if(adjMat[i][j]){
                            points[i]->xForce +=  diffrence * xDir / 2 ;
                            points[i]->yForce +=  diffrence * yDir / 2;
                        }
                    }

                    if(distance < points[i]->radius + points[j]->radius){
                        flag = 0;
                    }
                }
            }
        }
        xSum /= n;
        ySum /= n;

        xDir = xSum - points[i]->x;
        yDir = ySum - points[i]->y;

        diffrence = hypot(xDir, yDir);

        if(diffrence > 2){
            points[i]->xForce +=  xDir / diffrence / 2; 
            points[i]->yForce +=  yDir / diffrence / 2;
        }
        

    }
    FOR(i, n){
        points[i]->x += points[i]->xForce / n;
        points[i]->y += points[i]->yForce / n;
    }
    return flag;
    
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

void deletePoint(Point* point){
    free(point);
}
