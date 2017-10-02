RM := rm -rf

GCC_AVAIL := $(shell command -v g++ 2> /dev/null)
ifndef GCC_AVAIL
$(error g++ needs to be installed in version 4.9 or newer)
endif

GCCVERSION=$(shell g++ --version | grep ^g++ | sed 's/^.* //g')


ifndef GCCVERSION
$(error g++ needs to be installed)
endif


GCCMAJOR := $(shell echo "$(GCCVERSION)" | cut -f1 -d.)
GCCMINOR := $(shell echo "$(GCCVERSION)" | cut -f2 -d.)


ifeq  ($(shell test $(GCCMAJOR) -lt 4; echo $$?),0)
$(error g++ version needs to be at least 4.9)
endif

ifeq  ($(shell test $(GCCMAJOR) -eq 4; echo $$?),0)
ifeq ($(shell test $(GCCMINOR) -lt 9; echo $$?),0)
$(error g++ version needs to be at least 4.9)
endif
endif

ifeq ("$(wildcard /usr/include/tiff.h)","")
  ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    $(error tiff.h needed. This is usually in a package libtiff-devel or libtiff-dev)
  endif
endif

ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
	OPTIMIZATION = -O0 -g3
else
    CFLAGS=-DNDEBUG
	OPTIMIZATION = -O3
endif

all: easylib ./bin/GeoTiffHandlerTest

easylib: checkdir  ./lib/libGeoTiffHandler.a

precheck:
ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    	$(error tiff.h needed. This is usually in a package libtiff-dev)
endif

ifeq ("$(wildcard /usr/include/gdal/gdal_priv.h)","")
    	$(error gdal_priv.h needed. This is usually in a package libgdal-dev)
endif

clean:
	$(RM) ./bin/GeoTiffHandlerTest || true
	$(RM) ./lib/libGeoTiffHandler.a || true
	$(RM) ./obj/*.o || true
	$(RM) ./obj/*.d || true


checkdir:
	mkdir bin || true
	mkdir obj || true
	mkdir lib || true


# All of the sources participating in the build are defined here
# Add inputs and outputs from these tool invocations to the build variables 

OBJS += \
./obj/GeoTiffHandler.o \
./obj/readInTiff.o \
./obj/GeoTiffHandlerTest.o 

LIBS := -lgdal -ltiff
LIB_PATH := 


#CPP_DEPS += \
#./src/GeoTiffHandler.d 
#
#C_DEPS += \
#./src/readInTiff.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/include/gdal $(OPTIMIZATION) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I/usr/include/gdal $(OPTIMIZATION) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Tool invocations
./lib/libGeoTiffHandler.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r  "./lib/libGeoTiffHandler.a" $(OBJS) $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	
./bin/GeoTiffHandlerTest: $(OBJS) ./obj/GeoTiffHandlerTest.o $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "./bin/GeoTiffHandlerTest" $(OBJS) $(USER_OBJS) $(LIB_PATH) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	

.PHONY: all clean easylib
