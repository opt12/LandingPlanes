extern "C" {
#include "readInTiff.h"
}

#include <string>
#include <iostream>
#include <map>
#include "tile_worker.h"

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
    void get_tile_array( tile_worker& worker_in, int xmin, int ymin, int width, int length); 
    string tiff_input_file; 
    map <extractionParameters, tileCharacteristics*,cmpByStruct > global_map_map;
    
  public:
    tile_manager();
    tile_manager(string source_file_in);
    void get_tile(tile_worker& worker_in, int xmin, int ymin, int width, int length);

};
