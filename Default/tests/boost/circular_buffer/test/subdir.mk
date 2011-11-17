################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/boost/circular_buffer/test/base_test.cpp \
../tests/boost/circular_buffer/test/bounded_buffer_comparison.cpp \
../tests/boost/circular_buffer/test/constant_erase_test.cpp \
../tests/boost/circular_buffer/test/soft_iterator_invalidation.cpp \
../tests/boost/circular_buffer/test/space_optimized_test.cpp 

OBJS += \
./tests/boost/circular_buffer/test/base_test.o \
./tests/boost/circular_buffer/test/bounded_buffer_comparison.o \
./tests/boost/circular_buffer/test/constant_erase_test.o \
./tests/boost/circular_buffer/test/soft_iterator_invalidation.o \
./tests/boost/circular_buffer/test/space_optimized_test.o 

CPP_DEPS += \
./tests/boost/circular_buffer/test/base_test.d \
./tests/boost/circular_buffer/test/bounded_buffer_comparison.d \
./tests/boost/circular_buffer/test/constant_erase_test.d \
./tests/boost/circular_buffer/test/soft_iterator_invalidation.d \
./tests/boost/circular_buffer/test/space_optimized_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/boost/circular_buffer/test/%.o: ../tests/boost/circular_buffer/test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/mariannebondelarsen/Desktop/projekt/SoundComCode/include" -I/opt/local/boost_1_46_1/boost_1_46_1 -O2 -g -Wall -c -fmessage-length=0 -arch i686 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


