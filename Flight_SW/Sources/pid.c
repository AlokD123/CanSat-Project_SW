/*
 * PID.c
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#include "derivative.h"
#include "PID.h"
#include "AK8963.h"
#include "math.h"

int Calculate_Error_Mag (AK8963_Data sensordata) // To be implemented**** (Erik)
{
	// Compute current direction vector using the equation:
	// theta = arctan ( Y / X )
	/*
	Y                                                                                                                              
	^                                                            X   
	|                                                      XXXXXX    
	|                                                  XXXXX         
	|                                            XXXXXX              
	|                                        XXXXX                   
	|                                  XXXXXXX                       
	|                            XXXXXXX                             
	|                     XXXXXXXX                                   
	|              XXXXXXXXX                                         
	|       XXXXXXXX       XX  theta                                 
	| XXXXXXX                X                                       
	XX-----------------------X----------------------------------->  X
	*/
	double theta = atan2((double)sensordata.HY, (double)sensordata.HX); // Signed angle in radians (-PI, PI)
	theta = theta * (180.0/(atan(1)*4.0)); // Convert to signed degrees (-180, 180) (uses ghetto pi)
	return (int)theta; // Return signed angle from X axis (corresponds to setpoint of aligning X axis)
}

int Calculate_Correction (int errorvalue, int* last_i_term, int* last_d_term, double KP, double KI, double KD)
{
	int P_Term;	// Holds the calculated Proportional value
	int I_Term;	// Holds the calculated Integral value
	int D_Term;	// Holds the calculated Differential value
	
	// This calculates Proportional value, Kp is multiplied with errValue and the result is assigned to P_Term
	P_Term = KP * errorvalue;
	
	// Prepare Integral value, add the current error value to the integral value and assign the total to i_Temp
	*last_i_term = *last_i_term + errorvalue;
	
	// Prevents integral wind-up, limits i_Temp from getting too positive or negative
	if (*last_i_term > IMAX)
		*last_i_term = IMAX;
	else if (*last_i_term < IMIN)
		*last_i_term = IMIN;
	
	// Calculates the Integral value, Ki is multiplied with i_Temp and the result is assigned to I_Term
	I_Term = KI * (*last_i_term);
	
	// Calculates Differential value, Kd is multiplied with (d_Temp minus newMagValue) and the result is assigned to D_Term
	// The newMagValue will become the old ADC value on the next function call, this is assigned to d_Temp so it can be used
	D_Term = KD * ((*last_d_term) - errorvalue);
	*last_d_term = errorvalue;
	
	// We now have all the terms, return the correction to be made
	return (P_Term + I_Term + D_Term);
}
