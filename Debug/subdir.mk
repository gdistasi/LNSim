################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../FeeCalculator.cpp \
../Gateway.cpp \
../LightningNetwork.cpp \
../NetworkGenerator.cpp \
../PaymentChannel.cpp \
../PaymentChannelEndPoint.cpp \
../PaymentRequest.cpp \
../PaymentsGenerator.cpp \
../Test.cpp \
../main.cpp \
../utils.cpp 

OBJS += \
./FeeCalculator.o \
./Gateway.o \
./LightningNetwork.o \
./NetworkGenerator.o \
./PaymentChannel.o \
./PaymentChannelEndPoint.o \
./PaymentRequest.o \
./PaymentsGenerator.o \
./Test.o \
./main.o \
./utils.o 

CPP_DEPS += \
./FeeCalculator.d \
./Gateway.d \
./LightningNetwork.d \
./NetworkGenerator.d \
./PaymentChannel.d \
./PaymentChannelEndPoint.d \
./PaymentRequest.d \
./PaymentsGenerator.d \
./Test.d \
./main.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


