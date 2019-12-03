/*
 * PID.h
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#ifndef PID_H_
#define PID_H_

#include "AK8963.h"

/* Limits for integral factor */
#define IMAX 100 // Prevents integral over compensation
#define IMIN -100 // Prevents integral over compensation

int Calculate_Error_Mag (AK8963_Data sensordata);
int Calculate_Correction (int errorval, int* last_i_term, int* last_d_term, double KP, double KI, double KD);

#endif /* PID_H_ */
