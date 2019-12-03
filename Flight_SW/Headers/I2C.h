 
 /*
 * I2C.h Library for CANSAT 2015
 * Author: Maytham
 * 
 *
 */
 
 #ifndef I2C_H_
 #define I2C_H_

 
 
 void I2C0_init(void);
 void I2C1_init(void);
 void I2C0_Start();
 void I2C1_Start();
 void I2C0_Stop();
 void I2C1_Stop();
 uint8_t I2C0_Wait();
 uint8_t I2C1_Wait();
 void I2C0_RepeatedStart();
 void I2C1_RepeatedStart();
 void I2C0_DisableAck();
 void I2C1_DisableAck();
 void I2C0_EnableAck();
 void I2C1_EnableAck();
 void I2C0_EnterRxMode();
 void I2C1_EnterRxMode();
 unsigned char I2C0_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress);
 unsigned char I2C1_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress);
 void Pause(int number);
 void I2C0_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, char u8Data);
 void I2C1_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, char u8Data);
 void I2C0_MultiByteRead(uint8_t u8SlaveAddress, uint8_t u8RegisterAddress,uint8_t *Data, uint8_t Bytes);
 void I2C1_MultiByteRead(uint8_t u8SlaveAddress, uint8_t u8RegisterAddress,uint8_t *Data, uint8_t Bytes);
 void I2C0_WriteMultiByte(unsigned char SlaveAddress, unsigned char RegisterAddress, unsigned char *Data, int sizeofdata);

 #endif /* I2C_H_ */
