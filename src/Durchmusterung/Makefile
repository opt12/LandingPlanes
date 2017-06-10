CC=g++


landebahn: landebahn.c tile_manager.o
	$(CC) -o landebahn landebahn.c ../readInTiff/src/readInTiff.o tile_manager.o -I../readInTiff/src/  -ltiff

tile_manager.o: tile_manager.cpp
	$(CC) -c tile_manager.cpp -o tile_manager.o -I../readInTiff/src/
