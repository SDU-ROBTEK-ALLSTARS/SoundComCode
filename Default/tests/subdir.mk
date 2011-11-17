################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/circ_buf_ex.cpp \
../tests/dll_test_main.cpp 

C_SRCS += \
../tests/patest_sine.c 

OBJS += \
./tests/circ_buf_ex.o \
./tests/dll_test_main.o \
./tests/patest_sine.o 

C_DEPS += \
./tests/patest_sine.d 

CPP_DEPS += \
./tests/circ_buf_ex.d \
./tests/dll_test_main.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/mariannebondelarsen/Desktop/projekt/SoundComCode/include" -I/opt/local/boost_1_46_1/boost_1_46_1 -O2 -g -Wall -c -fmessage-length=0 -arch i686 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

tests/%.o: ../tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


