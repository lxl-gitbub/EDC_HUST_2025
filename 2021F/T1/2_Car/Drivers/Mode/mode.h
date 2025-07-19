#ifndef MODE_H
#define MODE_H

#include "kinematics.h"

// Define the modes for the car
typedef enum {
    PROPEL_MODE,  // Propeller mode ǰ��ģʽ
    SLEEP_MODE,   // Sleep mode ˯��ģʽ Ҳ���Ǹտ�ʼ�������κβ���
    WAIT_MODE,    // Wait mode �ȴ�ģʽ
} DRUG;

typedef struct {
    int n;
    DIR trace[10];
}LOC;


typedef struct{
    DRUG drug;  // Current mode of the car
    LOC loc;    // Current location of the car
    DIR dir;    // The Next direction of the car
}MODE;




#ifdef __cplusplus
extern "C" {
#endif

bool drugSet(MODE* mode);

bool isEndOfWay(LOC l);
// This function checks if the current location is the end of the way
DIR ForToBack(DIR dir);
// This function converts the forward direction to the backward direction
DIR DirGet(MODE* mode);
// This function returns the next direction based on the current mode and location
float DirToTheta(DIR dir);
// This function converts the direction to an angle
float LocToTheta(LOC loc);
// This function converts the location to an angle

void cpLoc(LOC* loc, LOC* loc2);
// This function copies the location data from loc to loc2
LOC BStringToLoc(char* str, int len);

#endif