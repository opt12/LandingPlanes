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

#include "json.hpp"
#include "1597_ipc_listener.h"
#include <semaphore.h>  /* required for semaphores */

using namespace std;

int search_for_planes(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler, float heading, float minLength, float width, int commSocket,const json *taskDescription, float noDataValue , rectSize pixelSize );

class tile_worker{

  private:
  int own_tile;
  void report(string report);
   int commSocket;
   double current_angle;
   const tileData* tile;
   float access_single_element(int x, int y);
   void check_steigungen(/*const int direction*//*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */);
   double resolution_x;
   double resolution_y;
   double landing_plane_length;
   double short_range_slope;
   double long_range_slope;
   double* not_defined;
   int check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row,const int &x, const int &y,vector< pair<int,int> > & coordlist,pixelPair start_point);   
   void calc_optimal_vector(); 
   double inc_x;
   double inc_y;
   void calc_start_coordinates();
   double startx;
   double starty;
   double current_x;
   double current_y;
   int still_needed();
   double orth_x;
   double orth_y; 
   int needed_points_in_a_row;
   int needed_orthogonal_points_in_a_row;
   double allowed_diff;
   double allowed_orthogonal_diff;
   //pixelPair start_point;
   //pixelPair end_point;
   double orthogonal_slope;
   double width_of_plane;

  void find_best_planes(vector< pair<int,int> > &coordlist);
   void create_landebahn_coord(pixelPair start_point,pixelPair end_point,string type,double actualRise, double actualVariance , double lenth_of_plane );
   GeoTiffHandler *myGeoTiffHandler;
   //vector< pair<int,int> > coordlist;
   const json *taskDescription;

    int direction;

   sem_t* count_sem;
   vector<pthread_t> threads;
   int get_start_values(double &startposx, double &startposy);
   pthread_mutex_t mutex_start_value;
   
    public:
    friend class thread_data;
    void *check_single_plane(void *x_void_ptr);
    tile_worker();
    tile_worker(const tileData* tile_in, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double angle, GeoTiffHandler* master, double width_of_plane, double orthogonal_slope, int commSocket, const json *taskDescription,  sem_t *count_sem,rectSize pixelSize); 
    ~tile_worker();
    void set_param_and_tile( tileData* tile_in);
    void set_taskDescription( const json *taskDescription);
    void set_param_and_tile (const tileData* tile_in);
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
    void set_width_of_plane(double width_of_plane);
    void set_orthogonal_slope(double orthogonal_slope);
    void set_commSocket(int commSocket);
    void set_semaphore(sem_t * count_sem);
};
