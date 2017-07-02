
CC=g++ -g

all: checkdir ./bin/landebahn documentation

clean:
	rm ./bin/landebahn || true
	rm -rf latex || true
	rm -rf html || true
	rm ./obj/*.o || true
./bin/landebahn: ./main/landebahn.c ./obj/tile_manager.o ./obj/tile_worker.o ../readInTiff/src/readInTiff.o
	$(CC) -o ./bin/landebahn ./main/landebahn.c ../readInTiff/src/readInTiff.o ./obj/tile_manager.o ./obj/tile_worker.o -I../readInTiff/src/ -I./include  -ltiff

./obj/tile_manager.o: ./src/tile_manager.cpp ./include/tile_manager.h
	$(CC) -c ./src/tile_manager.cpp -o ./obj/tile_manager.o -I../readInTiff/src/ -I./include

./obj/tile_worker.o: ./src/tile_worker.cpp ./include/tile_worker.h
	$(CC) -c ./src/tile_worker.cpp -o ./obj/tile_worker.o -I../readInTiff/src/ -I./include

../readInTiff/src/readInTiff.o:
	$(MAKE) -C ../readInTiff/src

documentation: html/index.html

checkdir:
	mkdir bin || true
	mkdir obj || true

html/index.html: ./src/tile_manager.cpp ./src/tile_worker.cpp ./include/tile_worker.h ./include/tile_manager.h 
	doxygen doxystyle

