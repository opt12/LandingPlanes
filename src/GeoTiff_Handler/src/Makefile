CC=g++ -std=c++11


ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
	$(error tiff.h needed. This is usually in a package libtiff-dev)
endif

ifeq ("$(wildcard /usr/include/gdal/gdal_priv.h)","")
	$(error gdal_priv.h needed. This is usually in a package libgdal-dev)
endif



all: geotiff

geotiff: readInTiff.o GeoTiffHandler.o GeoTiffHandlerTest 


readInTiff.o: readInTiff.c
	$(CC) -c readInTiff.c -ltiff

GeoTiffHandler.o: GeoTiffHandler.cpp
	$(CC) -c GeoTiffHandler.cpp -I/usr/include/gdal

GeoTiffHandlerTest: GeoTiffHandlerTest.cpp readInTiff.o
	$(CC) -o GeoTiffHandlerTest  GeoTiffHandlerTest.cpp GeoTiffHandler.o readInTiff.o -I/usr/include/gdal -ltiff -lgdal
