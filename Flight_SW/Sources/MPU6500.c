/*
 * MPU6500.c
 *
 *  Created on: Feb 12, 2015
 *      Author: Alok Deshpande
 */

#include "derivative.h"
#include "I2C.h"
#include "stdint.h"
#include "math.h"
#include "MPU6500.h"

#define MPUAddress 0x69 //0x69 On the new boards, 0x68 on the old board
#define MPUModeRegister 0x37
#define MPUByPassSelect 0x02

#define Accel_Xout_H 0x3B
#define Accel_Xout_L 0x3C
#define Accel_Yout_H 0x3D
#define Accel_Yout_L 0x3E
#define Accel_Zout_H 0x3F
#define Accel_Zout_L 0x40

#define Gyro_Xout_H 0x43
#define Gyro_Xout_L 0x44
#define Gyro_Yout_H 0x45
#define Gyro_Yout_L 0x46
#define Gyro_Zout_H 0x47
#define Gyro_Zout_L 0x48

// this function sets the accelerometer in bypass mode to be able to retrieve magnetometer data
void MPU_Select_ByPass(){
	I2C0_WriteRegister(MPUAddress,MPUModeRegister,MPUByPassSelect);
}

// this function retrieves the accelerometer data from the MPU sensor and returns it in a structure
MPU6500_Accel MPU_Get_Accel(){
	// creating a MPU structure that will hold the values
	MPU6500_Accel results;
	
	// reading the accelerometer data (x,y,z) from the sensor
	uint8_t AccelData[6];
	I2C0_MultiByteRead(MPUAddress,Accel_Xout_H, AccelData, 6);
	
	// temporary variable to hold the values
	uint16_t temporary=0;
	
	// Concatenating the two bytes for x and saving them in the structure
	temporary |= AccelData[0];
	temporary <<= 8;
	temporary |= AccelData[1];
	results.AccelX = (int16_t)temporary;
	
	// Concatenating the two bytes for y and saving them in the structure
	temporary = 0;
	temporary |= AccelData[2];
	temporary <<= 8;
	temporary |= AccelData[3];	
	results.AccelY = (int16_t)temporary;
	
	// Concatenating the two bytes for y and saving them in the structure
	temporary = 0;
	temporary |= AccelData[4];
	temporary <<= 8;
	temporary |= AccelData[5];	
	results.AccelZ = (int16_t)temporary;
	
	// returning the structure
	return results;
}

// this function retrieves the gyro data from the MPU sensor and returns it in a structure
MPU6500_Gyro MPU_Get_Gyro(){
	// creating a MPU structure that will hold the values
	MPU6500_Gyro results;
	
	// reading the gyro data (x,y,z) from the sensor
	uint8_t GyroData[6];
	I2C0_MultiByteRead(MPUAddress,Gyro_Xout_H, GyroData, 6);
	
	uint16_t temporary=0;
	
	// Concatenating the two bytes for x and saving them in the structure
	temporary |= GyroData[0];
	temporary <<= 8;
	temporary |= GyroData[1];
	results.GyroX = (int16_t)temporary;
	
	// Concatenating the two bytes for y and saving them in the structure
	temporary = 0;
	temporary |= GyroData[2];
	temporary <<= 8;
	temporary |= GyroData[3];	
	results.GyroY = (int16_t)temporary;
	
	// Concatenating the two bytes for y and saving them in the structure
	temporary = 0;
	temporary |= GyroData[4];
	temporary <<= 8;
	temporary |= GyroData[5];	
	results.GyroZ = (int16_t)temporary;
	
	return results;

}

/*
// this function adjusts the accelerometer data and returns it in the 2G form
MPU6500_Accel MPU_Adjust_Accel(MPU6500_Accel AccelData){
	// adjusting values for 2G setting
	float InMax = 32767;
	float InMin = (-32768);
	float OutMax = 2;
	float OutMin = (-2);
	MPU6500_Accel results;
	
	results.AccelX = (int16_t)((OutMax+(OutMin*(InMax-(float)AccelData.AccelX)/((float)AccelData.AccelX-InMin)))/(((InMax-(float)AccelData.AccelX)/((float)AccelData.AccelX-InMin))+1));
	results.AccelY = (int16_t)((OutMax+(OutMin*(InMax-(float)AccelData.AccelY)/((float)AccelData.AccelY-InMin)))/(((InMax-(float)AccelData.AccelY)/((float)AccelData.AccelY-InMin))+1));
	results.AccelZ = (int16_t)((OutMax+(OutMin*(InMax-(float)AccelData.AccelZ)/((float)AccelData.AccelZ-InMin)))/(((InMax-(float)AccelData.AccelZ)/((float)AccelData.AccelZ-InMin))+1));	
	return results;
}

// this function adjusts the gyro data and returns it in the 250dps form
MPU6500_Gyro MPU_Adjust_Gyro(MPU6500_Gyro GyroData){
	// adjusting values for 2G setting
	float InMax = 32767;
	float InMin = (-32768);
	float OutMax = 2;
	float OutMin = (-2);
	MPU6500_Gyro results;
	
	results.GyroX = (int16_t)((OutMax+(OutMin*(InMax-(float)GyroData.GyroX)/((float)GyroData.GyroX-InMin)))/(((InMax-(float)GyroData.GyroX)/((float)GyroData.GyroX-InMin))+1));
	results.GyroY = (int16_t)((OutMax+(OutMin*(InMax-(float)GyroData.GyroY)/((float)GyroData.GyroY-InMin)))/(((InMax-(float)GyroData.GyroY)/((float)GyroData.GyroY-InMin))+1));
	results.GyroZ = (int16_t)((OutMax+(OutMin*(InMax-(float)GyroData.GyroZ)/((float)GyroData.GyroZ-InMin)))/(((InMax-(float)GyroData.GyroZ)/((float)GyroData.GyroZ-InMin))+1));	
	return results;
}



void MPU_GyroConfig(){
	I2C0_WriteRegister(MPUAddress,0x1B,0x02);
}

void MPU_AccelConfig(){
	I2C0_WriteRegister(MPUAddress,0x1D,0x08);
}

void MPU_Clear(){
	I2C0_WriteRegister(MPUAddress,0x6A,0x01);
}

void MPU_Reset(){
	//0x88 To disable temperature
	I2C0_WriteRegister(MPUAddress,0x6B,0x88);
}

void MPU_EnsureEnable(){
	I2C0_WriteRegister(MPUAddress,0x6C,0x00);
}

MPU6500_Accel MPU_Get_Accel_OLD(){
	float InMax = 32767;
	float InMin = (-32768);
	float OutMax = 2;
	float OutMin = (-2);
	
	MPU6500_Accel results;
	uint8_t AccelXH= I2C0_ReadRegister(MPUAddress,Accel_Xout_H);
	uint8_t AccelXL= I2C0_ReadRegister(MPUAddress,Accel_Xout_L);
	uint8_t AccelYH= I2C0_ReadRegister(MPUAddress,Accel_Yout_H);
	uint8_t AccelYL= I2C0_ReadRegister(MPUAddress,Accel_Yout_L);
	uint8_t AccelZH= I2C0_ReadRegister(MPUAddress,Accel_Zout_H);
	uint8_t AccelZL= I2C0_ReadRegister(MPUAddress,Accel_Zout_L);
	
	uint16_t temporary=0;
	int16_t X,Y,Z;
	
	temporary |= AccelXH;
	temporary <<= 8;
	temporary |= AccelXL;
	X = (int16_t)temporary;
	
	temporary = 0;
	temporary |= AccelYH;
	temporary <<= 8;
	temporary |= AccelYL;	
	Y = (int16_t)temporary;
	
	temporary = 0;
	temporary |= AccelZH;
	temporary <<= 8;
	temporary |= AccelZL;	
	Z = (int16_t)temporary;
	
	results.AccelX = (float)((OutMax+(OutMin*(InMax-(float)X)/((float)X-InMin)))/(((InMax-(float)X)/((float)X-InMin))+1));
	results.AccelY = (float)((OutMax+(OutMin*(InMax-(float)Y)/((float)Y-InMin)))/(((InMax-(float)Y)/((float)Y-InMin))+1));
	results.AccelZ = (float)((OutMax+(OutMin*(InMax-(float)Z)/((float)Z-InMin)))/(((InMax-(float)Z)/((float)Z-InMin))+1));
	
	return results;
}

MPU6500_Gyro MPU_Get_Gyro_OLD(){
	float InMax = 32767;
	float InMin = (-32768);
	float OutMax = 250;
	float OutMin = (-250);
		
	MPU6500_Gyro results;
	uint8_t GyroXH= I2C0_ReadRegister(MPUAddress,Gyro_Xout_H);
	uint8_t GyroXL= I2C0_ReadRegister(MPUAddress,Gyro_Xout_L);
	uint8_t GyroYH= I2C0_ReadRegister(MPUAddress,Gyro_Yout_H);
	uint8_t GyroYL= I2C0_ReadRegister(MPUAddress,Gyro_Yout_L);
	uint8_t GyroZH= I2C0_ReadRegister(MPUAddress,Gyro_Zout_H);
	uint8_t GyroZL= I2C0_ReadRegister(MPUAddress,Gyro_Zout_L);
	
	uint16_t temporary=0;
	int16_t X,Y,Z;
	
	temporary |= GyroXH;
	temporary <<= 8;
	temporary |= GyroXL;
	X = (int16_t)temporary;
	
	temporary = 0;
	temporary |= GyroYH;
	temporary <<= 8;
	temporary |= GyroYL;	
	Y = (int16_t)temporary;
	
	temporary = 0;
	temporary |= GyroZH;
	temporary <<= 8;
	temporary |= GyroZL;	
	Z = (int16_t)temporary;
	
	results.GyroX = (float)((OutMax+(OutMin*(InMax-(float)X)/((float)X-InMin)))/(((InMax-(float)X)/((float)X-InMin))+1));
	results.GyroY = (float)((OutMax+(OutMin*(InMax-(float)Y)/((float)Y-InMin)))/(((InMax-(float)Y)/((float)Y-InMin))+1));
	results.GyroZ = (float)((OutMax+(OutMin*(InMax-(float)Z)/((float)Z-InMin)))/(((InMax-(float)Z)/((float)Z-InMin))+1));
	
	return results;

}
*/
