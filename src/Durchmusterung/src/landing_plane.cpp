#include "landing_plane.h"

/*! \brief constructor for a plane with predefined parameters
 *
 *
 *  This constructor creates a tile with predefined parameters
 */
landing_plane::landing_plane(double length /** [in] length of plane */, double varianz /** [in] variance of plane */, pair<int, int> startpoint /** [in] relative starting point of plane*/, pair<int, int> endpoint /** [in] relative ending point of plane */, double slope  /** [in] slope of plane */)
{
    this->length = length;
    this->varianz = varianz;
    this->startpoint = startpoint;
    this->endpoint = endpoint;
    this->slope = slope;
}

/*! \brief get starting point of plane
 *
 *
 *  This function returns the starting point of the current plane
@retval current starting point
 */
pair<int, int> landing_plane::getstartpoint()
{
    return this->startpoint;
}

/*! \brief get end point of plane
 *
 *
 *  This function returns the end point of the current plane
@retval current end point
 */
pair<int, int> landing_plane::getendpoint()
{
    return this->endpoint;
}

/*! \brief substitute plane with a new one when variance is better
 *
 *
 *  This function substitutes the current plane if the plane with new parameters has a better variance
 */
void landing_plane::check_better_varianz(double length /** [in] length of plane */, double varianz /** [in] variance of plane */, pair<int, int> startpoint /** [in] relative starting point of plane*/, pair<int, int> endpoint /** [in] relative ending point of plane */, double slope  /** [in] slope of plane */)
{
    if (varianz < this->varianz)
    {
        this->length = length;
        this->varianz = varianz;
        this->startpoint = startpoint;
        this->endpoint = endpoint;
        this->slope = slope;
    }
}

/*! \brief return current slope of plane
 *
 *
 *  This function returns the slope of the current plane
@retval slope
 */
double landing_plane::print_slope()
{
    return this->slope;
}

/*! \brief return current length of plane
 *
 *
 *  This function returns the length of the current plane
@retval length
 */
double  landing_plane::print_length()
{
    return this->length;
}

/*! \brief return current variance of plane
 *
 *
 *  This function returns the variance of the current plane
@retval variance
 */
double landing_plane::print_varianz()
{
    return this->varianz;
}

/*! \brief substitute plane with a new one when the new plane is longer
 *
 *
 *  This function substitutes the current plane if the plane with new parameters is longer
 */
void landing_plane::check_better_length(double length /** [in] length of plane */, double varianz /** [in] variance of plane */, pair<int, int> startpoint /** [in] relative starting point of plane*/, pair<int, int> endpoint /** [in] relative ending point of plane */, double slope  /** [in] slope of plane */)
{
    if (length > this->length)
    {
        this->length = length;
        this->varianz = varianz;
        this->startpoint = startpoint;
        this->endpoint = endpoint;
        this->slope = slope;
    }
}



