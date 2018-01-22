//#include "tile_worker.h"
#include "global.h"
#include "landing_plane.h"
#include "thread_data.h"

#include <fstream>


/*! \brief checks whether there are still untested starting points to be given to threads from the pool
 *
 *
 *  This function checks whether there are still some starting points to be tested.
 */
int tile_worker::still_needed()
{
    int still = 1; /// 1 means there are still values to be checked. Variable is used for return code
    pthread_mutex_lock( &mutex_start_value );

    if ( current_x < 0)
        still = 0;

    if ( current_y < 0)
        still = 0;

    if (current_x > tile->width.x)
        still = 0;

    if (current_y > tile->width.y)
        still = 0;

    pthread_mutex_unlock( &mutex_start_value );
    return still;
}

/*! \brief determine the next starting point for a landing plane scan
 *
 *
 *  The current x and y values are taken and the incremental x and/or y increment is added. The return code indicates whether this point is valid or if all work is done and no starting point is left.
@retval 0 valid point.
@retval 1 no point left.
 */
int tile_worker::get_start_values(double &startposx /** [in,out] reference to the x coordinate of starting point  */, double &startposy /** [in,out] reference to the y coordinate of starting point  */)
{
    pthread_mutex_lock( &mutex_start_value );
    startposx = current_x;
    startposy = current_y;

    if (direction == 1)
    {
        current_x++; //since inc_x zero increment by 1
        current_y = 0;
    }

    if (direction == 2)
    {
        current_x -= inc_x;

        if ((current_x >= tile->width.x - 1))
        {
            startx = tile->width.x - 1;
            starty += inc_y;
            current_x = startx;
            current_y = starty;
        }
    }

    if (direction == 3)
    {
        current_x = startx;
        current_y += 1; //since inc_y zero increment by 1
    }

    if (direction == 4)
    {
        current_x -= inc_x;

        if ((current_x >= tile->width.x - 1) )
        {
            starty += inc_y;
            current_x = tile->width.x - 1;
            current_y = starty;
        }
    }

    if (direction == 5)
    {
        current_y = tile->width.y - 1;
        current_x += 1;; // since inc_x zero shift by 1
    }

    if (direction == 6)
    {
        current_x -= inc_x;

        if ((current_x <= 0) )
        {
            starty += inc_y;;
            current_x = 0;
            current_y = starty;
        }
    }

    if (direction == 7)
    {
        current_x = 0;
        current_y += 1; //set to 1 because inc_y is 0
    }

    if (direction == 8)
    {
        current_x -= inc_x;

        if ((current_x <= 0) )
        {
            starty += inc_y;
            current_x = 0;
            current_y = starty;
        }
    }

    pthread_mutex_unlock( &mutex_start_value );

    //completed check
    if ( current_x < 0)
        return 1;

    if ( current_y < 0)
        return 1;

    if (current_x > tile->width.x)
        return 1;

    if (current_y > tile->width.y)
        return 1;

    return 0;
}




/*! \brief find longest valid and plane with minimal variance within the list
 *
 *
 *  This function iterates over the vector of valid points to find two planes.
 *  The first one is the longest possible plane which fulfills the maximal
 *  allowed slope. The second one is the one with the minimal variance but
 *  still fulfilling the maximal allowed slope
 */
void tile_worker::find_best_planes(vector< pair<int, int> > &coordlist /** [in,out] reference to the vector of valid points describing a potential plane */)
{
    landing_plane* plane_min_varianz = NULL;
    landing_plane* plane_max_length = NULL;

    for (int i = 0; i < (int) coordlist.size(); i++) // loop over all points in list
    {
        double sum = 0;
        int count = 0;

        for (int j = i; j < (int) coordlist.size() - 1; j++) // loop over all points from current start point to end of list
        {
            sum += fabs(access_single_element(coordlist[j].first, coordlist[j].second) - access_single_element(coordlist[j + 1].first, coordlist[j + 1].second));
            ++count;
            double length;
            double slope;

            if ((length = sqrt(pow(((coordlist[i].first - coordlist[j + 1].first) * resolution_x), 2) + pow(((coordlist[i].second - coordlist[j + 1].second) * resolution_y), 2))) >= landing_plane_length) // if minimal length fulfilled
            {
                if ((slope = fabs(access_single_element(coordlist[i].first, coordlist[i].second) - access_single_element(coordlist[j + 1].first, coordlist[j + 1].second))) <= long_range_slope * length / 100.0) // if maximal slope fulfilled
                {
                    double varianz = 0;
                    double mean = sum / (double) count;

                    for (int k = i; k < j; k++) // calculate variance
                    {
                        varianz += pow(fabs(access_single_element(coordlist[k].first, coordlist[k].second) - access_single_element(coordlist[k + 1].first, coordlist[k + 1].second)) - mean, 2);
                    }

                    varianz = varianz / count;

                    // now compare to both yet best planes and substitute if current plane is better
                    if (plane_min_varianz == NULL)
                        plane_min_varianz = new landing_plane(length, varianz, make_pair(coordlist[i].first, coordlist[i].second), make_pair(coordlist[j - 1].first, coordlist[j - 1].second), slope);
                    else
                        plane_min_varianz->check_better_varianz(length, varianz, make_pair(coordlist[i].first, coordlist[i].second), make_pair(coordlist[j - 1].first, coordlist[j - 1].second), slope);

                    if (plane_max_length == NULL)
                        plane_max_length = new landing_plane(length, varianz, make_pair(coordlist[i].first, coordlist[i].second), make_pair(coordlist[j - 1].first, coordlist[j - 1].second), slope);
                    else
                        plane_max_length->check_better_length(length, varianz, make_pair(coordlist[i].first, coordlist[i].second), make_pair(coordlist[j - 1].first, coordlist[j - 1].second), slope);
                }
            }
        }
    }


    if (plane_max_length != NULL) 
    {
        // now transfer plane with max length to mongodb
        pixelPair startpoint, endpoint;
        startpoint.x = plane_max_length->getstartpoint().first;
        startpoint.y = plane_max_length->getstartpoint().second;
        endpoint.x = plane_max_length->getendpoint().first;
        endpoint.y = plane_max_length->getendpoint().second;
        create_landebahn_coord(startpoint, endpoint, "true", plane_max_length->print_slope(), plane_max_length->print_varianz(), plane_max_length->print_length());
    }

    if (plane_min_varianz != NULL)
    {
        // now transfer plane with min variacne to mongodb
        pixelPair startpoint, endpoint;
        startpoint.x = plane_min_varianz->getstartpoint().first;
        startpoint.y = plane_min_varianz->getstartpoint().second;
        endpoint.x = plane_min_varianz->getendpoint().first;
        endpoint.y = plane_min_varianz->getendpoint().second;
        create_landebahn_coord(startpoint, endpoint, "false", plane_min_varianz->print_slope(), plane_min_varianz->print_varianz(), plane_min_varianz->print_length());
    }

    // free memory
    delete plane_max_length;
    plane_max_length = NULL;
    delete plane_min_varianz;
    plane_min_varianz = NULL;
}

/*! \brief set function for taskDescription
 *
 *
 *  This function sets taskDescription of object instance
 */
void tile_worker::set_taskDescription(const json* taskDescription /** [in] pointer to taskDescription */)
{
    this->taskDescription = taskDescription;
}

/*! \brief DEBUG - writes a given string into a temp file
 *
 *
 *  For debug reasons this function can be used for writing a string into a temp file
 */
void tile_worker::report(std::string report/** [in] string to be written into temp file*/)
{
    std::ofstream outfile;
    outfile.open("/tmp/landingreport.txt", std::ios_base::app);
    outfile << report << endl;
    outfile.close();
}

/*! \brief set function for commSocket with MongoDB
 *
 *
 *  This function sets commSocket of object instance
 */
void tile_worker::set_commSocket(int commSocket /** [in] socket for MongoDB access*/)
{
    this->commSocket = new int();
    *this->commSocket = commSocket;
}

/*! \brief set function for shared semaphore for parallel execution
 *
 *
 *  This function sets the reference to the semaphore of object instance
 */
void tile_worker::set_semaphore(sem_t* count_sem /** [in] sempahore for parallel execution*/)
{
    this->count_sem = count_sem;
}

/*! \brief constructor of the tile_worker object
 *
 *
 *  This constructor initializes the tile_worker object. This is a sepecific constructor which assumes that the caller already created a tile and manages it itself. All scanning parameters are used for creating the object instance
 */
tile_worker::tile_worker(const tileData* tile_in /** [in] pointer to geo tile */, double landing_plane_length /** [in] length of landing plane in [m]*/, double short_range_slope /** [in] max allowed short range slope for neighbour tiles*/, double long_range_slope /** [in] max allowed slope between first point of plane and last point of plane */, double* not_defined /** [in] pointer with value of the undefined value. NULL if such a value does not exist*/, double angle /** [in] angle for plane orientation */, GeoTiffHandler* master /** [in] pointer to a GeoTiffHandler object to perform conversion tasks*/, double width_of_plane /** [in] width of plane in [m] */, double orthogonal_slope /** [in] max allowed slope in orthogonal direction*/, int commSocket /** [in] commSocket for communication with MongoDB */, const json* taskDescription /** [in] pointer to taskDescription object describing the current scan*/, sem_t* count_sem /** [in] pointer to parallel execution managing semaphore*/, rectSize  pixelSize /** [in] scaling information parameter for GeoTiff object */)
{
    own_tile = 0;
    set_param_and_tile(tile_in);
    set_x_resolution(pixelSize.x);
    set_y_resolution(pixelSize.y);
    set_landing_plane_length(landing_plane_length);
    set_short_range_slope(short_range_slope);
    set_long_range_slope(long_range_slope);
    set_not_defined(not_defined);
    set_angle(angle);
    set_GeoTiffHandler(master);
    set_width_of_plane(width_of_plane);
    set_orthogonal_slope(orthogonal_slope);
    set_commSocket(commSocket);
    set_taskDescription(taskDescription);
    set_semaphore(count_sem);
    mutex_start_value = PTHREAD_MUTEX_INITIALIZER;
    current_x = 0;
    current_y = 0;
}

/*! \brief set function for GeoTiffHandler pointer
 *
 *
 *  This function sets GeoTiffHandler pointer to object value
 */
void tile_worker::set_GeoTiffHandler(GeoTiffHandler* master /** [in] pointer to external GeoTiffHandler object*/)
{
    myGeoTiffHandler = master;
}

/*! \brief create a json describing a valid plane and push it to MongoDB
 *
 *
 *  This function creates a json object for a valid plane and pushs it to the MongoDB
 */
void tile_worker::create_landebahn_coord(pixelPair start_point /** [in] start point of plane*/,
		pixelPair end_point /** [in] end point of plane*/,
		string type /** [in] type of plane (mergeable means it is the longest plane and can be merged later on with other planes*/,
		double actualSlope /** [in] slope of the plane*/,
		double actualVariance /** [in] variance of the plane*/,
		double length_of_plane /** [in] length of plane in [m]*/)
{
    pixelCoord pixstart = { tile->offset.x + start_point.x, tile->offset.y + start_point.y};
    pixelCoord pixend;
    pixend.x = tile->offset.x + end_point.x;
    pixend.y = tile->offset.y + end_point.y;
    if (commSocket != NULL) //otherwise the caller is not the web UI
    {
        json j = myGeoTiffHandler->getGeoJsonPolygon(pixstart, pixend, width_of_plane / 2.0);
        j["properties"] = (*taskDescription)["scanParameters"];
        j["properties"]["actualLength"] = length_of_plane;
        j["properties"]["actualSlope"] = actualSlope;
        j["properties"]["actualVariance"] = actualVariance;
        j["properties"]["actualHeading"] = current_angle;
        j["properties"]["mergeable"] = type;
        emitReceiptMsg(*commSocket, "landingPlane", j);
     }
     else
       cout << "Found plane"<<endl;
}

/*! \brief set function for orthogonal slope
 *
 *
 *  This function sets orthogonal slope to object value
 */
void tile_worker::set_orthogonal_slope(double orthogonal_slope /** [in] max orthogonal slope*/)
{
    this->orthogonal_slope = orthogonal_slope;
}

/*! \brief set function for width of plane
 *
 *
 *  This function sets width of plane
 */
void tile_worker::set_width_of_plane(double width_of_plane /** [in] width of plane*/)
{
    this->width_of_plane = width_of_plane;
}

/*! \brief set function for angle of orientation
 *
 *
 *  This function sets angle of plane orientation. Will be trimmed to a range 0 - 360 degrees
 */
void tile_worker::set_angle(double angle /** [in] orientation angle of plane*/)
{
    double shift = 0;
    double newangle = angle + ceil( (-angle + shift) / 360.0 ) * 360.0;
    current_angle = newangle;
}

/*! \brief function for static calculation of optimal incremental vectors
 *
 *
 *  This function performs a static calculation of all needed incremental vectors. Also additional values needed during scanning of area are calculated here once. All values depend on the orientation angle
 */
void tile_worker::calc_optimal_vector()
{
 
    inc_x = -sin(current_angle * PI / 180);
    inc_y = cos(current_angle * PI / 180);

    if (fabs(inc_x) < IMPRECISION)
        inc_x = 0.0;

    if (fabs(inc_y) < IMPRECISION)
        inc_y = 0.0;

    orth_x = -sin((current_angle + 90.0) * PI / 180.0);
    orth_y = cos((current_angle + 90.0) * PI / 180.0);

    if (fabs(orth_x) < IMPRECISION)
        orth_x = 0.0;

    if (fabs(orth_y) < IMPRECISION)
        orth_y = 0.0;

    needed_points_in_a_row = ceil((double) landing_plane_length / sqrt(pow(((double) resolution_x * inc_x), 2) + pow(((double) resolution_y * inc_y), 2)));
    allowed_diff = short_range_slope * sqrt(pow(resolution_x * inc_x, 2) + pow(resolution_y * inc_y, 2)) / 100.0;
    allowed_orthogonal_diff = orthogonal_slope * sqrt(pow(resolution_x * orth_x, 2) + pow(resolution_y * orth_y, 2)) / 100.0;
    needed_orthogonal_points_in_a_row = ceil(0.5 * (double) width_of_plane / sqrt(pow(((double) resolution_x * orth_x), 2) + pow(((double) resolution_y * orth_y), 2)));
}

/*! \brief function for initialization of start point for scanning
 *
 *
 *  This function determines the ideal starting point for the area scanning. It is crucial that all potential planes are included
 */
void tile_worker::calc_start_coordinates()
{
    if (current_angle >= 0.0 && current_angle < 90.0)
    {
        startx = 0;
        starty = 0;

        if (current_angle > 0.0)
            direction = 2;
        else
            direction = 1;
    }
    else if (current_angle == 90.0 )
    {
        startx = tile->width.x - 1;
        starty = 0;
        direction = 3;
    }
    else if (current_angle > 90.0 && current_angle < 180.0)
    {
        startx = 0;
        starty = tile->width.y;
        direction = 4;
    }
    else if (current_angle == 180.0)
    {
        startx = 0;
        starty = tile->width.y - 1;
        direction = 5;
    }
    else if (current_angle > 180.0 && current_angle < 270)
    {
        startx = tile->width.x - 1;
        starty = tile->width.y - 1;
        direction = 6;
    }
    else if (current_angle == 270.0)
    {
        startx = 0;
        starty = 0;
        direction = 7;
    }
    else if (current_angle > 270.0)
    {
        startx = tile->width.x - 1;
        starty = 0;
        direction = 8;
    }
}

/*! \brief function for fundamental check whether current list of points can form
 * a plane according to requirements
 *
 *
 *  This function checks whether the current list with points can be used for
 *  searchig for landing planes. It trunactes the list afterwards
 */
int tile_worker::check_current_landebahn(int &current_in_a_row /** [in,out] number of points in a row*/, vector< pair<int, int> > &coordlist /** [in,out] list with point references*/)
{
    if (coordlist.size() > 1)
    {
        find_best_planes(coordlist);
    }

    current_in_a_row = 0;
    coordlist.clear();
    return 0;
}

/*! \brief constructor of tile_worker
 *
 *
 *  This constructor is the default constructor which defines all class member variables to default
 */
tile_worker::tile_worker()
{
    own_tile = 1;
    tile = NULL;
    resolution_y = 0;
    resolution_x = 0;
    commSocket= NULL;
    current_x= 0;
    current_y = 0;
}

/*! \brief destructor of tile_worker
 *
 *
 *  This destructor removes the object instance. If tile is instanciated from caller, it is not deleted
 */
tile_worker::~tile_worker()
{
    if (own_tile)
        delete (tile);
}



/*! \brief set parameter and tile characteristics
 *
 *
 *  The pointer member variables are set to the instances created by the caller
 */
void tile_worker::set_param_and_tile(tileData* tile_in /** [in] pointer to external tile*/)
{
    tile = tile_in;
}

/*! \brief set parameter and tile characteristics
 *
 *
 *  The pointer member variables are set to the instances created by the caller
 */
void tile_worker::set_param_and_tile(const tileData* tile_in /** [in] pointer to external tile*/)
{
    tile = tile_in;
}


/*! \brief set parameter x resolution of map
 *
 *
 *  The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */
void tile_worker::set_x_resolution(double resolution_x /** [in] resolution in x dimension*/)
{
    this->resolution_x = fabs(resolution_x);
}


/*! \brief set parameter x resolution of map
 *
 *
 * The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */

void tile_worker::set_y_resolution(double resolution_y /** [in] resolution in y dimension*/)
{
    this->resolution_y = fabs(resolution_y);
}



/*! \brief DEBUG - print out the whole map in horizontal way but with single element retrieval.
 *
 *
 *  For debug reasons this function can be used for printing out the z-coordinates. All values are printed via single element retrieval.
 */
void tile_worker::check_element_access()
{
    for (int y = 0; y < tile->width.y; y++)
        for (int x = 0; x < tile->width.x; x++)
        {
            float temp = access_single_element(x, y);
            cout << temp << endl;
        }
}


//! @cond DoNotRaiseWarning
/*! \brief single map element retrieval
 *
 *
 *   This function retrieves the element with the requested x and y coordinate. If requeted element is outside the map then <float>::min() is returned.
@retval value if valid point
@retval numeric_limits<float>\::min() if point is outside range
 */
//! @endcond
float tile_worker::access_single_element(int x /** [in] x coordinate of requested field*/, int y /** [in] y coordinate of requested field*/)
{
    if (tile->width.x * y + x < tile->width.x * tile->width.y)
        return (tile->buf[tile->width.x * y + x]);
    else
        return numeric_limits<float>::min();
}



/*! \brief public wrapper for starting the scan
 *
 *
 *   This function is a simple wrapper to call the private scan method
 */
void tile_worker::durchmustere_kachel()
{
    check_steigungen();
    return;
}

/*! \brief set landing plane length
 *
 *
 *  This function sets the requested minimal landing plane length to the instance object 
 */
void tile_worker::set_landing_plane_length(double landing_plane_length /** [in] minimal length of plane*/)
{
    this->landing_plane_length = landing_plane_length;
}

/*! \brief set maximal slope of neighboured points in plane direction
 *
 *
 *  This function sets the maximal allowed slope of neighboured points in the scanning direction
 */
void tile_worker::set_short_range_slope(double short_range_slope /** [in] maximal allowed slope in short range*/)
{
    this->short_range_slope = short_range_slope;
}

/*! \brief set maximal slope of start and endpoint in plane direction
 *
 *
 *  This function sets the maximal allowed slope of start- and endpoint in the scanning direction
 */
void tile_worker::set_long_range_slope(double long_range_slope /** [in] maximal allowed slope in long range*/)
{
    this->long_range_slope = long_range_slope;
}

/*! \brief set the undefinied value
 *
 *
 *  This function sets the undefined data value. NULL if this does not exist
 */
void tile_worker::set_not_defined(double* not_defined /** [in] undefinied data value*/)
{
    this->not_defined = not_defined;
}



/*! \brief central function for thread handling
 *
 *
 *   This function is creating threads until the semaphore is 0. This allows to control the number of parallel execution
 */
void tile_worker::check_steigungen()
{
    calc_optimal_vector();
    calc_start_coordinates();
    current_x = startx;
    current_y = starty;

    while (still_needed())
    {
        sem_wait (count_sem);
        pthread_t newthread;
        thread_data* thread_data_temp = new thread_data(this);

        if (pthread_create(&newthread, NULL, thread_data::check_single_plane, thread_data_temp))
        {
            fprintf(stderr, "Error creating thread\n");
            return;
        }

        threads.push_back(newthread);
    }

    for (int i = 0; i < (int) threads.size(); i++)
        if (pthread_join(threads[i], NULL))
        {
            fprintf(stderr, "Error joining thread\n");
            return;
        }
    threads.clear();
    return;
}

