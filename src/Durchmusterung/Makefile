CC=g++ -g


landebahn: landebahn.c tile_manager.o tile_worker.o
	$(CC) -o landebahn landebahn.c ../readInTiff/src/readInTiff.o tile_manager.o tile_worker.o -I../readInTiff/src/  -ltiff

tile_manager.o: tile_manager.cpp tile_manager.h
	$(CC) -c tile_manager.cpp -o tile_manager.o -I../readInTiff/src/

tile_worker.o: tile_worker.cpp tile_worker.h
	$(CC) -c tile_worker.cpp -o tile_worker.o -I../readInTiff/src/
