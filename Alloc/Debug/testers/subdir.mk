################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../testers/tester-1.c \
../testers/tester-2.c \
../testers/tester-3.c \
../testers/tester-4.c \
../testers/tester-5.c \
../testers/tester-6.c \
../testers/tester-7.c \
../testers/tester-8.c \
../testers/tester-9.c 

OBJS += \
./testers/tester-1.o \
./testers/tester-2.o \
./testers/tester-3.o \
./testers/tester-4.o \
./testers/tester-5.o \
./testers/tester-6.o \
./testers/tester-7.o \
./testers/tester-8.o \
./testers/tester-9.o 

C_DEPS += \
./testers/tester-1.d \
./testers/tester-2.d \
./testers/tester-3.d \
./testers/tester-4.d \
./testers/tester-5.d \
./testers/tester-6.d \
./testers/tester-7.d \
./testers/tester-8.d \
./testers/tester-9.d 


# Each subdirectory must supply rules for building sources it contributes
testers/%.o: ../testers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


