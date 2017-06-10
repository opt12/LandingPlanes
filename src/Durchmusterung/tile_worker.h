extern "C" {
#include "readInTiff.h"
}

#include <iostream>

using namespace std;

class tile_worker{

  private:
   extractionParameters* eparam;
   tileCharacteristics* tile;
    
  public:
    tile_worker();
    void set_param_and_tile(extractionParameters* param_in, tileCharacteristics* tile_in);
    void print_out_map();
};
