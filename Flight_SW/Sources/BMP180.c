
 /*
 * BMP180.c
 * BMP180 Library For CANSAT's Project
 * Author: Maytham Sabbagh
 * Date: Tuesday, November 25th
 * Based on Freescale's code for the M0+ and the BMP180
 * to work with the CANSAT 2015 Project for Carleton University
 * 
 */
 
#include "derivative.h"
#include "I2C.h"
#include "stdint.h"
#include "math.h"
#include "BMP180.h"
#include "delay.h"

#define oss 1
#define BMPAddress 0x77
#define MainRegisterAddress 0xF4
#define DataMSB 0xF6
#define DataLSB 0xF7
#define DataXLSB 0xF8
#define PressControlData 0x34
#define TempControlData 0x2E
#define BMPCalibrationAddress 0xAA

// Initialization of BMP180_2, this will return a structure of type calibrationData, which contains the signed and unsigned calibration data
calibrationData BMP1802_init(){
	// temporary structure
	calibrationData calibdata;
	
	uint8_t Data[22];
	I2C1_MultiByteRead(BMPAddress, BMPCalibrationAddress, Data, 22);
	
	// a count that only goes up to 10 for the (0-10) 11 calibration data
	uint8_t count = 0;
	uint8_t counter = 0;
	// for loop to get the 11 calibration data
	for (;count < 22;){
		if(counter >= 6){
			// if more than 6, save the data in the signed calibration
			calibdata.calibration[counter-3] = ((Data[count]<<8) + Data[count+1]);
		}else if(counter >= 3){
			// if more than 3 and less than 6, save the data in the unsigned calibration
			calibdata.uncalibration[counter-3] = (Data[count]<<8) + Data[count+1];
		}else{
			// less than 3, save data in the signed calibration 
			calibdata.calibration[counter] = ((Data[count]<<8) + Data[count+1]);
		}
		count += 2;
		counter++;
	}
	
	return calibdata;
}

// Initialization of BMP180_1, this will return a structure of type calibrationData, which contains the signed and unsigned calibration data
calibrationData BMP1801_init(){
	// temporary structure
	calibrationData calibdata;
	
	uint8_t Data[22];
	I2C0_MultiByteRead(BMPAddress, BMPCalibrationAddress, Data, 22);
	
	// a count that only goes up to 10 for the (0-10) 11 calibration data
	uint8_t count = 0;
	uint8_t counter = 0;
	// for loop to get the 11 calibration data
	for (;count < 22;){
		if(counter >= 6){
			// if more than 6, save the data in the signed calibration
			calibdata.calibration[counter-3] = ((Data[count]<<8) + Data[count+1]);
		}else if(counter >= 3){
			// if more than 3 and less than 6, save the data in the unsigned calibration
			calibdata.uncalibration[counter-3] = (Data[count]<<8) + Data[count+1];
		}else{
			// less than 3, save data in the signed calibration 
			calibdata.calibration[counter] = ((Data[count]<<8) + Data[count+1]);
		}
		count += 2;
		counter++;
	}
	
	return calibdata;
}



// this function requires the most recent sensor data from the structure, and the initial pressure. Then it returns the current altitude
int32_t getAltitude(BMP180 sensordata, int32_t initialpressure){
	// altiture calculation 
	int32_t altitude = 44330.0*(1.0-(pow((((float)sensordata.pressure)/((float)initialpressure)),(1.0/5.255))));
	// returning the altitude
	return altitude;
}

// This function needs to be called at the beginning to read the initial pressure at ground. This function uses BMP180_2 which is on I2C0 line.  
int32_t getInitialPressure1(calibrationData calibdata){
	// ask for temperature and receive it
	askTemperature1();
	delay(5);
	int16_t temp = receiveTemperature1();
	// ask for pressure and receive it
	askPressure1();
	delay(5);
	int32_t press = receivePressure1();
	
	// calculate the initial pressure and then return
	BMP180 initialdata = CalculateBMP(calibdata, temp, press);
	return initialdata.pressure;
}

// This function needs to be called at the beginning to read the initial pressure at ground. This function uses BMP180_1 which is on I2C1 line.  
int32_t getInitialPressure2(calibrationData calibdata){
	// ask for temperature and receive it
	askTemperature2();
	delay(5);
	int16_t temp = receiveTemperature2();
	// ask for pressure and receive it
	askPressure2();
	delay(5);
	int32_t press = receivePressure2();
	
	// calculate the initial pressure and then return
	BMP180 initialdata = CalculateBMP(calibdata, temp, press);
	return initialdata.pressure;
}

// this function asks for temperature from BMP180_2, delay of 4.5ms must be there after
void askTemperature1(){
	// Letting the BMP180 know we are looking for the temperature data 
	I2C0_WriteRegister(BMPAddress, MainRegisterAddress, TempControlData);
}

// this function asks for the pressure from BMP180_2, delay of 4.5ms must be there after
void askPressure1(){
	// Setting the pressure collection mode with the oss value (low power 0, normal 1, high resolution 2, ultra high resolution 3)
	uint8_t PressureControlData = PressControlData + (oss<<6);
	
	// Letting the BMP180 know we are looking for the pressure data
	I2C0_WriteRegister(BMPAddress, MainRegisterAddress, PressureControlData);
}

// this function asks for temperature from BMP180_1, delay of 4.5ms must be there after
void askTemperature2(){
	// Letting the BMP180 know we are looking for the temperature data 
	I2C1_WriteRegister(BMPAddress, MainRegisterAddress, TempControlData);
}

// this function asks for the pressure from BMP180_1, delay of 4.5ms must be there after
void askPressure2(){
	// Setting the pressure collection mode with the oss value (low power 0, normal 1, high resolution 2, ultra high resolution 3)
	uint8_t PressureControlData = PressControlData + (oss<<6);
	
	// Letting the BMP180 know we are looking for the pressure data
	I2C1_WriteRegister(BMPAddress, MainRegisterAddress, PressureControlData);
}

// this function receives the temperature from BMP180_2, delay of 4.5ms must be there before
int16_t receiveTemperature1(){
	// Reading the MSB and LSB of the 16-bit temperature value 
	uint8_t Data[2];
	I2C0_MultiByteRead(BMPAddress, DataMSB, Data, 2);

	// combining the MSB and LSB and returning	
	return ((Data[0]<<8) + Data[1]);
}

int32_t receivePressure1(){
	// reading MSB, LSB, and XLSB
	uint8_t Data[3];
	I2C0_MultiByteRead(BMPAddress, DataMSB, Data, 3);
	
	// combining and returning
	return (((Data[0]<<16)+(Data[1]<<8)+Data[2])>>(8-oss));
}

// this function receives the temperature from BMP180_2, delay of 4.5ms must be there before
int16_t receiveTemperature2(){
	// Reading the MSB and LSB of the 16-bit temperature value 
	uint8_t Data[2];
	I2C1_MultiByteRead(BMPAddress, DataMSB, Data, 2);
		
	// combining the MSB and LSB and returning	
	return ((Data[0]<<8) + Data[1]);
}

int32_t receivePressure2(){
	// reading MSB, LSB, and XLSB 
	uint8_t Data[3];
	I2C1_MultiByteRead(BMPAddress, DataMSB, Data, 3);
	
	// combining and returning
	return (((Data[0]<<16)+(Data[1]<<8)+Data[2])>>(8-oss));
}

// this function takes the raw data, does all the calculations required by the manufacturer, and returns a structure of type BMP180 with the calculated pressure and temperature
BMP180 CalculateBMP(calibrationData calibdata, int16_t temp, int32_t press){
	// temporary variable to be used in the calculations
	int32_t result;
	
	// BMP structure to hold the calculated values
	BMP180 results;

	// calculating the temperature and saving in the structure
	int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(float)(1<<15)); //int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(pow(2,15)));
	int32_t X2 = (calibdata.calibration[6]*(float)(1<<11))/(X1+calibdata.calibration[7]); //int32_t X2 = (calibdata.calibration[6]*(pow(2,11)))/(X1+calibdata.calibration[7]);
	int32_t B5 = X1+X2;
	int32_t T = (B5+8)/(float)(1<<4); //int32_t T = (B5+8)/(pow(2,4));
	results.temperature = T;
	
	// calculating the pressure and saving in the structure
	int32_t B6 = B5 - 4000;
	X1 = (calibdata.calibration[4]*((B6*B6)/(float)(1<<12)))/(float)(1<<11); //X1 = (calibdata.calibration[4]*((B6*B6)/(pow(2,12))))/(pow(2,11));
	X2 = (calibdata.calibration[1]*B6)/(float)(1<<11); //X2 = (calibdata.calibration[1]*B6)/(pow(2,11));
	int32_t X3 = X1+X2;
	int32_t B3 = ((((calibdata.calibration[0]*4)+X3)<<oss)+2)/4;
	X1 = (calibdata.calibration[2]*B6)/(float)(1<<13); //X1 = (calibdata.calibration[2]*B6)/(pow(2,13));
	X2 = (calibdata.calibration[3]*(B6*B6/(float)(1<<12)))/(float)(1<<16); //X2 = (calibdata.calibration[3]*(B6*B6/pow(2,12)))/pow(2,16);
	X3 = ((X1+X2)+2)/(float)(1<<2); //X3 = ((X1+X2)+2)/pow(2,2);
	uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/(float)(1<<15)); //uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/pow(2,15));
	uint32_t B7 = ((unsigned long)press - B3)*(50000>>oss);
	if(B7< 0x80000000){
		result = (B7*2)/B4;
	}else{
		result = (B7/B4)*2;
	}
	X1 = (result/(float)(1<<8))*(result/(float)(1<<8)); //X1 = (result/pow(2,8))*(result/pow(2,8));
	X1 = (X1*3038)/(float)(1<<16); //X1 = (X1*3038)/pow(2,16);
	X2 = (-7357*result)/(float)(1<<16); //X2 = (-7357*result)/pow(2,16);
	results.pressure = result+((X1+X2+3791)/(float)(1<<4)); //results.pressure = result+((X1+X2+3791)/pow(2,4));
	
	// returning the structure
	return results;
}


/* These are the old functions after the main changes were done. KEEP THESE FOR REFERENCE



// Initialization of BMP180_2, this will return a structure of type calibrationData, which contains the signed and unsigned calibration data
calibrationData BMP1802_init_OLD(){
	// temporary structure
	calibrationData calibdata;
	
	// temporary variables
	uint8_t temp1;
	uint8_t temp2;
	
	// starting 2 addresses of the calibration data
	uint8_t address1 = 0xAA;
	uint8_t address2 = 0xAB;
	
	// a count that only goes up to 10 for the (0-10) 11 calibration data
	uint8_t count = 0;
	
	// for loop to get the 11 calibration data
	for (;count < 11;count++){
			// getting the lower and upper 8 bits of the calibration data point
			temp1 = I2C1_ReadRegister( BMPAddress, address1);
			temp2 = I2C1_ReadRegister( BMPAddress, address2);
			
			if(count >= 6){
				// if more than 6, save the data in the signed calibration
				calibdata.calibration[count-3] = (temp1<<8) + temp2;
			}else if(count >= 3){
				// if more than 3 and less than 6, save the data in the unsigned calibration
				calibdata.uncalibration[count-3] = (temp1<<8) + temp2;
			}else{
				// less than 3, save data in the signed calibration 
				calibdata.calibration[count] = (temp1<<8) + temp2;
			}
			// get the next 2 addresses 
			address1+=2;
			address2+=2;
		}
		
		return calibdata;
}

// Initialization of BMP180_1, this will return a structure of type calibrationData, which contains the signed and unsigned calibration data
calibrationData BMP1801_init_OLD(){
	// temporary structure
	calibrationData calibdata;
	
	// temporary variables
	uint8_t temp1;
	uint8_t temp2;
	
	// starting 2 addresses of the calibration data
	uint8_t address1 = 0xAA;
	uint8_t address2 = 0xAB;
	
	// a count that only goes up to 10 for the (0-10) 11 calibration data
	uint8_t count = 0;
	
	// for loop to get the 11 calibration data
	for (;count < 11;count++){
		// getting the lower and upper 8 bits of the calibration data point
		temp1 = I2C0_ReadRegister( BMPAddress, address1);
		temp2 = I2C0_ReadRegister( BMPAddress, address2);
		
		if(count >= 6){
			// if more than 6, save the data in the signed calibration
			calibdata.calibration[count-3] = (temp1<<8) + temp2;
		}else if(count >= 3){
			// if more than 3 and less than 6, save the data in the unsigned calibration
			calibdata.uncalibration[count-3] = (temp1<<8) + temp2;
		}else{
			// less than 3, save data in the signed calibration 
			calibdata.calibration[count] = (temp1<<8) + temp2;
		}
		// get the next 2 addresses 
		address1+=2;
		address2+=2;
	}
	
	return calibdata;
}

// this function returns raw temperature data from BMP180_1
int16_t getTemperature2(){
	// Letting the BMP180 know we are looking for the temperature data 
	I2C1_WriteRegister(BMPAddress, MainRegisterAddress, TempControlData);
		
	// Wait for 4.5ms must be generated
	delay(5);
	
	// Reading the MSB and LSB of the 16-bit temperature value
	uint8_t Data[2];
	I2C1_MultiByteRead(BMPAddress, DataMSB, Data, 2);
		
	// combining the MSB and LSB and returning	
	return ((Data[0]<<8) + Data[1]);
}

// this function returns raw temperature data from BMP180_2
int16_t getTemperature1(){
	// Letting the BMP180 know we are looking for the temperature data 
	I2C0_WriteRegister(BMPAddress, MainRegisterAddress, TempControlData);
		
	// Wait for 4.5ms must be generated
	delay(5);
	
	// Reading the MSB and LSB of the 16-bit temperature value 
	uint8_t Data[2];
	I2C1_MultiByteRead(BMPAddress, DataMSB, Data, 2);
		
	// combining the MSB and LSB and returning	
	return ((Data[0]<<8) + Data[1]);
}

// this returns raw 32-bit pressure data from the BMP180_1 
int32_t getPressure2(){
	// Setting the pressure collection mode with the oss value (low power 0, normal 1, high resolution 2, ultra high resolution 3)
	uint8_t PressureControlData = PressControlData + (oss<<6);
	
	// Letting the BMP180 know we are looking for the pressure data
	I2C1_WriteRegister(BMPAddress, MainRegisterAddress, PressureControlData);
	
	// Wait for 4.5ms must be generated
	delay(5);
	
	// reading MSB, LSB, and XLSB
	uint8_t Data[3];
	I2C1_MultiByteRead(BMPAddress, DataMSB, Data, 3);
	
	// combining and returning
	return (((Data[0]<<16)+(Data[1]<<8)+Data[2])>>(8-oss));
}

// this returns raw 32-bit pressure data from the BMP180_2 
int32_t getPressure1(){
	// Setting the pressure collection mode with the oss value (low power 0, normal 1, high resolution 2, ultra high resolution 3)
	uint8_t PressureControlData = PressControlData + (oss<<6);
	
	// Letting the BMP180 know we are looking for the pressure data
	I2C0_WriteRegister(BMPAddress, MainRegisterAddress, PressureControlData);
	
	// Wait for 4.5ms must be generated
	delay(5);
	
	uint8_t Data[3];
	I2C0_MultiByteRead(BMPAddress, DataMSB, Data, 3);
	// reading MSB, LSB, and XLSB 
	
	// combining and returning
	return (((Data[0]<<16)+(Data[1]<<8)+Data[2])>>(8-oss));
}

// this function reads the raw data, does all the calculations required by the manufacturer, and returns a structure of type BMP180 with the calculated pressure and temperature
BMP180 ReadBMP2(calibrationData calibdata){
	// temporary variable to be used in the calculations
	int32_t result;
	
	// BMP structure to hold the calculated values
	BMP180 results;
	
	// reading the temperature values from the sensor
	int16_t temp = getTemperature2();

	// calculating the temperature and saving in the structure
	int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(float)(1<<15)); //int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(pow(2,15)));
	int32_t X2 = (calibdata.calibration[6]*(float)(1<<11))/(X1+calibdata.calibration[7]); //int32_t X2 = (calibdata.calibration[6]*(pow(2,11)))/(X1+calibdata.calibration[7]);
	int32_t B5 = X1+X2;
	int32_t T = (B5+8)/(float)(1<<4); //int32_t T = (B5+8)/(pow(2,4));
	results.temperature = T;
	
	// reading the pressure from the sensor
	int32_t press = getPressure2();

	// calculating the pressure and saving in the structure
	int32_t B6 = B5 - 4000;
	X1 = (calibdata.calibration[4]*((B6*B6)/(float)(1<<12)))/(float)(1<<11); //X1 = (calibdata.calibration[4]*((B6*B6)/(pow(2,12))))/(pow(2,11));
	X2 = (calibdata.calibration[1]*B6)/(float)(1<<11); //X2 = (calibdata.calibration[1]*B6)/(pow(2,11));
	int32_t X3 = X1+X2;
	int32_t B3 = ((((calibdata.calibration[0]*4)+X3)<<oss)+2)/4;
	X1 = (calibdata.calibration[2]*B6)/(float)(1<<13); //X1 = (calibdata.calibration[2]*B6)/(pow(2,13));
	X2 = (calibdata.calibration[3]*(B6*B6/(float)(1<<12)))/(float)(1<<16); //X2 = (calibdata.calibration[3]*(B6*B6/pow(2,12)))/pow(2,16);
	X3 = ((X1+X2)+2)/(float)(1<<2); //X3 = ((X1+X2)+2)/pow(2,2);
	uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/(float)(1<<15)); //uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/pow(2,15));
	uint32_t B7 = ((unsigned long)press - B3)*(50000>>oss);
	if(B7< 0x80000000){
		result = (B7*2)/B4;
	}else{
		result = (B7/B4)*2;
	}
	X1 = (result/(float)(1<<8))*(result/(float)(1<<8)); //X1 = (result/pow(2,8))*(result/pow(2,8));
	X1 = (X1*3038)/(float)(1<<16); //X1 = (X1*3038)/pow(2,16);
	X2 = (-7357*result)/(float)(1<<16); //X2 = (-7357*result)/pow(2,16);
	results.pressure = result+((X1+X2+3791)/pow(2,4));
	
	// returning the structure
	return results;
}

// this function reads the raw data, does all the calculations required by the manufacturer, and returns a structure of type BMP180 with the calculated pressure and temperature
BMP180 ReadBMP1(calibrationData calibdata){
	// temporary variable to be used in the calculations
	int32_t result;
	
	// BMP structure to hold the calculated values
	BMP180 results;
	
	// reading the temperature values from the sensor
	int16_t temp = getTemperature1();

	// calculating the temperature and saving in the structure
	int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(float)(1<<15)); //int32_t X1 = (temp - calibdata.uncalibration[2])*(calibdata.uncalibration[1]/(pow(2,15)));
	int32_t X2 = (calibdata.calibration[6]*(float)(1<<11))/(X1+calibdata.calibration[7]); //int32_t X2 = (calibdata.calibration[6]*(pow(2,11)))/(X1+calibdata.calibration[7]);
	int32_t B5 = X1+X2;
	int32_t T = (B5+8)/(float)(1<<4); //int32_t T = (B5+8)/(pow(2,4));
	results.temperature = T;
	
	// reading the pressure from the sensor
	int32_t press = getPressure1();

	// calculating the pressure and saving in the structure
	int32_t B6 = B5 - 4000;
	X1 = (calibdata.calibration[4]*((B6*B6)/(float)(1<<12)))/(float)(1<<11); //X1 = (calibdata.calibration[4]*((B6*B6)/(pow(2,12))))/(pow(2,11));
	X2 = (calibdata.calibration[1]*B6)/(float)(1<<11); //X2 = (calibdata.calibration[1]*B6)/(pow(2,11));
	int32_t X3 = X1+X2;
	int32_t B3 = ((((calibdata.calibration[0]*4)+X3)<<oss)+2)/4;
	X1 = (calibdata.calibration[2]*B6)/(float)(1<<13); //X1 = (calibdata.calibration[2]*B6)/(pow(2,13));
	X2 = (calibdata.calibration[3]*(B6*B6/(float)(1<<12)))/(float)(1<<16); //X2 = (calibdata.calibration[3]*(B6*B6/pow(2,12)))/pow(2,16);
	X3 = ((X1+X2)+2)/(float)(1<<2); //X3 = ((X1+X2)+2)/pow(2,2);
	uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/(float)(1<<15)); //uint32_t B4 = calibdata.uncalibration[0]*((unsigned long)(X3+32768)/pow(2,15));
	uint32_t B7 = ((unsigned long)press - B3)*(50000>>oss);
	if(B7< 0x80000000){
		result = (B7*2)/B4;
	}else{
		result = (B7/B4)*2;
	}
	X1 = (result/(float)(1<<8))*(result/(float)(1<<8)); //X1 = (result/pow(2,8))*(result/pow(2,8));
	X1 = (X1*3038)/(float)(1<<16); //X1 = (X1*3038)/pow(2,16);
	X2 = (-7357*result)/(float)(1<<16); //X2 = (-7357*result)/pow(2,16);
	results.pressure = result+((X1+X2+3791)/pow(2,4));
	
	// returning the structure
	return results;
}


// this function receives the temperature from BMP180_2, delay of 4.5ms must be there before
int16_t receiveTemperature1_OLD(){
	// Reading the MSB and LSB of the 16-bit temperature value 
	uint8_t temp1=I2C0_ReadRegister(BMPAddress, DataMSB);
	uint8_t temp2=I2C0_ReadRegister(BMPAddress, DataLSB);
		
	// combining the MSB and LSB and returning	
	return ((temp1<<8) + temp2);
}

// this function receives the pressure from BMP180_2, delay of 4.5ms must be there before
int32_t receivePressure1_OLD(){
	// reading MSB, LSB, and XLSB 
	uint8_t temp1=I2C0_ReadRegister(BMPAddress, DataMSB);
	uint8_t temp2=I2C0_ReadRegister(BMPAddress, DataLSB);
	uint8_t temp3=I2C0_ReadRegister(BMPAddress, DataXLSB);
	
	// combining and returning
	return (((temp1<<16)+(temp2<<8)+temp3)>>(8-oss));
}

// this function receives the temperature from BMP180_1, delay of 4.5ms must be there before
int16_t receiveTemperature2_OLD(){
	// Reading the MSB and LSB of the 16-bit temperature value 
	uint8_t temp1=I2C1_ReadRegister(BMPAddress, DataMSB);
	uint8_t temp2=I2C1_ReadRegister(BMPAddress, DataLSB);
		
	// combining the MSB and LSB and returning	
	return ((temp1<<8) + temp2);
}

// this function receives the pressure from BMP180_1, delay of 4.5ms must be there before
int32_t receivePressure2_OLD(){
	// reading MSB, LSB, and XLSB 
	uint8_t temp1=I2C1_ReadRegister(BMPAddress, DataMSB);
	uint8_t temp2=I2C1_ReadRegister(BMPAddress, DataLSB);
	uint8_t temp3=I2C1_ReadRegister(BMPAddress, DataXLSB);
	
	// combining and returning
	return (((temp1<<16)+(temp2<<8)+temp3)>>(8-oss));
}
*/
