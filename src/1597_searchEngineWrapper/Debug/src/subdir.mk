################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/1597_FakeDurchmusterung.cpp \
../src/1597_QueueDispatcher.cpp \
../src/1597_ipc_listener.cpp \
../src/1597_scanToMFile.cpp \
../src/1597_searchEngineWrapper.cpp \
../src/1597_searcherTask.cpp \
../src/geojson_utils.cpp 

OBJS += \
./src/1597_FakeDurchmusterung.o \
./src/1597_QueueDispatcher.o \
./src/1597_ipc_listener.o \
./src/1597_scanToMFile.o \
./src/1597_searchEngineWrapper.o \
./src/1597_searcherTask.o \
./src/geojson_utils.o 

CPP_DEPS += \
./src/1597_FakeDurchmusterung.d \
./src/1597_QueueDispatcher.d \
./src/1597_ipc_listener.d \
./src/1597_scanToMFile.d \
./src/1597_searchEngineWrapper.d \
./src/1597_searcherTask.d \
./src/geojson_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/gdal -I../../GeoTiff_Handler/src -I../../1597_searchEngineWrapper/src -I../../Durchmusterung/include -I../../gdal/gdal-2.2.1/gcore -I../../gdal/gdal-2.2.1/port -I../../gdal/gdal-2.2.1/ogr -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


