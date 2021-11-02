################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DRV/src/drv_485.c \
../DRV/src/drv_ble.c \
../DRV/src/drv_cat1.c \
../DRV/src/drv_debug.c \
../DRV/src/drv_led.c \
../DRV/src/drv_mx25.c \
../DRV/src/drv_pwm2iv.c \
../DRV/src/drv_sx1278.c 

OBJS += \
./DRV/src/drv_485.o \
./DRV/src/drv_ble.o \
./DRV/src/drv_cat1.o \
./DRV/src/drv_debug.o \
./DRV/src/drv_led.o \
./DRV/src/drv_mx25.o \
./DRV/src/drv_pwm2iv.o \
./DRV/src/drv_sx1278.o 

C_DEPS += \
./DRV/src/drv_485.d \
./DRV/src/drv_ble.d \
./DRV/src/drv_cat1.d \
./DRV/src/drv_debug.d \
./DRV/src/drv_led.d \
./DRV/src/drv_mx25.d \
./DRV/src/drv_pwm2iv.d \
./DRV/src/drv_sx1278.d 


# Each subdirectory must supply rules for building sources it contributes
DRV/src/%.o: ../DRV/src/%.c DRV/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/Administrator/Desktop/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

