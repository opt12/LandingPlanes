CC=g++ -std=c++11

all: geotiff

geotiff: readInTiff.o GeoTiffHandler.o GeoTiffHandlerTest 


readInTiff.o: readInTiff.c
	$(CC) -c readInTiff.c -ltiff -I../../gdal/gdal-2.2.1/gcore -I../../gdal/gdal-2.2.1/port -I../../gdal/gdal-2.2.1/ogr 

GeoTiffHandler.o: GeoTiffHandler.cpp
	$(CC) -c GeoTiffHandler.cpp -I../../gdal/gdal-2.2.1/gcore -I../../gdal/gdal-2.2.1/port -I../../gdal/gdal-2.2.1/ogr

GeoTiffHandlerTest: GeoTiffHandlerTest.cpp readInTiff.o pcs.csv
	$(CC) -o GeoTiffHandlerTest  GeoTiffHandlerTest.cpp -I../../gdal/gdal-2.2.1/gcore -I../../gdal/gdal-2.2.1/port -I../../gdal/gdal-2.2.1/ogr GeoTiffHandler.o readInTiff.o ../../gdal/gdal-2.2.1/.libs/libgdal.so -ltiff 

pcs.csv: ../../gdal/gdal-2.2.1/data/pcs.csv
	cp  ../../gdal/gdal-2.2.1/data/pcs.csv .
