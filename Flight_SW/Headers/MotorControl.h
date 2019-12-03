/*
 * PWM.h
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#ifndef PWM_H_
#define PWM_H_

#define GPIO_MUX 1<<8
#define MOTOR_FWD_SHIFT 0
#define MOTOR_FWD_MASK 1<<0
#define MOTOR_BWD_SHIFT 1
#define MOTOR_BWD_MASK 1<<1
#define PWM_PRESCALER 1 // divde by 2 ^ this value
#define TPM0_CLK_FREQ 2000000
#define PWM_FREQ 50000

void Init_MotorControl();
void Motor_FWD (uint8_t drivepercent);
void Motor_BWD (uint8_t drivepercent);
void Motor_Stop();

#endif /* PWM_H_ */
