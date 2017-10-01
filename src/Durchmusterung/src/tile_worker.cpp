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


/*! \brief pointer to function which is given to p_thread.
 *
 *
 *  Within this function the p_thread is doing its work. First a new starting point to scan is requested by the master. If no more available, the thread is terminated. If a valid starting point is available then the thread is fetching point after point which is located in the requested plane orientation. For each point, several checks are performed e.g. slope with previous point, slope with orthogonal points, validity of data point value itself, .... if all criteria match the job parameter, then this point is added to a vector (list). This procedure is repeated until no further points are left of the current point does not fulfill the requested parameters. Then the current collected points in the vector (list) are checked whether they fulfill the min. length of the plane. If this is the case a subroutine is called for calculating the longest plane and the plane with min variance but fulfilling the min. length criteria.
@retval NULL thread is done
 */
void* thread_data::check_single_plane(void* x_void_ptr /** [in,out] pointer to the tile_worker object */)
{
    tile_worker* my_tile_worker = ((thread_data*)x_void_ptr)->my_tile_worker;
    int completed = 0;
    int current_in_a_row = 0;
    double i;
    double j;
    int checksum = 0;
    int previous_x = 0;
    int previous_y = 0;
    int previous_valid = 0;

    while (! my_tile_worker->get_start_values(i, j) ) // checks whether starting points for scanning are left
    {
        pixelPair start_point;
        vector< pair<int, int> > coordlist;

        while (!completed)
        {
            ++checksum;

            if (my_tile_worker->not_defined == NULL || *my_tile_worker->not_defined != my_tile_worker->access_single_element(i, j)) // checks whether current point has valid geo information
            {
                if (previous_valid) // checks whether it is not the first point in a row
                {
                    if (fabs(my_tile_worker->access_single_element(i, j) - my_tile_worker->access_single_element(previous_x, previous_y)) < my_tile_worker->allowed_diff) // check diff between neighboured points in plane direction
                    {
                        // now loop over all orthogonal elements
                        int ok = 1;

                        for (int k = 0; k < 2; k++) // loop in two directions 
                        {
                            double new_x = i;
                            double new_y = j;
                            double factor = pow(-1, k);

                            for (int l = 0; l < my_tile_worker->needed_orthogonal_points_in_a_row; l++) // loop over all needed orthogonal points
                            {
                                double old_x = new_x;
                                double old_y = new_y;
                                new_x += factor * my_tile_worker->orth_x;
                                new_y += factor * my_tile_worker->orth_y;

                                if (((new_x >= 0) && (new_x < my_tile_worker->tile->width.x)) && ((new_y >= 0) && (new_y < my_tile_worker->tile->width.y))) // vaild range check
                                {
                                    if (fabs(my_tile_worker->access_single_element(new_x, new_y) - my_tile_worker->access_single_element(old_x, old_y)) > my_tile_worker->allowed_orthogonal_diff) // check for orthogonal diff
                                    {
                                        ok = 0;
                                    }

                                    if (fabs(my_tile_worker->access_single_element(new_x, new_y) - my_tile_worker->access_single_element(new_x - my_tile_worker->inc_x, new_y - my_tile_worker->inc_y)) > my_tile_worker->allowed_diff) // check for diff in plane direction for all orthogonal neighbours
                                    {
                                        ok = 0;
                                    }
                                }
                                else
                                {
                                    ok = 0;
                                }
                            }
                        }

                        if (ok)
                        {
                            if (current_in_a_row == 0)
                            {
                                //save_point_for later
                                start_point.x = i;
                                start_point.y = j;
                            }

                            ++current_in_a_row;
                            coordlist.push_back(make_pair(i, j)); // add current point
                        }

                        if (!ok) // current point is not valid but it might be that current coordlist already has enough valid points
                        {
                            my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                        }
                    }
                    else  // current point is not valid but it might be that current coordlist already has enough valid points
                    {
                        my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point); 
                    }
                }

                previous_valid = 1;
            }
            else
            {
                //current point not def
                my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point); 
            }

            // move point forward
            previous_x = i;
            previous_y = j;
            i += my_tile_worker->inc_x;
            j += my_tile_worker->inc_y;

            
            // now we have to check whether the new point is still in the range of valid tile. If not collected points have to be checked to have a potential valid plane
            if (my_tile_worker->direction == 1)
            {
                if (j >= my_tile_worker->tile->width.y)
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 2)
            {
                if ((i < 0) || (j >= my_tile_worker->tile->width.y))
                {
                    my_tile_worker-> check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 3)
            {
                if (i < 0)
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 4)
            {
                if ((i < 0) || (j < 0))
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 5)
            {
                if (j < 0)
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 6)
            {
                if ((i > my_tile_worker->tile->width.x - 1) || (j < 0))
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 7)
            {
                if (i > my_tile_worker->tile->width.x - 1)
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }

            if (my_tile_worker->direction == 8)
            {
                if ((i > my_tile_worker->tile->width.x - 1) || (j > my_tile_worker->tile->width.y - 1))
                {
                    my_tile_worker->check_current_landebahn(current_in_a_row, my_tile_worker->needed_points_in_a_row, i, j, coordlist, start_point);
                    break;
                }
            }
        }
    }

    // here the outer loop quits which indicates that no starting points are left
    sem_post(my_tile_worker->count_sem);
    /* the function must return something - NULL will do */
    return NULL;
}

/*! \brief find longest valid and plane with minimal variance within the list
 *
 *
 *  This function iterates over the vector of valid points to find two planes. The first one is the longest possible plane which fulfills the maximal allowed slope. The second one is the one with the minimal variance but still fulfilling the maximal allowed slope 
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
    this->commSocket = commSocket;
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

tile_worker::tile_worker(const tileData* tile_in, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double angle, GeoTiffHandler* master, double width_of_plane, double orthogonal_slope, int commSocket, const json* taskDescription, sem_t* count_sem, rectSize  pixelSize )
{
    own_tile = 0;
    set_param_and_tile(tile_in);
    // cout << "before call to worker"<<endl;
    set_x_resolution(pixelSize.x);
    set_y_resolution(pixelSize.y); // ask Felix how to retrieve this information from tiff
    set_landing_plane_length(landing_plane_length);
    // cout << "hier ist slope "<<short_range_slope<<endl;
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
    // report("size is "+floattostring(tile_in->width.x)+" uind " +floattostring(tile_in->width.y));
    mutex_start_value = PTHREAD_MUTEX_INITIALIZER;
    current_x = 0;
    current_y = 0;
}

void tile_worker::set_GeoTiffHandler(GeoTiffHandler* master)
{
    myGeoTiffHandler = master;
}

void tile_worker::create_landebahn_coord(pixelPair start_point, pixelPair end_point, string type, double actualRise, double actualVariance, double length_of_plane)
{
    //cout << "start point "<<start_point.x<<" und " <<start_point.y<<endl;
    pixelCoord pixstart = { tile->offset.x + start_point.x, tile->offset.y + start_point.y};
    //geoCoord start = myGeoTiffHandler->pixel2Geo( pixstart);
    //cout <<"lb start"<<start<<endl;
    pixelCoord pixend;
    pixend.x = tile->offset.x + end_point.x;
    pixend.y = tile->offset.y + end_point.y;
    //cout << "end point "<<end_point.x<<" und " <<end_point.y<<endl;
    //geoCoord end = myGeoTiffHandler->pixel2Geo( pixend);
    //cout << "lb end "<<end<<endl;
    //report("width of plane is "+floattostring(width_of_plane));
    json j = myGeoTiffHandler->getGeoJsonPolygon(pixstart, pixend, width_of_plane / 2.0);
    j["properties"] = (*taskDescription)["scanParameters"];
    j["properties"]["actualLength"] = length_of_plane;
    j["properties"]["actualRise"] = actualRise;
    j["properties"]["actualVariance"] = actualVariance;
    j["properties"]["actualHeading"] = current_angle;
    j["properties"]["mergeable"] = type;
    emitReceiptMsg(commSocket, "landingPlane", j);
}

void tile_worker::set_orthogonal_slope(double orthogonal_slope)
{
    this->orthogonal_slope = orthogonal_slope;
}

void tile_worker::set_width_of_plane(double width_of_plane)
{
    this->width_of_plane = width_of_plane;
}

void tile_worker::set_angle(double angle)
{
    double shift = 0;
    double newangle = angle + ceil( (-angle + shift) / 360.0 ) * 360.0;
    current_angle = newangle;
    // report("Angle is "+floattostring(current_angle));
}

void tile_worker::calc_optimal_vector()
{
    // cout << "Current angle is "<<current_angle<<endl;
    inc_x = -sin(current_angle * PI / 180);
    inc_y = cos(current_angle * PI / 180);

    if (fabs(inc_x) < IMPRECISION)
        inc_x = 0.0;

    if (fabs(inc_y) < IMPRECISION)
        inc_y = 0.0;

    //  cout << "inc x is "<<inc_x<<endl;
    //  cout << "inc y is "<<inc_y<<endl;
    // report("inc x is "+floattostring(inc_x));
    //report("inc y is "+floattostring(inc_y));
    orth_x = -sin((current_angle + 90.0) * PI / 180.0);
    orth_y = cos((current_angle + 90.0) * PI / 180.0);

    if (fabs(orth_x) < IMPRECISION)
        orth_x = 0.0;

    if (fabs(orth_y) < IMPRECISION)
        orth_y = 0.0;

    needed_points_in_a_row = ceil((double) landing_plane_length / sqrt(pow(((double) resolution_x * inc_x), 2) + pow(((double) resolution_y * inc_y), 2)));
    allowed_diff = short_range_slope * sqrt(pow(resolution_x * inc_x, 2) + pow(resolution_y * inc_y, 2)) / 100.0;
    allowed_orthogonal_diff = orthogonal_slope * sqrt(pow(resolution_x * orth_x, 2) + pow(resolution_y * orth_y, 2)) / 100.0;
    // cout << "allowed from "<<short_range_slope << " and " <<resolution_x<< " and incx " <<inc_x<<" and res y" <<resolution_y <<" and inc_y 2"<<inc_y<<endl;
    needed_orthogonal_points_in_a_row = ceil(0.5 * (double) width_of_plane / sqrt(pow(((double) resolution_x * orth_x), 2) + pow(((double) resolution_y * orth_y), 2)));
    //report("orthogonal points in a row are "+floattostring(needed_orthogonal_points_in_a_row));
}

void tile_worker::calc_start_coordinates()
{
    //report("calc coord with angle "+floattostring(current_angle));
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


int tile_worker::check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row, const int &current_x, const int &current_y, vector< pair<int, int> > &coordlist, pixelPair start_point)
{
    if (coordlist.size() > 1)
    {
        /*
          if (sqrt(pow(((coordlist[0].first-coordlist.back().first)*resolution_x),2)+pow(((coordlist[0].second-coordlist.back().second)*resolution_y),2)) >= landing_plane_length)
        //   if (current_in_a_row>needed_points_in_a_row)
           {
        //     cout << "Landebahn gefunden "<<start_point.x<< "und " <<start_point.y <<" bis "<<current_x<<" und "<<current_y<<" current in row "<<current_in_a_row<<" und needed" <<needed_points_in_a_row<<endl;
        //      for (int i=0; i < coordlist.size(); i++)
        //        cout << i <<" => "<<access_single_element(coordlist[i].first,coordlist[i].second)<<'\n';
        //      for (std::map<int,double>::iterator it=coordlist.begin(); it!=coordlist.end(); ++it)
         //       std::cout << it->first << " => " << it->second << '\n';
             report("Landebahn gefunden wirklich");
             pixelPair end_point;
             end_point.x=current_x;
             end_point.y=current_y;
            create_landebahn_coord(start_point,end_point);
           }*/
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
}


tile_worker::~tile_worker()
{
    if (own_tile)
        delete (tile);
}



/*! \brief set parameter and tile characteristics
 *
 *
 *  The pointer member variables are set to the instances created by the tile manager
 */
void tile_worker::set_param_and_tile(tileData* tile_in)
{
    tile = tile_in;
}

void tile_worker::set_param_and_tile(const tileData* tile_in)
{
    tile = tile_in;
}


/*! \brief set parameter x resolution of map
 *
 *
 *  The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */

void tile_worker::set_x_resolution(double resolution_x)
{
    this->resolution_x = resolution_x;
}


/*! \brief set parameter x resolution of map
 *
 *
 * The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */

void tile_worker::set_y_resolution(double resolution_y)
{
    this->resolution_y = resolution_y;
}





/*! \brief DEBUG - print out the whole map in horizontal order
 *
 *
 *  For debug reasons this function can be used for printing out the z-coordinates. Only values larger than -32767 are printed.
 */
void tile_worker::print_out_map()
{
    /*for (int i=0; i < eparam->requestedwidth*eparam->requestedlength; i++)
    if (tile->buf[i] > -32767)
      cout << tile->buf[i]<<endl;*/
}

//debug function
/*! \brief DEBUG - print out the whole map in horizontal way but with single element retrieval.
 *
 *
 *  For debug reasons this function can be used for printing out the z-coordinates. Only values larger than -32767 are printed via single element retrieval.
 */
void tile_worker::check_element_access()
{
    for (int y = 0; y < tile->width.y; y++)
        for (int x = 0; x < tile->width.x; x++)
        {
            float temp = access_single_element(x, y);

            if (temp > -32767)
                cout << temp << endl;
        }
}


/*! \brief single map element retrieval
 *
 *
 *   This function retrieves the element with the requested x and y coordinate. If requeted element is outside the map then <float>::min() is returned.
 */
float tile_worker::access_single_element(int x, int y)
{
    if (tile->width.x * y + x < tile->width.x * tile->width.y)
        return (tile->buf[tile->width.x * y + x]);
    else
        return numeric_limits<float>::min();
}



/*! \brief DEBUG wrapper for all 8 orientations
 *
 *
 *   This function is a simple wrapper to check the check_steigungen function for all 8 possible dimensions
 */
void tile_worker::durchmustere_kachel()
{
    //report("durchmustere kacheln\n");
    //  set_angle(0);
    check_steigungen(/*1*/);
    /*set_angle(45);
    check_steigungen(2);
    set_angle(90);
    check_steigungen(3);
    set_angle(135);
    check_steigungen(4);
    set_angle(180);
    check_steigungen(5);
    set_angle(225);
    check_steigungen(6);
    set_angle(270);
    check_steigungen(7);
    set_angle(315);
    check_steigungen(8);*/
    return;
}

void tile_worker::set_landing_plane_length(double landing_plane_length)
{
    this->landing_plane_length = landing_plane_length;
}

void tile_worker::set_short_range_slope(double short_range_slope)
{
    this->short_range_slope = short_range_slope;
}

void tile_worker::set_long_range_slope(double long_range_slope)
{
    this->long_range_slope = long_range_slope;
}

void tile_worker::set_not_defined(double* not_defined)
{
    this->not_defined = not_defined;
}



/*! \brief central function for checking conditions
 *
 *
 *   This function is currently realized by a simple approach with 8 given orientations (45° stepwise). It has included some internal validations for debug reasons. This is still under construction.
 */

void tile_worker::check_steigungen(/*const int direction*/ /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */)
{
    calc_optimal_vector();
    calc_start_coordinates();
    /*  int inc_x=0;
      int inc_y=0;
      int startx;
      int starty;

      int orth_x=0;
      int orth_y=0;*/
    //  int allowed_diff=0;
    //  int needed_points_in_a_row=0;
    current_x = startx;
    current_y = starty;

    while (still_needed())
    {
        //cout << "before sem"<<endl;
        sem_wait (count_sem);
        //cout << "after sem"<<endl;
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

    //cout << "slope "<<short_range_slope<<" and reso " << resolution_y<<endl;
    return;
}

