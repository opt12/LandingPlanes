extern "C" {
#include "readInTiff.h"
}

#include <string>
#include <iostream>
#include <map>
#include "tile_worker.h"
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <ogr_spatialref.h>
#include "GeoTiffHandler.h"

using namespace std;


class tile_manager {

    private:
        void get_tile_array( tile_worker* worker_in, int idxX, int idxY);
        string tiff_input_file;  ///< path to the input tiff file for scanning
        double landing_plane_length; ///< min length of plane
        double angle_increment; ///< angle increment for next scan
        double start_angle_of_plane; ///< start angle for scanning
        double current_angle; ///< current angle which area is scanned for
        double short_range_slope; ///< max. allowed slope between to neighboured fields in direction of plane
        double long_range_slope; ///< max. allowed slope between first and last plane point
        double orthogonal_slope; ///< max. allowed slope between to neighboured fields in orthogonal direction
        double width_of_plane; ///< width of plane
        double* not_defined; ///< max. definition of the not defined data value. NULL if does not exist
        GeoTiffHandler myGeoTiffHandler; ///< instance of GeoTiffHandler for data conversion
        datasetInfo info; ///< instance of datasetInfo
        tilingCharacteristics* tileChar; ///< pointer to instance of tilingCharacteristics
        tileData* tile; ///< pointer to instance of a tile
        rectSize pixelSize; ///< scale information from geo tiff file

    public:
        tile_manager();
        ~tile_manager();
        tile_manager(string source_file_in, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_definied, double start_angle_of_plane, double angle_increment, double width_of_plane, double orthogonal_slope);
        void get_tile(tile_worker* worker_in, int idxX, int idxY);
        int init_geo_handler();
        int select_area(int xmin, int xmax, int ymin, int ymax);
        int get_tiles_X();
        int get_tiles_Y();
        void release_tile(int i, int j);
};
