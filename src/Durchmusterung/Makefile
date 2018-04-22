CC=g++ -pg -g -std=c++11 -O3 -Wall

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

DOXY_AVAIL := $(shell command -v doxygen 2> /dev/null)

#test -f /usr/include/tiff.h && $(error tiff.h needed. This is usually in a package libtiff-devel)

ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
else
    CFLAGS=-DNDEBUG
endif

all: checkdir checkGeoTiffHandlerLib documentation ./lib/plane_library.a ./bin/landebahn

easylib: checkdir checkGeoTiffHandlerLib documentation ./lib/plane_library.a

precheck:
ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    	$(error tiff.h needed. This is usually in a package libtiff-dev)
endif

clean:
	rm ./bin/landebahn || true
	rm ./lib/plane_library.a || true
	rm -rf latex || true
	rm -rf html || true
	rm ./obj/*.o || true

./bin/landebahn: ./main/landebahn.c ./obj/landing_plane.o ./obj/tile_manager.o ./obj/tile_worker.o ./obj/thread_data.o ./obj/global.o  ../1597_searchEngineWrapper/obj/1597_ipc_listener.o
	$(CC) $(CFLAGS) -o ./bin/landebahn ./main/landebahn.c ./obj/tile_manager.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o ./obj/global.o -I../readInTiff/src/ -I./include  -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/ -L../GeoTiff_Handler/lib -lGeoTiffHandler ../1597_searchEngineWrapper/obj/1597_ipc_listener.o  -lpthread -lgdal -ltiff 

./obj/tile_manager.o: ./src/tile_manager.cpp ./include/tile_manager.h 
	$(CC) $(CFLAGS) -c ./src/tile_manager.cpp -o ./obj/tile_manager.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/ -L../GeoTiff_Handler/lib -lGeoTiffHandler

./obj/global.o: ./src/global.cpp
	$(CC) $(CFLAGS) -c ./src/global.cpp -o ./obj/global.o 

./obj/tile_worker.o: ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) $(CFLAGS) -c ./src/tile_worker.cpp -o ./obj/tile_worker.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/


./obj/landing_plane.o: ./src/landing_plane.cpp ./include/landing_plane.h
	$(CC) $(CFLAGS) -c ./src/landing_plane.cpp -o ./obj/landing_plane.o -I./include

./obj/plane_library.o: ./src/plane_library.cpp ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) $(CFLAGS) -c ./src/plane_library.cpp -o ./obj/plane_library.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I/usr/include/gdal -I../1597_searchEngineWrapper/src/

./obj/thread_data.o: ./src/thread_data.cpp ./include/thread_data.h
	$(CC) $(CFLAGS) -c ./src/thread_data.cpp -o ./obj/thread_data.o -I./include -I../readInTiff/src/ -I../GeoTiff_Handler/src -I/usr/include/gdal -I../1597_searchEngineWrapper/src/

./lib/plane_library.a: ./obj/plane_library.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o ./obj/global.o
	ar rcs ./lib/plane_library.a ./obj/plane_library.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o ./obj/global.o

#../GeoTiff_Handler/src/readInTiff.o: precheck
#	$(MAKE) -C ../GeoTiff_Handler/src/ -I/usr/include/gdal/

#../GeoTiff_Handler/src/GeoTiffHandler.o: precheck
#	$(MAKE) -C ../GeoTiff_Handler/src/ -I/usr/include/gdal/

checkGeoTiffHandlerLib: ../GeoTiff_Handler/lib/libGeoTiffHandler.a

../GeoTiff_Handler/lib/libGeoTiffHandler.a: 
	$(MAKE) -C ../GeoTiff_Handler/ easylib

../1597_searchEngineWrapper/obj/1597_ipc_listener.o: ./lib/plane_library.a
	$(MAKE) -C ../1597_searchEngineWrapper/

documentation: html/index.html

checkdir:
	mkdir bin || true
	mkdir obj || true
	mkdir lib || true

html/index.html: ./src/tile_manager.cpp ./src/tile_worker.cpp ./src/landing_plane.cpp ./src/plane_library.cpp ./src/thread_data.cpp  ./src/global.cpp ./include/thread_data.h ./include/plane_library.h ./include/landing_plane.h ./include/global.h ./include/tile_worker.h ./include/tile_manager.h
ifndef DOXY_AVAIL
	$(info "doxygen not available so no documentation created")
else 
	doxygen doxystyle
endif

.PHONY: all clean documentation checkdir precheck checkGeoTiffHandlerLib

