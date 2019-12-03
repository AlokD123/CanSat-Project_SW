/*
 * Scheduling.h
 *
 *  Created on: Dec 14, 2014
 *      Author: Erik Johnson
 *  
 *  This file is the header file for scheduling the time critical events.
 */

#ifndef SCHEDULING_H_
#define SCHEDULING_H_

#define TPM_CLK_KHZ 4000 // MCGIRCLK in kHz (as setup in main->BasicInit)

//void Init_TPM0(int); // Obsolete
void Init_TPM1(int);
//void Init_Scheduling(int, int); // Obsolete
void Init_Servicing(int);

#endif /* SCHEDULING_H_ */
