extern "C" {
#include "readInTiff.h"
}

#include <iostream>
#include <limits> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include "GeoTiffHandler.h"

using namespace std;

class tile_worker{

  private:
   tileData* tile;
   float access_single_element(int x, int y);
   void check_steigungen(const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */);
   double resolution_x;
   double resolution_y;
   double landing_plane_length;
   double short_range_slope;
   double long_range_slope;
   double* not_defined;
   int check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row);   
 
  public:
    tile_worker();
    void set_param_and_tile( tileData* tile_in);
    void print_out_map();
    void check_element_access();
    void durchmustere_kachel();
    void set_x_resolution(double resolution_x);
    void set_y_resolution(double resolution_y);
    void set_landing_plane_length(double landing_plane_length);
    void set_short_range_slope(double short_range_slope);
    void set_long_range_slope(double long_range_slope);
    void set_not_defined(double* not_defined);
};
