#include "tile_worker.h"
#include "global.h"

#include <fstream>

void tile_worker::set_taskDescription(const json *taskDescription)
{
this->taskDescription = taskDescription;
}

string floattostring(double in)
{
std::ostringstream strs;
strs << in;
return strs.str();
}

void tile_worker::report(std::string report)
{
 std::ofstream outfile;

  outfile.open("/tmp/landingreport.txt", std::ios_base::app);
  outfile << report <<endl;
  outfile.close();

}

void tile_worker::set_commSocket(int commSocket)
{
  this->commSocket=commSocket;
}

tile_worker::tile_worker(const tileData* tile_in, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double angle, GeoTiffHandler* master, double width_of_plane, double orthogonal_slope, int commSocket, const json *taskDescription)
{
own_tile=0;
 set_param_and_tile(tile_in);
  cout << "before call to worker"<<endl;
report("init worker with angle "+floattostring(angle));
 set_x_resolution(20.0);
 set_y_resolution(20.0); // ask Felix how to retrieve this information from tiff
 set_landing_plane_length(landing_plane_length);
 cout << "hier ist slope "<<short_range_slope<<endl;
 set_short_range_slope(short_range_slope);
 set_long_range_slope(long_range_slope);
 set_not_defined(not_defined);
 set_angle(angle);
 set_GeoTiffHandler(master);
 set_width_of_plane(width_of_plane);
 set_orthogonal_slope(orthogonal_slope);
 set_commSocket(commSocket);
  set_taskDescription(taskDescription);
  report("size is "+floattostring(tile_in->width.x)+" uind " +floattostring(tile_in->width.y));
}

void tile_worker::set_GeoTiffHandler(GeoTiffHandler * master)
{
myGeoTiffHandler=master;


}

void tile_worker::create_landebahn_coord()
{

cout << "start point "<<start_point.x<<" und " <<start_point.y<<endl;
pixelCoord pixstart = { tile->offset.x+start_point.x, tile->offset.y+start_point.y};
 //geoCoord start = myGeoTiffHandler->pixel2Geo( pixstart);
//cout <<"lb start"<<start<<endl;
pixelCoord pixend;
pixend.x=tile->offset.x+end_point.x;
pixend.y=tile->offset.y+end_point.y;

cout << "end point "<<end_point.x<<" und " <<end_point.y<<endl;

//geoCoord end = myGeoTiffHandler->pixel2Geo( pixend);

//cout << "lb end "<<end<<endl;

json j = myGeoTiffHandler->getGeoJsonPolygon(pixstart, pixend, /*width_of_plane*/1);

std::ofstream outfile;

  outfile.open("/tmp/landingreport.txt", std::ios_base::app);
  outfile << j.dump(4)<<endl;
  outfile.close();

float lengthFromJson=3000;
//float lengthFromJson = j["properties"]["actualLength"];

//        j["properties"] = (*p.taskDescription)["scanParameters"];
       j["properties"] = (*taskDescription)["scanParameters"];

        j["properties"]["actualLength"] = lengthFromJson;
        j["properties"]["actualRise"] = 666.666;
        j["properties"]["actualVariance"] = 666.666;
        j["properties"]["actualHeading"] = current_angle;
cout << j.dump(4) << endl;

        emitReceiptMsg(commSocket, "landingPlane", j);

}

void tile_worker::set_orthogonal_slope(double orthogonal_slope)
{
  this->orthogonal_slope=orthogonal_slope;
}

void tile_worker::set_width_of_plane(double width_of_plane)
{
  this->width_of_plane=width_of_plane;
}

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

  report("inc x is "+floattostring(inc_x));
  report("inc y is "+floattostring(inc_y)); 
  orth_x=-sin((current_angle+90.0)*PI/180.0);
  orth_y= cos((current_angle+90.0)*PI/180.0);
  if (fabs(orth_x) < IMPRECISION)
    orth_x=0.0;
  if (fabs(orth_y) < IMPRECISION)
    orth_y=0.0;
  needed_points_in_a_row=ceil((double) landing_plane_length/sqrt(pow(((double) resolution_x*inc_x),2)+pow(((double) resolution_y*inc_y),2)));
  allowed_diff=short_range_slope*sqrt(pow(resolution_x*inc_x,2)+pow(resolution_y*inc_y,2))/100.0;
  allowed_orthogonal_diff=orthogonal_slope*sqrt(pow(resolution_x*orth_x,2)+pow(resolution_y*orth_y,2))/100.0;
  cout << "allowed from "<<short_range_slope << " and " <<resolution_x<< " and incx " <<inc_x<<" and res y" <<resolution_y <<" and inc_y 2"<<inc_y<<endl;
  needed_orthogonal_points_in_a_row=ceil(0.5 *(double) width_of_plane/sqrt(pow(((double) resolution_x*orth_x),2)+pow(((double) resolution_y*orth_y),2)));
}

void tile_worker::calc_start_coordinates()
{
  report("calc coord with angle "+floattostring(current_angle));
  if (current_angle >= 0.0 && current_angle < 90.0)  
  {
    startx=0;
    starty=0;
    if (current_angle > 0.0)
      direction = 2;
    else
    direction=1;
  } 
  else if (current_angle == 90.0 )  {
        startx=tile->width.x-1;
        starty=0;
    direction=3;
  }
  else if (current_angle > 90.0 && current_angle < 180.0)
  {
    startx=0;
    starty=tile->width.y;
    direction = 4;
  }
  else if (current_angle == 180.0) 
  {
         startx=0;
        starty=tile->width.y-1;
direction=5;
  }
  else if (current_angle > 180.0 && current_angle < 270)
  {
    startx=tile->width.x-1;
    starty=tile->width.y-1;
   direction = 6;
  }
  else if (current_angle == 270.0)
  {
    startx=0;
    starty=0;
    direction =7;
  }
  else if (current_angle > 270.0)
  {
    startx=tile->width.x-1;
    starty=0;
    direction = 8;
  }
  
}


int tile_worker::check_current_landebahn(int &current_in_a_row, const int &needed_points_in_a_row, const int &current_x, const int &current_y)
{
   if (current_in_a_row>needed_points_in_a_row)  
   {
     cout << "Landebahn gefunden "<<start_point.x<< "und " <<start_point.y <<" bis "<<current_x<<" und "<<current_y<<" current in row "<<current_in_a_row<<" und needed" <<needed_points_in_a_row<<endl;
      for (std::map<int,double>::iterator it=coordlist.begin(); it!=coordlist.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
     end_point.x=current_x;
     end_point.y=current_y;
    create_landebahn_coord(); 
   }
current_in_a_row=0;
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
  own_tile=1;
   tile = NULL;
   resolution_y=0;
   resolution_x=0;
}


tile_worker::~tile_worker()
{
  if (own_tile)
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

void tile_worker::set_param_and_tile(const tileData* tile_in)
{
  tile= tile_in;
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

report("durchmustere kacheln\n");
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
  cout << "allowed_orthogonal_diff" <<allowed_orthogonal_diff<<endl;
  cout << "needed points in a row"<<needed_points_in_a_row<<endl;
  cout << "needed orthogonal points in a row"<<needed_orthogonal_points_in_a_row<<endl;
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
  int accept_orthogonal_slope=1000; /* temp only*/
  while (! completed)
  {
    ++checksum;
//    printf("aktuell %d und %d mit %lf\n",i,j,access_single_element(i,j));
   

    if (not_defined == NULL || *not_defined != access_single_element(i,j)) 
    {
    if (previous_valid)
    {
 //     printf("compare point %d,%d with %d,%d\n",i,j,previous_x,previous_y);
      if (fabs(access_single_element(i,j) - access_single_element(previous_x,previous_y)) < allowed_diff)
      {
   //     printf("accept  sh.sl. %lf und %lf\n",access_single_element(i,j),access_single_element(previous_x,previous_y));
        // now loop over all orthogonal elements
        int ok=1;
        for(int k=0; k < 2; k++)
        {
          double new_x=i;
          double new_y=j;
          double factor = pow(-1,k);
        for(int l=0; l < 2; l++)
        {
          double old_x=new_x;
          double old_y=new_y;
          new_x += factor * orth_x;
          new_y += factor * orth_y;
          if (((new_x>=0) && (new_x<tile->width.x)) && ((new_y>=0) && (new_y < tile->width.y)))
          {
             if (fabs(access_single_element(new_x,new_y) - access_single_element(old_x,old_y)) > allowed_orthogonal_diff)
             {
               ok = 0;
               //report("fail1 since "+floattostring(access_single_element(new_x,new_y))+" and "+floattostring(access_single_element(old_x,old_y))+" is larger than "+floattostring(allowed_orthogonal_diff));
             }
             if (fabs(access_single_element(new_x,new_y) - access_single_element(new_x-inc_x,new_y-inc_y)) > allowed_diff)
             {
               ok = 0;
               //report("diff is "+floattostring(fabs(access_single_element(new_x,new_y) - access_single_element(new_x-inc_x,new_y-inc_y))));
             }
          }
          else
          {
            //report("fail3");
            ok = 0;
          }
        }
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
          coordlist[current_in_a_row]=access_single_element(i,j);
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
     coordlist.clear();
    }
    previous_x=i;
    previous_y=j;
    i +=inc_x;
    j +=inc_y;

   //report("point after inc "+floattostring(i)+","+floattostring(j));
 // cout << "point "<<i<<","<<j<<endl;
   
   if (direction == 1)
   {
     if (j >= tile->width.y)
     {
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       i++; //since inc_x zero increment by 1
       j=0;
       previous_valid=0;
     }
   }
   
   if (direction == 2)
   {
     if ((i<0) || (j>=tile->width.y))
     {
       if (startx < tile->width.x -1)
         startx-=inc_x;
       else
         starty+=inc_y;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       i=startx;
       j=starty;
       previous_valid=0;
     }
   }

   if (direction == 3)
   {
     if (i<0)
     {
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       i=startx;
       j+=1; //since inc_y zero increment by 1
       previous_valid=0;
     }
   }


   if (direction == 4)
   {
     if ((i<0) || (j < 0))
     {
       if (startx < tile->width.x  -1)
         startx -= inc_x;
       else
         starty +=inc_y;
check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       i=startx;
       j=starty;
       previous_valid=0;
     }
   }

   if (direction == 5)
   {
     if (j < 0)
     {
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       j=tile->width.y-1;
       i+=1;; // since inc_x zero shift by 1
       previous_valid=0;
     }
   }

   if (direction == 6)
   {
     if ((i>tile->width.x -1) || (j < 0))
     {
       if (startx > 0)
         startx -= inc_x;
       else
         starty += inc_y;;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
       if (startx < 0)
         startx = 0;
       i=startx;
       j=starty;
       previous_valid=0;
     }

   }
   
   if (direction == 7)
   {
     if (i > tile->width.x -1)
     {
      check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);
      i=0;
       j+=1; //set to 1 because inc_y is 0
     }
   }

   if (direction == 8)
   {
     if ((i> tile->width.x -1) || (j > tile->width.y -1))
     {
       if (startx == 0)
         starty+=inc_y;
       else
         startx-=inc_x;
       if ((startx < 0) && (starty == 0))
         startx = 0;
       check_current_landebahn(current_in_a_row, needed_points_in_a_row,i,j);

       i=startx;
       j=starty;
       previous_valid=0;
     }

   }

   //completed check
   //report("Corrected point "+floattostring(i)+","+floattostring(j));
     if( i < 0)
       completed =1;
 
     if ( j < 0)
       completed = 1;
 
      if (i > tile->width.x)
        completed = 1;

      if (j > tile->width.y)
        completed=1;
  }

  report("Checksum is "+floattostring(checksum));
  printf("Checksum is %d\n",checksum);
 
  if (checksum == tile->width.x*tile->width.y) 
   printf("OK\n");
  else
    printf("FATAL\n");
  return;
}
