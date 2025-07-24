/*
 * SensorProc.h
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Header file for sensor processing functions
 */
#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include "AllHeader.h"

bool isInTheYaw(float targetYaw, float tolerance); // Check if the current yaw is within a specified tolerance of the target yaw

bool Road_detect(int nummin, int nummax);
bool half_Detect();
bool cross_Roads_Detect();

#endif // SENSOR_PROC_H