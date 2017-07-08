all: gcore/gdal_priv.h



gcore/gdal_priv.h: gdal221.zip gdal-2.2.1/configure gcore/gdal_priv.h


gdal221.zip:
	wget -O gdal221.zip http://download.osgeo.org/gdal/2.2.1/gdal221.zip
	touch $@

gdal-2.2.1/configure: gdal221.zip
	unzip -u gdal221.zip
	cd ./gdal-2.2.1; ./configure



gcore/gdal_priv.h:
	cd ./gdal-2.2.1; $(MAKE)
