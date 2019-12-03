/*
 * AK8963.h
 *
 *  Created on: Feb 12, 2015
 *      Author: Alok Deshpande
 */

#ifndef AK8963_H_
#define AK8963_H_

typedef struct
{
	int16_t HX;
	int16_t HY;
	int16_t HZ;
	uint8_t Success;
} AK8963_Data;

typedef struct
{
	int8_t ASAX;
	int8_t ASAY;
	int8_t ASAZ;
} AK8963_FuseRom;

/*	In order to read the magnetometer data
 * 	MPU MUST be set to bypass mode (check MPU6500.h for details)
 * 	
 * 	*
 * 	GetMagnoCalibrationData must be called and the calibration data must be saved
 * 	SetContinuous mode must be set for continuous mode to operate
 * 	* SetContinuous must be called after if GetMagnoCalibrationData, Get_MagnetometerSingle, or Self_TestData are called. If they are called, they stop the 
 * 		continuous mode reading, therefore the device must be set to that after
 * 	
 * 	Get_Magnetometer is called with the calibration data for the actual adjusted magnetometer data
 * 
 */

void Magno_SetContinuous();
AK8963_Data Get_Magnetometer(AK8963_FuseRom CalibrationData);
AK8963_FuseRom GetMagnoCalibrationData();
AK8963_Data AdjustMagnoData(AK8963_Data MagnoData, AK8963_FuseRom CalibrationData);

/*AK8963_Data Get_MagnetometerSingle(AK8963_FuseRom CalibrationData);
AK8963_Data Self_TestData();
AK8963_Data Get_Magnetometer_OLD(AK8963_FuseRom CalibrationData);
AK8963_FuseRom GetMagnoCalibrationData_OLD();
*/


#endif /* AK8963_H_ */
