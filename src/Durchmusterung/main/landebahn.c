/** @file landebahn.c */

#include <stdlib.h>
#include <stdio.h>
#include "tile_manager.h"
#include "error.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


/*! \brief check whether a given file is accessible for reading
 *
 *
 * This function checks whether the given input file is readable
@retval 1 file can be opened
@retval 0 file not accessible
 */
int file_readable(string infile /** [in] file to be checked  */)
{
    ifstream my_file(infile.c_str());

    if (my_file.good())
    {
        my_file.close();
        return 1;
    }
    else
        return 0;
}


/*! \brief main function for isolated test programm
 *
 *
 * This function checks whether the given input file is readable
@param [in] -E <value> tiff file
@param [in] -L <value> min length of plane in [m]
@param [in] -S <value> max short range slope in percent
@param [in] -T <value> max long range slope in percent
@param [in] -A <value> angle for searching
@param [in] -I <value> angle increment for searching
@param [in] -W <value> width of plane in [m]
@param [in] -O <value> orthogonal slope in percent
 */
int main(int argc, char* argv[])
{
    int arg = 1;
    string tiff_in;
    //set some defaults which will be overwritten
    double landing_plane_length = 0.0;
    double short_range_slope = 100;
    double long_range_slope = 100;
    double* not_defined = NULL;
    double start_angle_of_plane = 0;
    double angle_increment = 45.0;
    double width_of_plane = 0.0;
    double orthogonal_slope = 0.0;

    while (arg < argc && argv[arg][0] == '-')  //while arguments present starting with '-'
    {
        if (argv[arg][1] == 'E')
        {
            tiff_in = argv[arg + 1];
            ++arg;
        }
        else if (argv[arg][1] == 'L') // length of landing plane
        {
            landing_plane_length = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'S') //short range slope in percent
        {
            short_range_slope = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'T') // long range slope in percent
        {
            long_range_slope = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'A') // angle of searching
        {
            start_angle_of_plane = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'I') // angle increment of searching
        {
            angle_increment = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'W') // width of plane
        {
            width_of_plane = atof(argv[arg + 1]);
            ++arg;
        }
        else if (argv[arg][1] == 'O') // othogonal slope
        {
            orthogonal_slope = atof(argv[arg + 1]);
            ++arg;
        }

        ++arg;
    }

    if (landing_plane_length == 0.0)
    {
        cout << "Fatal Error: landing plane length not given or set to zero" << endl;
        exit(1);
    }

    if (!file_readable(tiff_in))
    {
        cout << "Fatal Error: Input geo tiff file does not exist or is not readable" << endl;
        exit(2);
    }

    // outout search parameters
    cout << "Minimum landing plane length is " << landing_plane_length << " m" << endl;
    cout << "Input file: " << tiff_in << endl;
    cout << "short range slope: " << short_range_slope << " %" << endl;
    cout << "long range slope: " << long_range_slope << " %" << endl;
    cout << "start angle (relative to S->N: " << start_angle_of_plane << endl;
    cout << "angle increment: " << angle_increment << endl;



    tile_worker* worker1;
    worker1 = new tile_worker();
    tile_manager* central_manager;
    central_manager = new tile_manager(tiff_in, landing_plane_length, short_range_slope, long_range_slope, not_defined, start_angle_of_plane, angle_increment, width_of_plane, orthogonal_slope);

    if (central_manager->init_geo_handler() != SUCCESS)
    {
        cout << "Error init geo handler" << endl;
        return LP_ERR_INIT_GEO ;
    }

    if (not_defined != NULL)
        cout << "not defined ist " << *not_defined << endl;

    central_manager->select_area(0, std::numeric_limits<int>::max(), 0, std::numeric_limits<int>::max());
    //looping over all subtiles
    for (int i = 0; i < central_manager->get_tiles_X(); i++)
        for (int j = 0; j < central_manager->get_tiles_Y(); j++)
        {
            central_manager->get_tile(worker1, i, j);
            worker1->durchmustere_kachel();
            central_manager->release_tile(i, j);
        }

    delete (worker1);
    delete not_defined;
    delete (central_manager);
    return 0;
}
