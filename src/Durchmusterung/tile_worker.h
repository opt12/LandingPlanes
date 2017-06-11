extern "C" {
#include "readInTiff.h"
}

#include <iostream>
#include <limits> 

using namespace std;

class tile_worker{

  private:
   extractionParameters* eparam;
   tileCharacteristics* tile;
   float access_single_element(int x, int y);
    
  public:
    tile_worker();
    void set_param_and_tile(extractionParameters* param_in, tileCharacteristics* tile_in);
    void print_out_map();
    void check_element_access();
};
