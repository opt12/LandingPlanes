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


class tile_worker {

    private:
        int own_tile; ///< Check whether the tile has to be freed or if it is the job of the caller
        void report(string report);
        int* commSocket; ///< Pointer to socket to put results into (MongoDB)
        double current_angle; ///< current angle of plane orientation to be scanned.
        const tileData* tile; ///< reference to the tile data (Geo data , ...)
        float access_single_element(int x, int y);
        void check_steigungen();
        double resolution_x; ///< distance in [m] between two horizontal data points
        double resolution_y; ///< distance in [m] between two vertical data points
        double landing_plane_length; ///< minimum plane length in [m]
        double short_range_slope; ///< max. allowed slope between two neighbours in plane direction
        double long_range_slope; ///< max. allowed slope between first and last point of plane
        double* not_defined; ///< if a not defined value is present is has to be set here
        int check_current_landebahn(int &current_in_a_row, vector< pair<int, int> > &coordlist);
        void calc_optimal_vector();
        double inc_x; ///< incremental step size in x-dimension
        double inc_y;///< incremental step size in y-dimension
        void calc_start_coordinates();
        double startx; ///< x-dimension of start point in tile
        double starty; ///< y-dimension of start point in tile
        double current_x; ///< current x-dimension of start point
        double current_y; ///< current y-dimension of start point
        int still_needed();
        double orth_x; ///< incremental step size in orthogonal dimension (x- part)
        double orth_y; ///< incremental step size in orthogonal dimension (y- part)
        int needed_points_in_a_row; ///< number of accepted data points in a row for fulfilling minimum length condition
        int needed_orthogonal_points_in_a_row; ///< number of accepted data points in a row for fulfilling minimum width condition
        double allowed_diff; ///< maximum difference between two neighboured tiles in direction of plane
        double allowed_orthogonal_diff; ///< maximum difference between two neighboured tiles in orthogonal direction
        double orthogonal_slope; ///< slope in orthogonal direction
        double width_of_plane; ///< minimum width of plane

        void find_best_planes(vector< pair<int, int> > &coordlist);
        void create_landebahn_coord(pixelPair start_point, pixelPair end_point, string type, double actualRise, double actualVariance, double lenth_of_plane );
        GeoTiffHandler* myGeoTiffHandler; ///< pointer to GeotiffhandlerObject for GeoTiff conversions
        const json* taskDescription; ///< task identification json object for later mongoDB 
        int direction; ///< direcion describes how to set the starting point of scanning and how to step incrementally through the area. It depends on the orientation angle of the plane

        sem_t* count_sem; ///< semaphore to control the maximum number of parallel threads
        vector<pthread_t> threads; ///< vector hosting the thread instances
        int get_start_values(double &startposx, double &startposy);
        pthread_mutex_t mutex_start_value; ///< mutex for blocking the critical section for requesting new start points for scanning

    public:
        friend class thread_data;
        tile_worker();
        tile_worker(const tileData* tile_in, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double angle, GeoTiffHandler* master, double width_of_plane, double orthogonal_slope, int commSocket, const json* taskDescription,  sem_t* count_sem, rectSize pixelSize);
        ~tile_worker();
        void set_param_and_tile( tileData* tile_in);
        void set_taskDescription( const json* taskDescription);
        void set_param_and_tile (const tileData* tile_in);
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
        void set_semaphore(sem_t* count_sem);
};
