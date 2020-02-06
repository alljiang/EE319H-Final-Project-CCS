################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../EE319H.cfg 

CPP_SRCS += \
../ILI9341.cpp \
../dma.cpp \
../start.cpp 

CMD_SRCS += \
../EK_TM4C123GXL.cmd 

C_SRCS += \
../EK_TM4C123GXL.c \
../SD_SPI.c \
../UART.c \
../Utils.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./EK_TM4C123GXL.d \
./SD_SPI.d \
./UART.d \
./Utils.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./EK_TM4C123GXL.obj \
./ILI9341.obj \
./SD_SPI.obj \
./UART.obj \
./Utils.obj \
./dma.obj \
./start.obj 

CPP_DEPS += \
./ILI9341.d \
./dma.d \
./start.d 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"EK_TM4C123GXL.obj" \
"ILI9341.obj" \
"SD_SPI.obj" \
"UART.obj" \
"Utils.obj" \
"dma.obj" \
"start.obj" 

C_DEPS__QUOTED += \
"EK_TM4C123GXL.d" \
"SD_SPI.d" \
"UART.d" \
"Utils.d" 

CPP_DEPS__QUOTED += \
"ILI9341.d" \
"dma.d" \
"start.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../EK_TM4C123GXL.c" \
"../SD_SPI.c" \
"../UART.c" \
"../Utils.c" 

CPP_SRCS__QUOTED += \
"../ILI9341.cpp" \
"../dma.cpp" \
"../start.cpp" 


