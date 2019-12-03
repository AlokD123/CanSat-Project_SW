/*
 * GPIO.h
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_MUX 1<<8

#define LED1_E 1<<24
#define LED2_E 1<<25
#define LED3_A 1<<1
#define LED4_A 1<<2
#define LED5_A 1<<4
#define LED6_A 1<<5
#define LED7_A 1<<12
#define LED8_A 1<<13

#define JP4 1<<2
#define JP5 1<<3
#define JP6 1<<4

#define BUZZER_PIN_MASK 1<<0
#define RM1_PIN_MASK 1<<10
#define RM2_PIN_MASK 1<<11

void Init_LEDs();
void LightAll();
void DarkenAll();
void Light(int led);
void Darken(int led);

void Init_Jumpers();
int ReadJumper(int jumper);
int* ReadAllJumpers();

void Init_GPIO();
void ActivateLocationAssistanceDevice();
void ReleaseFromContainer();
void ReleaseBlades();
void StopReleaseFromContainer();
void StopReleaseBlades();

#endif /* GPIO_H_ */
