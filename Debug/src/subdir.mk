################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c 

CU_SRCS += \
../src/DFT.cu 

CU_DEPS += \
./src/DFT.d 

OBJS += \
./src/DFT.o \
./src/main.o 

C_DEPS += \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -G -g -O0 -gencode arch=compute_60,code=sm_60  -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -G -g -O0 --compile --relocatable-device-code=false -gencode arch=compute_60,code=compute_60 -gencode arch=compute_60,code=sm_60  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -G -g -O0 -gencode arch=compute_60,code=sm_60  -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -G -g -O0 --compile  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


