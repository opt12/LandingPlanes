
CC=g++ -g -std=c++11

ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
else
    CFLAGS=-DNDEBUG
endif

all: checkdir ./bin/landebahn documentation ./lib/plane_library.a 

clean:
	rm ./bin/landebahn || true
	rm -rf latex || true
	rm -rf html || true
	rm ./obj/*.o || true
./bin/landebahn: ./main/landebahn.c ./obj/tile_manager.o ./obj/tile_worker.o ../GeoTiff_Handler/src/GeoTiffHandler.o ../GeoTiff_Handler/src/readInTiff.o ./bin/pcs.csv
	$(CC) $(CFLAGS) -o ./bin/landebahn ./main/landebahn.c ../GeoTiff_Handler/src/GeoTiffHandler.o ../GeoTiff_Handler/src/readInTiff.o ./obj/tile_manager.o ./obj/tile_worker.o -I../readInTiff/src/ -I./include   -I../gdal/gdal-2.2.1/gcore -I../gdal/gdal-2.2.1/port -I../gdal/gdal-2.2.1/ogr -I../GeoTiff_Handler/src ../gdal/gdal-2.2.1/.libs/libgdal.so -ltiff

./obj/tile_manager.o: ./src/tile_manager.cpp ./include/tile_manager.h
	$(CC) $(CFLAGS) -c ./src/tile_manager.cpp -o ./obj/tile_manager.o -I../readInTiff/src/ -I./include -I../gdal/gdal-2.2.1/gcore -I../gdal/gdal-2.2.1/port -I../gdal/gdal-2.2.1/ogr -I../GeoTiff_Handler/src

./obj/tile_worker.o: ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) $(CFLAGS) -c ./src/tile_worker.cpp -o ./obj/tile_worker.o -I../readInTiff/src/ -I./include -I../gdal/gdal-2.2.1/gcore -I../gdal/gdal-2.2.1/port -I../gdal/gdal-2.2.1/ogr -I../GeoTiff_Handler/src

./obj/plane_library.o: ./src/plane_library.cpp
	$(CC) $(CFLAGS) -c ./src/plane_library.cpp -o ./obj/plane_library.o -I../readInTiff/src/ -I./include -I../GeoTiff_Handler/src -I../gdal/gdal-2.2.1/gcore -I../gdal/gdal-2.2.1/port -I../gdal/gdal-2.2.1/ogr

./lib/plane_library.a: ./obj/plane_library.o
	ar rcs ./lib/plane_library.a ./obj/plane_library.o

../GeoTiff_handler/src/readInTiff.o:
	$(MAKE) -C ../GeoTiff_Handler/src/

../GeoTiff_handler/src/GeoTiffHandler.o:
	$(MAKE) -C ../GeoTiff_Handler/src/

./bin/pcs.csv: ../gdal/gdal-2.2.1/data/pcs.csv
	cp  ../gdal/gdal-2.2.1/data/pcs.csv ./bin

documentation: html/index.html

checkdir:
	mkdir bin || true
	mkdir obj || true
	mkdir lib || true

html/index.html: ./src/tile_manager.cpp ./src/tile_worker.cpp ./include/tile_worker.h ./include/tile_manager.h 
	doxygen doxystyle

