################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SX127xLib/radio.c \
../SX127xLib/sx1276-Fsk.c \
../SX127xLib/sx1276-FskMisc.c \
../SX127xLib/sx1276-Hal.c \
../SX127xLib/sx1276-LoRa.c \
../SX127xLib/sx1276-LoRaMisc.c \
../SX127xLib/sx1276.c 

OBJS += \
./SX127xLib/radio.o \
./SX127xLib/sx1276-Fsk.o \
./SX127xLib/sx1276-FskMisc.o \
./SX127xLib/sx1276-Hal.o \
./SX127xLib/sx1276-LoRa.o \
./SX127xLib/sx1276-LoRaMisc.o \
./SX127xLib/sx1276.o 

C_DEPS += \
./SX127xLib/radio.d \
./SX127xLib/sx1276-Fsk.d \
./SX127xLib/sx1276-FskMisc.d \
./SX127xLib/sx1276-Hal.d \
./SX127xLib/sx1276-LoRa.d \
./SX127xLib/sx1276-LoRaMisc.d \
./SX127xLib/sx1276.d 


# Each subdirectory must supply rules for building sources it contributes
SX127xLib/radio.o: ../SX127xLib/radio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/radio.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276-Fsk.o: ../SX127xLib/sx1276-Fsk.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276-Fsk.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276-FskMisc.o: ../SX127xLib/sx1276-FskMisc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276-FskMisc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276-Hal.o: ../SX127xLib/sx1276-Hal.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276-Hal.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276-LoRa.o: ../SX127xLib/sx1276-LoRa.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276-LoRa.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276-LoRaMisc.o: ../SX127xLib/sx1276-LoRaMisc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276-LoRaMisc.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SX127xLib/sx1276.o: ../SX127xLib/sx1276.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../DRV/inc -I../HAL/inc -I../APP/inc -I"C:/Users/JTZL_ZX/Desktop/SuperGateway/APP/SX127xLib" -O0 -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage -MMD -MP -MF"SX127xLib/sx1276.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

