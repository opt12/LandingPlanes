all: gcore/gdal_priv.h



gcore/gdal_priv.h: gdal223.zip gdal-2.2.3/configure gcore/gdal_priv.h


gdal223.zip:
	wget -O gdal223.zip http://download.osgeo.org/gdal/2.2.3/gdal223.zip
	touch $@

gdal-2.2.3/configure: gdal223.zip
	unzip -u gdal223.zip
	cd ./gdal-2.2.3; ./configure



gcore/gdal_priv.h:
	cd ./gdal-2.2.3; $(MAKE)
