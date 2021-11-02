################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/src/hal_adc.c \
../HAL/src/hal_flash.c \
../HAL/src/hal_gpio.c \
../HAL/src/hal_spi.c \
../HAL/src/hal_timer.c \
../HAL/src/hal_uart.c \
../HAL/src/hal_wdg.c 

OBJS += \
./HAL/src/hal_adc.o \
./HAL/src/hal_flash.o \
./HAL/src/hal_gpio.o \
./HAL/src/hal_spi.o \
./HAL/src/hal_timer.o \
./HAL/src/hal_uart.o \
./HAL/src/hal_wdg.o 

C_DEPS += \
./HAL/src/hal_adc.d \
./HAL/src/hal_flash.d \
./HAL/src/hal_gpio.d \
./HAL/src/hal_spi.d \
./HAL/src/hal_timer.d \
./HAL/src/hal_uart.d \
./HAL/src/hal_wdg.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/src/%.o: ../HAL/src/%.c HAL/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/Administrator/Desktop/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

