CC=g++ -std=c++11


precheck:
	ifeq ("$(wildcard /usr/include/x86_64-linux-gnu/tiff.h)","")
    		$(error tiff.h needed. This is usually in a package libtiff-dev)
	endif

	ifeq ("$(wildcard /usr/include/gdal/gdal_priv.h)","")
    		$(error gdal_priv.h needed. This is usually in a package libgdal-dev)
	endif



all: geotiff

geotiff: readInTiff.o GeoTiffHandler.o GeoTiffHandlerTest 


readInTiff.o: precheck readInTiff.c
	$(CC) -c readInTiff.c -ltiff

GeoTiffHandler.o: precheck GeoTiffHandler.cpp
	$(CC) -c GeoTiffHandler.cpp -I/usr/include/gdal

GeoTiffHandlerTest: precheck GeoTiffHandlerTest.cpp readInTiff.o
	$(CC) -o GeoTiffHandlerTest  GeoTiffHandlerTest.cpp GeoTiffHandler.o readInTiff.o -I/usr/include/gdal -ltiff -lgdal
