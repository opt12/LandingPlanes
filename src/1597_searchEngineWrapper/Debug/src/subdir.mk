################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/1597_QueueDispatcher.cpp \
../src/1597_ipc_listener.cpp \
../src/1597_scanToMFile.cpp \
../src/1597_searchEngineWrapper.cpp \
/home/eckstein/git/LandingPlanes/src/GeoTiff_Handler/src/GeoTiffHandler.cpp 

C_SRCS += \
/home/eckstein/git/LandingPlanes/src/GeoTiff_Handler/src/readInTiff.c 

OBJS += \
./src/1597_QueueDispatcher.o \
./src/1597_ipc_listener.o \
./src/1597_scanToMFile.o \
./src/1597_searchEngineWrapper.o \
./src/GeoTiffHandler.o \
./src/readInTiff.o 

CPP_DEPS += \
./src/1597_QueueDispatcher.d \
./src/1597_ipc_listener.d \
./src/1597_scanToMFile.d \
./src/1597_searchEngineWrapper.d \
./src/GeoTiffHandler.d 

C_DEPS += \
./src/readInTiff.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/gdal -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/GeoTiffHandler.o: /home/eckstein/git/LandingPlanes/src/GeoTiff_Handler/src/GeoTiffHandler.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/gdal -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/readInTiff.o: /home/eckstein/git/LandingPlanes/src/GeoTiff_Handler/src/readInTiff.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


