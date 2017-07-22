#include "tile_worker.h"
#include "global.h"


void tile_worker::set_angle(double angle)
{
  current_angle=angle;
}

void tile_worker::calc_optimal_vector()
{
  cout << "Current angle is "<<current_angle<<endl;

  inc_x=-sin(current_angle*PI/180);
  inc_y=cos(current_angle*PI/180);

  if (fabs(inc_x) < IMPRECISION)
    inc_x=0.0;
  if (fabs(inc_y) < IMPRECISION)
    inc_y=0.0;
  cout << "inc x is "<<inc_x<<endl;
  cout << "inc y is "<<inc_y<<endl;  
  orth_x=-sin((current_angle+90.0)*PI/180.0);
  orth_y= cos((current_angle+90.0)*PI/180.0);
  needed_points_in_a_row=ceil((double) landing_plane_length/sqrt(pow(((double) resolution_x*inc_x),2)+pow(((double) resolution_y*inc_y),2)));
  allowed_diff=short_range_slope*sqrt(pow(resolution_x*inc_x,2)+pow(resolution_y*inc_y,2))/100.0;
}

void tile_worker::calc_start_coordinates()
{
  if (current_angle >= 0.0 && current_angle < 90.0)  
  {
    startx=0;
    starty=0;

  } 
  else if (current_angle >= 90.0 && current_angle < 180.0)  {
        startx=tile->width.x-1;
        starty=0;

  }
  else if (current_angle == 180.0) 
  {
         startx=0;
        starty=tile->width.y-1;

  }
  else if (current_angle > 180.0 && current_angle < 315.0)
  {
    startx=0;
    starty=0;
  }
  else if (current_angle >= 315.0)
  {
    startx=0;
    starty=tile->width.y-1;
  }
  
}


int tile_worker::check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row, const int &current_x, const int &current_y)
{
   if (current_in_a_row>needed_points_in_a_row)  
   {
     cout << "Landebahn gefunden "<<start_point.x<< "und " <<start_point.y <<" bis "<<current_x<<" und "<<current_y<<" current in row "<<current_in_a_row<<" und needed" <<needed_points_in_a_row<<endl;
     current_in_a_row=0;
   }
  return 0;
}

/*! \brief constructor of tile_worker
 *   
 *
 *  This constructor is the default constructor which defines all class member variables to default
 */
tile_worker::tile_worker()
{
   tile = NULL;
   resolution_y=0;
   resolution_x=0;
}


tile_worker::~tile_worker()
{
  delete(tile);
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


/*! \brief set parameter x resolution of map
 *   
 *
 *  The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */

void tile_worker::set_x_resolution(double resolution_x)
{
  this->resolution_x=resolution_x;
}


/*! \brief set parameter x resolution of map
 *   
 *
 * The resolution of the geo tiff has to be given to the worker so that it can make appropriate adaptions to slope parameters
 */

void tile_worker::set_y_resolution(double resolution_y)
{
  this->resolution_y=resolution_y;
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
    for(int x = 0; x < tile->width.x; x++)
    {
      float temp = access_single_element(x,y);
     if (temp > -32767)
      cout << temp<<endl;
    }
}


/*! \brief single map element retrieval
 *   
 *
 *   This function retrieves the element with the requested x and y coordinate. If requeted element is outside the map then <float>::min() is returned. 
 */
float tile_worker::access_single_element(int x, int y)
{
  if (tile->width.x*y+x < tile->width.x*tile->width.y)
    return(tile->buf[tile->width.x*y+x]);
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
                           set_angle(0);
                           check_steigungen(1);
                           set_angle(45);
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
                           check_steigungen(8);
  return;
}

void tile_worker::set_landing_plane_length(double landing_plane_length)
{
 this->landing_plane_length=landing_plane_length;
}

void tile_worker::set_short_range_slope(double short_range_slope)
{
 this->short_range_slope=short_range_slope;
}

void tile_worker::set_long_range_slope(double long_range_slope)
{
 this->long_range_slope=long_range_slope;
}

void tile_worker::set_not_defined(double* not_defined)
{
  this->not_defined=not_defined;
}



/*! \brief central function for checking conditions
 *   
 *
 *   This function is currently realized by a simple approach with 8 given orientations (45° stepwise). It has included some internal validations for debug reasons. This is still under construction.
 */

void tile_worker::check_steigungen(const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */)
{

  calc_optimal_vector();
  calc_start_coordinates(); 
/*  int inc_x=0;
  int inc_y=0;
  int startx;
  int starty;

  int orth_x=0;
  int orth_y=0;*/
  
  int allowed_diff=0;
//  int needed_points_in_a_row=0;

  cout << "slope "<<short_range_slope<<" and reso " << resolution_y<<endl;

/*
  switch (direction){
    case 1:
        /*startx=0;
        starty=0;
        inc_x=0;
        inc_y=1;
        orth_x=1;
        orth_y=0;
        allowed_diff=short_range_slope*resolution_y/100.0;
//        needed_points_in_a_row=ceil((double) landing_plane_length/(double) resolution_y);
        break;
    case 2:
        /*startx=0;
        starty=0;
        inc_x=-1;
        inc_y=1;
        orth_x=1;
        orth_y=-1;
        allowed_diff=short_range_slope*sqrt(pow(resolution_y,2)+pow(resolution_x,2))/100.0;
  //      needed_points_in_a_row=ceil((double) landing_plane_length/ (double) sqrt(pow(resolution_y,2)+pow(resolution_x,2)));
        break;
    case 3:
       /* startx=tile->width.x-1;
        starty=0;
        inc_x=-1;
        inc_y=0;
        orth_x=0;
        orth_y=1;
        allowed_diff=short_range_slope*resolution_x/100.0;
  //      needed_points_in_a_row=ceil((double) landing_plane_length/(double) resolution_x);
        break;
    case 4:
        /*startx=tile->width.x-1;
        starty=0;
        inc_x=-1;
        inc_y=-1;*/
        /*orth_x=1;
        orth_y=-1;
        allowed_diff=short_range_slope*sqrt(pow(resolution_y,2)+pow(resolution_x,2))/100.0; 
 //       needed_points_in_a_row=ceil((double) landing_plane_length/ (double) sqrt(pow(resolution_y,2)+pow(resolution_x,2)));
        break;
    case 5:
        /*startx=0;
        starty=tile->width.y-1;
        inc_x=0;
        inc_y=-1;
        orth_x=1;
        orth_y=0;
        allowed_diff=short_range_slope*resolution_y/100.0;
   //      needed_points_in_a_row=ceil((double) landing_plane_length/(double) resolution_y);
         break;
    case 6:
        /*startx=0;
        starty=0;
        inc_x=1;
        inc_y=-1;
        orth_x=1;
        orth_y=1;
        allowed_diff=short_range_slope*sqrt(pow(resolution_y,2)+pow(resolution_x,2))/100.0; 
  //      needed_points_in_a_row=ceil((double) landing_plane_length/(double) sqrt(pow(resolution_y,2)+pow(resolution_x,2)));
        break;
    case 7:
        /*startx=0;
        starty=0;
        inc_x=1;
        inc_y=0;
        orth_x=0;
        orth_y=1;
        allowed_diff=short_range_slope*resolution_x/100.0;
  //      needed_points_in_a_row=ceil((double) landing_plane_length/(double) resolution_x);
        break;
    case 8:
        /*startx=0;
        starty=tile->width.y-1;
        inc_x=1;
        inc_y=1;
        orth_x=1;
        orth_y=-1;
        allowed_diff=short_range_slope*sqrt(pow(resolution_y,2)+pow(resolution_x,2))/100.0; 
  //      needed_points_in_a_row=ceil((double) landing_plane_length/(double) sqrt(pow(resolution_y,2)+pow(resolution_x,2)));
        break;
    default:
        return ; 
  }
*/
  cout << "allowed short range diff "<< allowed_diff<<endl;
  cout << "needed points in a row"<<needed_points_in_a_row<<endl;
  cout << "startx "<<startx<<", starty "<<starty<<endl;
  cout << "inc_x "<<inc_x<<", inc_y "<<inc_y<<endl;
  cout << "orth_x "<<orth_x<<", orth_y "<<orth_y<<endl;
  int completed=0;

  int current_in_a_row=0;
  double i=startx;
  double j=starty;
  int checksum = 0;
  
  int previous_x=0;
  int previous_y=0;
  int previous_valid =0;
  int accept_orthogonal_slope=1000;
  while (! completed)
  {
    ++checksum;
//    printf("aktuell %d und %d mit %lf\n",i,j,access_single_element(i,j));
   

    if (not_defined == NULL || *not_defined != access_single_element(i,j)) 
    {
    if (previous_valid)
    {
 //     printf("compare point %d,%d with %d,%d\n",i,j,previous_x,previous_y);
      if (fabs(access_single_element(i,j) - access_single_element(i,j)) < short_range_slope)
      {
   //     printf("accept  sh.sl. %lf und %lf\n",access_single_element(i,j),access_single_element(previous_x,previous_y));
        int new_x=i+orth_x;
        int new_y=j+orth_y;
        int ok=1;
        if (((new_x>=0) && (new_x<tile->width.x)) && ((new_y>=0) && (new_y < tile->width.y)))
          if (fabs(access_single_element(i,j)-access_single_element(new_x,new_y)) > accept_orthogonal_slope)
            ok=0;
        if (ok)
        {
          new_x=i-orth_x;
          new_y=j-orth_y;
          if (((new_x>=0) && (new_x<tile->width.x)) && ((new_y>=0) && (new_y < tile->width.y)))
            if (fabs(access_single_element(i,j)-access_single_element(new_x,new_y)) > accept_orthogonal_slope)
              ok=0;

        }
        if (ok)
        {
          if (current_in_a_row==0)
          {
             //save_point_forlater
             start_point.x=i;
             start_point.y=j;
          }
          ++current_in_a_row;
     //     printf("Ein fertiger Punkt ist %d, %d\n",i,j);  
        }
        if(!ok)
        {
          check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
        }
      }
      else
      {
        check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
    //    printf("not accept sh.sl. %lf und %lf\n",access_single_element(i,j),access_single_element(previous_x,previous_y)); 
      }
    }
      previous_valid=1;
    }
    else
    {
     //current point not def
     previous_valid=0;
     current_in_a_row=0;
    }
    previous_x=i;
    previous_y=j;
    i +=inc_x;
    j +=inc_y;

  // cout << "point "<<i<<","<<j<<endl;
   
   if (direction == 1)
   {
     if (j >= tile->width.y)
     {
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       i++;
       j=0;
       previous_valid=0;
     }
   }
   
   if (direction == 2)
   {
     if ((i<0) || (j>=tile->width.y))
     {
       if (startx < tile->width.x -1)
         startx++;
       else
         starty++;

       i=startx;
       j=starty;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       previous_valid=0;
     }
   }

   if (direction == 3)
   {
     if (i<0)
     {
       i=startx;
       j++;
       previous_valid=0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
     }
   }


   if (direction == 4)
   {
     if ((i<0) || (j < 0))
     {
       if (starty < tile->width.y  -1)
         ++starty;
       else
         --startx;

       i=startx;
       j=starty;
       previous_valid=0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
     }
   }

   if (direction == 5)
   {
     if (j < 0)
     {
       j=tile->width.y-1;
       ++i;
       previous_valid=0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
     }
   }

   if (direction == 6)
   {
     if ((i>tile->width.x -1) || (j < 0))
     {
       if (starty < tile->width.y  -1)
         ++starty;
       else
         ++startx;

       i=startx;
       j=starty;
       previous_valid=0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
     }

   }
   
   if (direction == 7)
   {
     if (i > tile->width.x -1)
     {
       i=0;
      check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       ++j;
     }
   }

   if (direction == 8)
   {
     if ((i> tile->width.x -1) || (j > tile->width.y -1))
     {
       if (starty > 0)
         --starty;
       else
         ++startx;

       i=startx;
       j=starty;
       previous_valid=0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
     }

   }

   //completed check

     if( i < 0)
       completed =1;
  
      if (i >= tile->width.x)
        completed = 1;

      if (j >= tile->width.y)
        completed=1;
  }

  printf("Checksum is %d\n",checksum);
 
  if (checksum == tile->width.x*tile->width.y) 
   printf("OK\n");
  else
    printf("FATAL\n");
  return;
}
