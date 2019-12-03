/*
 * MPU6500.h
 *
 *  Created on: Feb 12, 2015
 *      Author: Alok Deshpande
 */

#ifndef MPU6500_H_
#define MPU6500_H_

typedef struct
{
	int16_t AccelX;
	int16_t AccelY;
	int16_t AccelZ;
} MPU6500_Accel;

typedef struct
{
	int16_t GyroX;
	int16_t GyroY;
	int16_t GyroZ;
} MPU6500_Gyro;

/*	To read the accelerometer data, create a Accel structure and then call MPU_Get_Accel and the sturcture will be filled with the accelerometer data
 * 	Same can be applied to the gyro data, instead with a gyro structure and MPU_Get_Gyro
 * 	
 * 	MPU_Select_ByPass is called once, it sets the MPU in bypass mode to allow for retrieval of magnetometer data
 * 	
 * 	adjusting functions are used IF the data needs to be adjusted to 2G scale for accel and 250dps scale for gyro
 * 
 */

void MPU_Select_ByPass();
MPU6500_Accel MPU_Get_Accel();
MPU6500_Gyro MPU_Get_Gyro();
MPU6500_Accel MPU_Adjust_Accel(MPU6500_Accel AccelData);
MPU6500_Gyro MPU_Adjust_Gyro(MPU6500_Gyro GyroData);

/*
void MPU_GyroConfig();
void MPU_AccelConfig();
void MPU_Clear();
void MPU_Reset();
void MPU_EnsureEnable();
MPU6500_Accel MPU_Get_Accel_OLD();
MPU6500_Gyro MPU_Get_Gyro_OLD();
*/

#endif /* MPU6500_H_ */
