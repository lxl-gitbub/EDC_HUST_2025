/*
 * SensorProc.c
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Source file for sensor processing functions
 */
#include "SensorProc.h"

bool isInTheYaw(float targetYaw, float tolerance)
{
    // Check if the current yaw is within the specified tolerance of the target yaw
    float currentYaw = getYaw(); // Get the current yaw angle
    float dif = fabs(sumTheta(currentYaw, -targetYaw)); // Calculate the difference between current and target yaw
    // If the absolute difference is less than the tolerance, return true
    if (dif < tolerance || 180 - dif < tolerance) {
        return true; // If within tolerance, return true
    } else {
        return false; // Otherwise, return false
    }
}

bool Road_detect(int nummin, int nummax)
{
		int sumIRDetect = 0;
		int i = 0;
		while(i < 8){	if(Digtal[i]==0) sumIRDetect ++; i++;}
		if(sumIRDetect <= nummax && sumIRDetect >= nummin) 	return 1;
		else return 0;
}
bool half_Detect(){
//三岔路口检测
	return Road_detect(5, 7);
}

bool cross_Roads_Detect(){
//十字路口（丁字路口）检测
	return Road_detect(7, 8);
}