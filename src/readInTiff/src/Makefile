CC=gcc

all: geotiff

geotiff: testReadInTiff

testReadInTiff: readInTiff.o testReadInTiff.c
	$(CC) -o testReadInTiff testReadInTiff.c -std=c99 readInTiff.o -ltiff

readInTiff.o: readInTiff.c
	$(CC) -c readInTiff.c -ltiff 
