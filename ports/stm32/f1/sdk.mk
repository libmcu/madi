SDK_ROOT ?= external/STM32CubeF1
LD_SCRIPT ?= $(PORT_ROOT)/STM32F103C8Tx_FLASH.ld

ST_SRCS = \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c \
	\
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \

ST_INCS = \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	$(SDK_ROOT)/Drivers/CMSIS/Include \

ST_DEFS = \
	USE_HAL_DRIVER \
	STM32F103xB \

$(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o)): CFLAGS+=-Wno-error

SRCS += $(ST_SRCS)
INCS += $(ST_INCS)
DEFS += $(ST_DEFS)
