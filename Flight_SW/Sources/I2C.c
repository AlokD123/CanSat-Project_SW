/*
 * I2C Library for CANSAT 2015
 * Author: Maytham
 * 
 *
 */
 
 #include "derivative.h"
 #include "I2C.h"
 
 #define I2C0_SIM_Init		0x40u
 #define I2C1_SIM_Init		0x80u
 #define PORTB_SIM_Init		0x400u
 #define PORTE_SIM_Init		0x2000u
 #define I2C_Enable_C1		0x80u
 
void I2C0_init(void)
{ 
	// Turn on ports B0 and B1
	SIM_SCGC5 |= PORTB_SIM_Init;
	// PORTS B0 and B1 to alternate function 
	PORTB_PCR0 |= PORT_PCR_MUX(2);
	PORTB_PCR1 |= PORT_PCR_MUX(2);
	
	// SIM_SCGC4: I2C0=1
	SIM_SCGC4 |= I2C0_SIM_Init;
	
	/* I2C0_F: MULT=0,ICR=0x1F */
	I2C0_F = (I2C_F_MULT(0x00) | I2C_F_ICR(0x1F)); /* Set prescaler bits */
	
	I2C0_C1 = 0x00;
	// Enable I2C0 module;
	I2C0_C1 = I2C_Enable_C1;
}
 
void I2C1_init(void)
{
	// Turn on ports E0 and E1
	SIM_SCGC5 |= PORTE_SIM_Init;
	// PORTS E0 and B1 to alternate function 
	PORTE_PCR0 |= PORT_PCR_MUX(6);
	PORTE_PCR1 |= PORT_PCR_MUX(6);
	
	// SIM_SCGC4: I2C1=1
	SIM_SCGC4 |= I2C1_SIM_Init; 

	/* I2C0_F: MULT=0,ICR=0x1F */
	I2C1_F = (I2C_F_MULT(0x00) | I2C_F_ICR(0x1F)); /* Set prescaler bits */
	
	// Enable I2C0 module;
	I2C1_C1 = I2C_Enable_C1;
}

void I2C0_Start(){
	// I2C_C1_TX_MASK == 0x10u, I2C_C1_MST_MASK == 0x20u
	// Transmit mode selected
	I2C0_C1 |= I2C_C1_TX_MASK;
		
	I2C0_C1 |= I2C_C1_MST_MASK;
}

void I2C1_Start(){
	// I2C_C1_TX_MASK == 0x10u, I2C_C1_MST_MASK == 0x20u
	// Transmit mode selected
	I2C1_C1 |= I2C_C1_TX_MASK;
		
	I2C1_C1 |= I2C_C1_MST_MASK;
}

void I2C0_Stop(){
	// I2C_C1_TX_MASK == 0x10u, I2C_C1_MST_MASK == 0x20u
	// Slave mode selected
    I2C0_C1 &= ~I2C_C1_MST_MASK;
			
	I2C0_C1 &= ~I2C_C1_TX_MASK;
}

void I2C1_Stop(){
	// I2C_C1_TX_MASK == 0x10u, I2C_C1_MST_MASK == 0x20u
	// Slave mode selected
    I2C1_C1 &= ~I2C_C1_MST_MASK;
			
	I2C1_C1 &= ~I2C_C1_TX_MASK;
}

uint8_t I2C0_Wait(){
	uint16_t i = 0;
	// Remake the Masks (I2C_S1_IICIF_MASK == 0x2u)
    while((I2C0_S & I2C_S_IICIF_MASK)==0){
    	if(i == 0xFFFF){
    		return 0x00;
    	}
    	i++;
    }
		
	I2C0_S |= I2C_S_IICIF_MASK;
	return 0xFF;
}

uint8_t I2C1_Wait(){
	uint16_t i = 0;
	// Remake the Masks (I2C_S1_IICIF_MASK == 0x2u)
    while((I2C1_S & I2C_S_IICIF_MASK)==0){
    	if(i == 0xFFFF){
			return 0x00;
		}
		i++;
    }
		
	I2C1_S |= I2C_S_IICIF_MASK;
	return 0xFF;
}

void I2C0_RepeatedStart(){
	// I2C_C1_RSTA_MASK == 0x4u;
	I2C0_C1 |= I2C_C1_RSTA_MASK;
}

void I2C1_RepeatedStart(){
	// I2C_C1_RSTA_MASK == 0x4u;
	I2C1_C1 |= I2C_C1_RSTA_MASK;
}

void I2C0_DisableAck(){
	// ACK is not sent on the next byte
	//	I2C_C1_TXAK_MASK == 0x8u
    I2C0_C1 |= I2C_C1_TXAK_MASK;
}

void I2C1_DisableAck(){
	// ACK is not sent on the next byte
	//	I2C_C1_TXAK_MASK == 0x8u
    I2C1_C1 |= I2C_C1_TXAK_MASK;
}

void I2C0_EnableAck(){
	// ACK is sent on the next byte
	//	I2C_C1_TXAK_MASK == 0x8u
    I2C0_C1 &= ~I2C_C1_TXAK_MASK;
}

void I2C1_EnableAck(){
	// ACK is sent on the next byte
	//	I2C_C1_TXAK_MASK == 0x8u
    I2C1_C1 &= ~I2C_C1_TXAK_MASK;
}

void I2C0_EnterRxMode(){
	// I2C_C1_TX_MASK == 0x10u
	// Enter Receive mode
    I2C0_C1 &= ~I2C_C1_TX_MASK;
}

void I2C1_EnterRxMode(){
	// I2C_C1_TX_MASK == 0x10u
	// Enter Receive mode
    I2C1_C1 &= ~I2C_C1_TX_MASK;
}

unsigned char I2C0_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress)
{
	unsigned char result;
	// Starting I2C Transmit and selecting the device as master  
	I2C0_Start();
	
	I2C0_D = u8SlaveAddress<<1;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return 0x00;
	}

	I2C0_D = u8RegisterAddress;										
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return 0x00;
	}

	I2C0_RepeatedStart();
	
	I2C0_D = ((u8SlaveAddress<<1) | 0x01);							
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return 0x00;
	}	

	I2C0_EnterRxMode();
	I2C0_DisableAck();										
	
	result = I2C0_D;
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return 0x00;
	}
	
	// Stopping I2C transmit and making the device a slave
	I2C0_Stop();
			
	result = I2C0_D;
	
	Pause(50);
	return result;
}

unsigned char I2C1_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress)
{
	unsigned char result;
	// Starting I2C Transmit and selecting the device as master  
	I2C1_Start();
	
	I2C1_D = u8SlaveAddress<<1;									
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return 0x00;
	}									

	I2C1_D = u8RegisterAddress;										
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return 0x00;
	}		

	I2C1_RepeatedStart();
	
	I2C1_D = ((u8SlaveAddress<<1) | 0x01);							
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return 0x00;
	}		

	I2C1_EnterRxMode();
	I2C1_DisableAck();										
	
	result = I2C1_D;
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return 0x00;
	}		
	
	// Stopping I2C transmit and making the device a slave
	I2C1_Stop();
			
	result = I2C1_D;
	
	Pause(50);
	return result;
}
 
 
void Pause(int number)
{
	int cnt;
	for(cnt=0; cnt<number; cnt++)
	{
		asm("nop");
	};
}

void I2C0_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, /*unsigned*/ char u8Data)
{
	I2C0_Start();	          
	I2C0_D = u8SlaveAddress << 1;									/* Send I2C device address with W/R bit = 0 */
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = u8RegisterAddress;										/* Send register address */
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = u8Data;												/* Send the data */
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_Stop();

    Pause(50);
}

void I2C1_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, /*unsigned*/ char u8Data)
{
	I2C1_Start();	          
	I2C1_D = u8SlaveAddress << 1;									/* Send I2C device address with W/R bit = 0 */
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	

	I2C1_D = u8RegisterAddress;										/* Send register address */
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	

	I2C1_D = u8Data;												/* Send the data */
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	

	I2C1_Stop();

    Pause(50);
}

void I2C0_MultiByteRead(uint8_t u8SlaveAddress, uint8_t u8RegisterAddress,uint8_t *Data, uint8_t Bytes)
{
	// Starting I2C Transmit and selecting the device as master  
	I2C0_Start();
	
	I2C0_D = u8SlaveAddress<<1;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}										

	I2C0_D = u8RegisterAddress;										
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_RepeatedStart();
	
	I2C0_D = ((u8SlaveAddress<<1) | 0x01);							
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}	

	I2C0_EnterRxMode();
	I2C0_EnableAck();
	Data[0] = I2C0_D;
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	int i = 0;
	for(;i<Bytes;i++){
		Data[i] = I2C0_D;
		if(I2C0_Wait() == 0x00){
			I2C0_Stop();
			return;
		}
		if(i+2==Bytes){
			I2C0_DisableAck();	
		}
	}
	// Stopping I2C transmit and making the device a slave
	I2C0_Stop();
	
	Pause(50);
	return;
}

void I2C1_MultiByteRead(uint8_t u8SlaveAddress, uint8_t u8RegisterAddress,uint8_t *Data, uint8_t Bytes)
{
	// Starting I2C Transmit and selecting the device as master  
	I2C1_Start();
	
	I2C1_D = u8SlaveAddress<<1;									
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}										

	I2C1_D = u8RegisterAddress;										
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	

	I2C1_RepeatedStart();
	
	I2C1_D = ((u8SlaveAddress<<1) | 0x01);							
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	

	I2C1_EnterRxMode();
	I2C1_EnableAck();
	Data[0] = I2C1_D;
	if(I2C1_Wait() == 0x00){
		I2C1_Stop();
		return;
	}	
	
	int i = 0;
	for(;i<Bytes;i++){
		Data[i] = I2C1_D;
		if(I2C1_Wait() == 0x00){
			I2C1_Stop();
			return;
		}	
		if(i+2==Bytes){
			I2C1_DisableAck();	
		}
	}
	// Stopping I2C transmit and making the device a slave
	I2C1_Stop();
	
	Pause(50);
	return;
}

void I2C0_WriteMultiByte(unsigned char SlaveAddress, unsigned char RegisterAddress, unsigned char *Data, int sizeofdata)
{
	I2C0_Start();	          
	I2C0_D = SlaveAddress<<1;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = RegisterAddress;										
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	int i=0;
	for(;i<sizeofdata;i++){
		I2C0_D = Data[i];	
		if(I2C0_Wait() == 0x00){
			I2C0_Stop();
			return;
		}
	}
	I2C0_Stop();

	Pause(50);
}
