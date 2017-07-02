extern "C" {
#include "readInTiff.h"
}

#include <iostream>
#include <limits> 
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class tile_worker{

  private:
   extractionParameters* eparam;
   tileCharacteristics* tile;
   float access_single_element(int x, int y);
   void check_steigungen(const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */);
   double resolution_x;
   double resolution_y;
    
  public:
    tile_worker();
    void set_param_and_tile(extractionParameters* param_in, tileCharacteristics* tile_in);
    void print_out_map();
    void check_element_access();
    void durchmustere_kachel();
    void set_x_resolution(double x_resolution);
    void set_y_resolution(double y_resolution);
};
