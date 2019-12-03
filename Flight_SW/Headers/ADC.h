/*
 * ADC.h
 *
 *  Created on: May 13, 2015
 *      Author: Erik Johnson
 */

#ifndef ADC_H_
#define ADC_H_

#include "derivative.h"

/* Prototypes */
void adc_init(void);
int adc_cal(void);
unsigned short adc_read(unsigned char ch);
unsigned short ReadBatteryVoltage();

#endif /* ADC_H_ */
