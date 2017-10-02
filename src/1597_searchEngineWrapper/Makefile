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

all:  ./bin/1597_searchEngineWrapper

precheck:
ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    	$(error tiff.h needed. This is usually in a package libtiff-dev)
endif

ifeq ("$(wildcard /usr/include/gdal/gdal_priv.h)","")
    	$(error gdal_priv.h needed. This is usually in a package libgdal-dev)
endif

clean:
	$(RM) ./bin/1597_searchEngineWrapper || true
	$(RM) ./obj/*.o || true
	$(RM) ./obj/*.d || true


checkdir:
	mkdir bin || true
	mkdir obj || true

checkGeoTiffHandlerLib: ../GeoTiff_Handler/lib/libGeoTiffHandler.a

../GeoTiff_Handler/lib/libGeoTiffHandler.a: 
	$(MAKE) -C ../GeoTiff_Handler/ easylib

check_plane_library: ../Durchmusterung/lib/plane_library.a

../Durchmusterung/lib/plane_library.a: 
	$(MAKE) -C ../Durchmusterung/ easylib


# All of the sources participating in the build are defined here
# Add inputs and outputs from these tool invocations to the build variables 

OBJS += \
obj/1597_FakeDurchmusterung.o \
obj/1597_QueueDispatcher.o \
obj/1597_ipc_listener.o \
obj/1597_scanToMFile.o \
obj/1597_searchEngineWrapper.o \
obj/1597_searcherTask.o \
obj/geojson_utils.o 

LIBS := -lpthread -l:plane_library.a -lGeoTiffHandler -lgdal -ltiff
LIB_PATH := -L../Durchmusterung/lib -L../GeoTiff_Handler/lib 
INCLUDE_PATH := -I/usr/include/gdal \
-I../GeoTiff_Handler/src \
-I../1597_searchEngineWrapper/src \
-I../Durchmusterung/include 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x $(INCLUDE_PATH) $(OPTIMIZATION) -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Tool invocations
./bin/1597_searchEngineWrapper: checkdir checkGeoTiffHandlerLib check_plane_library $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -o "./bin/1597_searchEngineWrapper" $(INCLUDE_PATH) $(OPTIMIZATION) $(OBJS) $(USER_OBJS) $(LIB_PATH) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


.PHONY: checkdir all clean dependents
