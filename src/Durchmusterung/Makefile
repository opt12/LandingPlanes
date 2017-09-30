

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
    $(error tiff.h needed. This is usually in a package libtiff-devel)
  endif
endif

DOXY_AVAIL := $(shell command -v doxygen 2> /dev/null)

#test -f /usr/include/tiff.h && $(error tiff.h needed. This is usually in a package libtiff-devel)

ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
else
    CFLAGS=-DNDEBUG
endif

all: checkdir ./bin/landebahn documentation ./lib/plane_library.a 

easylib: checkdir documentation ./lib/plane_library.a

precheck:
	ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    		$(error tiff.h needed. This is usually in a package libtiff-dev)
	endif

	ifeq ("$(wildcard /usr/include/gdal/gdal_priv.h)","")
    		$(error gdal_priv.h needed. This is usually in a package libgdal-dev)
	endif

clean:
	rm ./bin/landebahn || true
	rm -rf latex || true
	rm -rf html || true
	rm ./obj/*.o || true
./bin/landebahn: ./main/landebahn.c ./obj/landing_plane.o ./obj/tile_manager.o ./obj/tile_worker.o ./obj/thread_data.o ../GeoTiff_Handler/src/GeoTiffHandler.o ../GeoTiff_Handler/src/readInTiff.o ../1597_searchEngineWrapper/Debug/src/1597_ipc_listener.o
	$(CC) $(CFLAGS) -o ./bin/landebahn ./main/landebahn.c ../GeoTiff_Handler/src/GeoTiffHandler.o ../GeoTiff_Handler/src/readInTiff.o ./obj/tile_manager.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o -I../readInTiff/src/ -I./include  -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/  ../1597_searchEngineWrapper/Debug/src/1597_ipc_listener.o  -ltiff -lpthread -lgdal

./obj/tile_manager.o: ./src/tile_manager.cpp ./include/tile_manager.h ../GeoTiff_Handler/src/GeoTiffHandler.o
	$(CC) $(CFLAGS) -c ./src/tile_manager.cpp -o ./obj/tile_manager.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/

./obj/tile_worker.o: ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) $(CFLAGS) -c ./src/tile_worker.cpp -o ./obj/tile_worker.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I../1597_searchEngineWrapper/src/ -I/usr/include/gdal/


./obj/landing_plane.o: ./src/landing_plane.cpp ./include/landing_plane.h
	$(CC) $(CFLAGS) -c ./src/landing_plane.cpp -o ./obj/landing_plane.o -I./include

./obj/plane_library.o: ./src/plane_library.cpp ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) $(CFLAGS) -c ./src/plane_library.cpp -o ./obj/plane_library.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I/usr/include/gdal -I../1597_searchEngineWrapper/src/

./obj/thread_data.o: ./src/thread_data.cpp ./include/thread_data.h
	$(CC) $(CFLAGS) -c ./src/thread_data.cpp -o ./obj/thread_data.o -I./include -I../readInTiff/src/ -I../GeoTiff_Handler/src -I/usr/include/gdal -I../1597_searchEngineWrapper/src/

./lib/plane_library.a: ./obj/plane_library.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o
	ar rcs ./lib/plane_library.a ./obj/plane_library.o ./obj/tile_worker.o ./obj/landing_plane.o ./obj/thread_data.o

../GeoTiff_handler/src/readInTiff.o: precheck
	$(MAKE) -C ../GeoTiff_Handler/src/ -I/usr/include/gdal/

../GeoTiff_handler/src/GeoTiffHandler.o: precheck
	$(MAKE) -C ../GeoTiff_Handler/src/ -I/usr/include/gdal/


../1597_searchEngineWrapper/Debug/src/1597_ipc_listener.o: ./lib/plane_library.a
	$(MAKE) all -C ../1597_searchEngineWrapper/Debug/

documentation: html/index.html

checkdir:
	mkdir bin || true
	mkdir obj || true
	mkdir lib || true

html/index.html: ./src/tile_manager.cpp ./src/tile_worker.cpp ./include/tile_worker.h ./include/tile_manager.h
ifndef DOXY_AVAIL
	$(info "doxygen not available so no documentation created")
else 
	doxygen doxystyle
endif

