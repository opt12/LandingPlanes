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


struct cmpByStruct {
    bool operator()(const extractionParameters& a, const extractionParameters& b) const {
        if (a.requestedxmin < b.requestedxmin)
          return true;
        else if (a.requestedxmin == b.requestedxmin) 
          if (a.requestedymin < b.requestedymin)
            return true;
          else if  (a.requestedymin == b.requestedymin)
            if (a.requestedwidth < b.requestedwidth)
              return true;
            else if (a.requestedwidth == b.requestedwidth)
              if(a.requestedlength < b.requestedlength)
                return true;


        return false;
    }
};



class tile_manager{

  private:
    void get_tile_array( tile_worker* worker_in, int idxX, int idxY); 
    string tiff_input_file;
    double landing_plane_length;
    double angle_increment;
    double start_angle_of_plane;
    double current_angle;
    double short_range_slope;
    double long_range_slope; 
    map <extractionParameters, tileCharacteristics*,cmpByStruct > global_map_map;
    double* not_defined;
    GeoTiffHandler myGeoTiffHandler;
    datasetInfo info;  
    tilingCharacteristics *tileChar;  
 
  public:
    tile_manager();
    ~tile_manager();
    tile_manager(string source_file_in, double landing_plane_length, double short_range_slope, double long_range_slope,double* not_definied,double start_angle_of_plane, double angle_increment);
    void get_tile(tile_worker* worker_in, int idxX, int idxY);
    int init_geo_handler();
    int select_area(int xmin, int xmax, int ymin, int ymax);
   int get_tiles_X();
    int get_tiles_Y();
};
