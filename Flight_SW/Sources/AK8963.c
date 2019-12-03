/*
 * AK8963.c
 *
 *  Created on: Feb 12, 2015
 *      Author: Alok Deshpande
 */


#include "derivative.h"
#include "I2C.h"
#include "stdint.h"
#include "math.h"
#include "AK8963.h"
#include "delay.h"

// Page 15 is important

#define MagAddress 0x0F
#define ST1 0x02
#define ST2 0x09
#define CNTL1 0x0A
#define CNTL2 0x0B

#define HXL 0x03
#define HXH 0x04
#define HYL 0x05
#define HYH 0x06
#define HZL 0x07
#define HZH 0x08

#define ASTC 0x0C

#define ASAXX 0x10
#define ASAYY 0x11
#define ASAZZ 0x12

// Sets the magnetometer in continuous mode (IF self test or single reading, or calibrationdata functions are called at any point, this functions MUST be recalled before continuous is set again)
void Magno_SetContinuous(){
	I2C0_WriteRegister(MagAddress,CNTL1,0x16);
}

// This functions retrieves the magnetometer data (adjusted data) (ASSUMING magnetometer is in continuous mode). Adjusting data must be passed on to function
// Adjusting data should be read earlier 
AK8963_Data Get_Magnetometer(AK8963_FuseRom CalibrationData){
	// structure to hold the data
	AK8963_Data results;
	results.Success = 0;
	uint16_t i=0;
	// checking if data is ready
	int8_t Status = I2C0_ReadRegister(MagAddress,ST1);
	while(!(Status & 0x01)){
			Status = I2C0_ReadRegister(MagAddress,ST1);
			if(i==0xFFFF){
				break;
			}
			i++;
	}
	// data is ready
	if(Status & 0x01){
		// asking for the magnetometer data
		uint8_t Data[6];
		I2C0_MultiByteRead(MagAddress, HXL, Data, 6);
		
		// saving the data in the structure (x,y,z)
		uint16_t temp;
		temp = Data[1];
		temp <<= 8;
		temp |= Data[0];
		results.HX = (int16_t)temp;
		
		temp=0;
		temp = Data[3];
		temp <<= 8;
		temp |= Data[2];
		results.HY = (int16_t)temp;
		
		temp=0;
		temp = Data[5];
		temp <<= 8;
		temp |= Data[4];
		results.HZ = (int16_t)temp;
		
		// letting the user know the retreival is successful
		results.Success = 0xFF;
	}else{
		// retrieval is not successful, WILL NEVER GET HERE IF THE WHILE LOOP IS KEPT
		results.HX = 0x00;
		results.HY = 0x00;
		results.HZ = 0x00;
		results.Success = 0x00;
	}
	
	// letting the magnetometer know the data reading is done
	//int8_t Required = I2C0_ReadRegister(MagAddress,ST2);
	I2C0_ReadRegister(MagAddress,ST2);
	
	// adjusting the data with the calibration data
	results = AdjustMagnoData(results,CalibrationData);
	
	// returning the adjusted data
	return results;
}

// this fucntion adjusts the magetometer data that is passed in with the calibration data read earlier and passed in as well
AK8963_Data AdjustMagnoData(AK8963_Data MagnoData, AK8963_FuseRom CalibrationData){
	AK8963_Data results = MagnoData;
	
	results.HX = (int16_t)results.HX*((((float)(CalibrationData.ASAX-128))/(float)256)+1);
	results.HY = (int16_t)results.HY*((((float)(CalibrationData.ASAY-128))/(float)256)+1);
	results.HZ = (int16_t)results.HZ*((((float)(CalibrationData.ASAZ-128))/(float)256)+1);
	
	return results;
}

// Make sure if you call this function that you reset the mode to continuous after you are done with this function. 
AK8963_FuseRom GetMagnoCalibrationData(){
	// structure to hold the calibration data
	AK8963_FuseRom results;
	
	// self test mode set
	I2C0_WriteRegister(MagAddress,CNTL1,0x1F);
	// getting the calibration data
	uint8_t Data[3];
	I2C0_MultiByteRead(MagAddress, ASAXX, Data, 3);
	
	// saving the data in the structure
	results.ASAX = (int8_t)Data[0];
	results.ASAY = (int8_t)Data[1];
	results.ASAZ = (int8_t)Data[2];
	
	// going back to standby mode REQUIRED
	I2C0_WriteRegister(MagAddress,CNTL1,0x10);
	// returning the calibration data
	return results;
}

/*
// this fucntion sets single mode, waits for the data to be calculated, then retrieves and does the same as the other magnetometer get function
AK8963_Data Get_MagnetometerSingle(AK8963_FuseRom CalibrationData){
	AK8963_Data results;
	// single measurement mode set
	I2C0_WriteRegister(MagAddress,CNTL1,0x11);
	delay(9);
	// see if data is ready
	int8_t Status = I2C0_ReadRegister(MagAddress,ST1);
	if(Status & 0x01){
		// get the data and save it in structure
		uint8_t Data[6];
		I2C0_MultiByteRead(MagAddress, HXL, Data, 6);
		
		results.HX = Data[1];
		results.HX <<= 8;
		results.HX |= Data[0];
		
		results.HY = Data[3];
		results.HY <<= 8;
		results.HY |= Data[2];
		
		results.HZ = Data[5];
		results.HZ <<= 8;
		results.HZ |= Data[4];
		
		results.Success = 0xFF;
	}else{
		// data reading is not successful
		results.HX = 0x00;
		results.HY = 0x00;
		results.HZ = 0x00;
		results.Success = 0x00;
	}
	
	// letting the magnetometer know that the reading is done
	//int8_t Required = I2C0_ReadRegister(MagAddress,ST2);
	I2C0_ReadRegister(MagAddress,ST2);
	
	// adjusting the data
	results = AdjustMagnoData(results,CalibrationData);

	// returning the adjusted data
	return results;
}

// this function puts the magnetometer in selftest mode, then reads the selftest data and returns the raw data
AK8963_Data Self_TestData(){
	// data structure to hold the data
	AK8963_Data results;
	// setting the self test mode, these steps are required
	I2C0_WriteRegister(MagAddress,ASTC,0x40);
	// self test mode set
	I2C0_WriteRegister(MagAddress,CNTL1,0x18);
	delay(15);
	// seeing if data is ready
	int8_t Status = I2C0_ReadRegister(MagAddress,ST1);
	if(Status & 0x01){
		// getting the self test data
		uint8_t Data[6];
		I2C0_MultiByteRead(MagAddress, HXL, Data, 6);
		
		results.HX = Data[1];
		results.HX <<= 8;
		results.HX |= Data[0];
		
		results.HY = Data[3];
		results.HY <<= 8;
		results.HY |= Data[2];
		
		results.HZ = Data[5];
		results.HZ <<= 8;
		results.HZ |= Data[4];
		
		results.Success = 0xFF;
	}else{
		// data reading is not successful
		results.HX = 0x00;
		results.HY = 0x00;
		results.HZ = 0x00;
		results.Success = 0x00;
	}
	
	// letting the device know that the reading is done
	//int8_t Required = I2C0_ReadRegister(MagAddress,ST2);
	I2C0_ReadRegister(MagAddress,ST2);
	// stopping the self test mode and putting the device back in standby mode
	I2C0_WriteRegister(MagAddress,ASTC,0x00);
	I2C0_WriteRegister(MagAddress,CNTL1,0x10);
	
	// returning the raw data
	return results;
}


AK8963_Data Get_Magnetometer_OLD(AK8963_FuseRom CalibrationData){
	AK8963_Data results;
	int8_t Status = I2C0_ReadRegister(MagAddress,ST1);
	while(!(Status & 0x01)){
			Status = I2C0_ReadRegister(MagAddress,ST1);
	}
	if(Status & 0x01){
		int8_t HXLow = I2C0_ReadRegister(MagAddress,HXL);
		int8_t HXHigh = I2C0_ReadRegister(MagAddress,HXH);
		int8_t HYLow = I2C0_ReadRegister(MagAddress,HYL);
		int8_t HYHigh = I2C0_ReadRegister(MagAddress,HYH);
		int8_t HZLow = I2C0_ReadRegister(MagAddress,HZL);
		int8_t HZHigh = I2C0_ReadRegister(MagAddress,HZH);
		
		results.HX = HXHigh;
		results.HX <<= 8;
		results.HX |= HXLow;
		
		results.HY = HYHigh;
		results.HY <<= 8;
		results.HY |= HYLow;
		
		results.HZ = HZHigh;
		results.HZ <<= 8;
		results.HZ |= HZLow;
		
		results.Success = 0xFF;
	}else{
		results.HX = 0x00;
		results.HY = 0x00;
		results.HZ = 0x00;
		results.Success = 0x00;
	}
	
	//int8_t Required = I2C0_ReadRegister(MagAddress,ST2);
	I2C0_ReadRegister(MagAddress,ST2);
	
	results = AdjustMagnoData(results,CalibrationData);
	
	return results;
}

// Make sure if you call this function that you reset the mode to continuous after you are done with this function. 
AK8963_FuseRom GetMagnoCalibrationData_OLD(){
	AK8963_FuseRom results;
	// self test mode set
	I2C0_WriteRegister(MagAddress,CNTL1,0x1F);
	results.ASAX = I2C0_ReadRegister(MagAddress,ASAXX);
	results.ASAY = I2C0_ReadRegister(MagAddress,ASAYY);
	results.ASAZ = I2C0_ReadRegister(MagAddress,ASAZZ);
	I2C0_WriteRegister(MagAddress,CNTL1,0x10);
	return results;
}
*/
