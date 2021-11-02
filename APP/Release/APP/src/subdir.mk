################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/src/app_communication.c \
../APP/src/app_data.c \
../APP/src/app_device.c \
../APP/src/app_protocol.c \
../APP/src/app_task.c 

OBJS += \
./APP/src/app_communication.o \
./APP/src/app_data.o \
./APP/src/app_device.o \
./APP/src/app_protocol.o \
./APP/src/app_task.o 

C_DEPS += \
./APP/src/app_communication.d \
./APP/src/app_data.d \
./APP/src/app_device.d \
./APP/src/app_protocol.d \
./APP/src/app_task.d 


# Each subdirectory must supply rules for building sources it contributes
APP/src/app_communication.o: ../APP/src/app_communication.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"APP/src/app_communication.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
APP/src/app_data.o: ../APP/src/app_data.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"APP/src/app_data.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
APP/src/app_device.o: ../APP/src/app_device.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"APP/src/app_device.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
APP/src/app_protocol.o: ../APP/src/app_protocol.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"APP/src/app_protocol.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
APP/src/app_task.o: ../APP/src/app_task.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"APP/src/app_task.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

