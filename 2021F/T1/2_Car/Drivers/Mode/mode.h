#ifndef MODE_H
#define MODE_H

#include "kinematics.h"

// Define the modes for the car
typedef enum {
    PROPEL_MODE,  // Propeller mode ��ҩģʽ
    RETURN_MODE,  // Return mode �ع�ģʽ
    WAIT_MODE,    // Wait mode �ȴ�ģʽ
} DRUG;

typedef enum {
    ZERO,
    L1,
    R1,
    ONE,
    L2,
    R2,
    TWO,
    L3,
    R3,
    L3_L,
    L3_R,
    R3_L,
    R3_R,
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
DIR DirGet(MODE* mode);
// This function returns the next direction based on the current mode and location

#endif