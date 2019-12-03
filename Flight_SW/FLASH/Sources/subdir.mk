################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/ADC.c" \
"../Sources/AK8963.c" \
"../Sources/AT24CM01.c" \
"../Sources/BMP180.c" \
"../Sources/CanSat-Main.c" \
"../Sources/DS1338.c" \
"../Sources/GPIO.c" \
"../Sources/I2C.c" \
"../Sources/MPU6500.c" \
"../Sources/MotorControl.c" \
"../Sources/PID.c" \
"../Sources/Scheduling.c" \
"../Sources/UART.c" \
"../Sources/delay.c" \
"../Sources/sa_mtb.c" \

C_SRCS += \
../Sources/ADC.c \
../Sources/AK8963.c \
../Sources/AT24CM01.c \
../Sources/BMP180.c \
../Sources/CanSat-Main.c \
../Sources/DS1338.c \
../Sources/GPIO.c \
../Sources/I2C.c \
../Sources/MPU6500.c \
../Sources/MotorControl.c \
../Sources/PID.c \
../Sources/Scheduling.c \
../Sources/UART.c \
../Sources/delay.c \
../Sources/sa_mtb.c \

OBJS += \
./Sources/ADC.o \
./Sources/AK8963.o \
./Sources/AT24CM01.o \
./Sources/BMP180.o \
./Sources/CanSat-Main.o \
./Sources/DS1338.o \
./Sources/GPIO.o \
./Sources/I2C.o \
./Sources/MPU6500.o \
./Sources/MotorControl.o \
./Sources/PID.o \
./Sources/Scheduling.o \
./Sources/UART.o \
./Sources/delay.o \
./Sources/sa_mtb.o \

C_DEPS += \
./Sources/ADC.d \
./Sources/AK8963.d \
./Sources/AT24CM01.d \
./Sources/BMP180.d \
./Sources/CanSat-Main.d \
./Sources/DS1338.d \
./Sources/GPIO.d \
./Sources/I2C.d \
./Sources/MPU6500.d \
./Sources/MotorControl.d \
./Sources/PID.d \
./Sources/Scheduling.d \
./Sources/UART.d \
./Sources/delay.d \
./Sources/sa_mtb.d \

OBJS_QUOTED += \
"./Sources/ADC.o" \
"./Sources/AK8963.o" \
"./Sources/AT24CM01.o" \
"./Sources/BMP180.o" \
"./Sources/CanSat-Main.o" \
"./Sources/DS1338.o" \
"./Sources/GPIO.o" \
"./Sources/I2C.o" \
"./Sources/MPU6500.o" \
"./Sources/MotorControl.o" \
"./Sources/PID.o" \
"./Sources/Scheduling.o" \
"./Sources/UART.o" \
"./Sources/delay.o" \
"./Sources/sa_mtb.o" \

C_DEPS_QUOTED += \
"./Sources/ADC.d" \
"./Sources/AK8963.d" \
"./Sources/AT24CM01.d" \
"./Sources/BMP180.d" \
"./Sources/CanSat-Main.d" \
"./Sources/DS1338.d" \
"./Sources/GPIO.d" \
"./Sources/I2C.d" \
"./Sources/MPU6500.d" \
"./Sources/MotorControl.d" \
"./Sources/PID.d" \
"./Sources/Scheduling.d" \
"./Sources/UART.d" \
"./Sources/delay.d" \
"./Sources/sa_mtb.d" \

OBJS_OS_FORMAT += \
./Sources/ADC.o \
./Sources/AK8963.o \
./Sources/AT24CM01.o \
./Sources/BMP180.o \
./Sources/CanSat-Main.o \
./Sources/DS1338.o \
./Sources/GPIO.o \
./Sources/I2C.o \
./Sources/MPU6500.o \
./Sources/MotorControl.o \
./Sources/PID.o \
./Sources/Scheduling.o \
./Sources/UART.o \
./Sources/delay.o \
./Sources/sa_mtb.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/ADC.o: ../Sources/ADC.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/ADC.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/ADC.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/AK8963.o: ../Sources/AK8963.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/AK8963.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/AK8963.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/AT24CM01.o: ../Sources/AT24CM01.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/AT24CM01.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/AT24CM01.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/BMP180.o: ../Sources/BMP180.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/BMP180.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/BMP180.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/CanSat-Main.o: ../Sources/CanSat-Main.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/CanSat-Main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/CanSat-Main.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/DS1338.o: ../Sources/DS1338.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/DS1338.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/DS1338.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/GPIO.o: ../Sources/GPIO.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/GPIO.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/GPIO.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/I2C.o: ../Sources/I2C.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/I2C.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/I2C.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/MPU6500.o: ../Sources/MPU6500.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/MPU6500.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/MPU6500.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/MotorControl.o: ../Sources/MotorControl.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/MotorControl.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/MotorControl.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/PID.o: ../Sources/PID.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/PID.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/PID.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Scheduling.o: ../Sources/Scheduling.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Scheduling.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Scheduling.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/UART.o: ../Sources/UART.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/UART.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/UART.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/delay.o: ../Sources/delay.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/delay.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/delay.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/sa_mtb.o: ../Sources/sa_mtb.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/sa_mtb.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/sa_mtb.o"
	@echo 'Finished building: $<'
	@echo ' '


