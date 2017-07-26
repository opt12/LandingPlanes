extern "C" {
#include "readInTiff.h"
}

#include <iostream>
#include <limits> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include "GeoTiffHandler.h"

#define PI 3.14159265

using namespace std;

class tile_worker{

  private:
   double current_angle;
   tileData* tile;
   float access_single_element(int x, int y);
   void check_steigungen(const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */);
   double resolution_x;
   double resolution_y;
   double landing_plane_length;
   double short_range_slope;
   double long_range_slope;
   double* not_defined;
   int check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row,const int &x, const int &y);   
   void calc_optimal_vector(); 
   double inc_x;
   double inc_y;
   void calc_start_coordinates();
   double startx;
   double starty;
   double orth_x;
   double orth_y; 
   int needed_points_in_a_row;
   double allowed_diff;
   pixelPair start_point;
   pixelPair end_point;

   void create_landebahn_coord();
   GeoTiffHandler *myGeoTiffHandler;
   map<int,double> coordlist;

  public:
    tile_worker();
    ~tile_worker();
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
    void set_angle(double angle);
    void set_GeoTiffHandler(GeoTiffHandler* master);
};
