#ifndef _IRTRACKING_H_
#define _IRTRACKING_H_

#include "AllHeader.h"

void car_irtrack(void); //≤ª”√PID—≤œﬂ


void lineWalking(void);
void APP_IRR_PID_Init(void);

bool Road_detect(int, int);
bool half_Detect();
bool cross_Roads_Detect();

#endif
