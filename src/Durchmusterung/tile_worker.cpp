#include "tile_worker.h"


/*! \brief constructor of tile_manager
 *   
 *
 * 
 */
tile_worker::tile_worker()
{
   eparam = NULL;
   tile = NULL;

}

void tile_worker::set_param_and_tile(extractionParameters* param_in, tileCharacteristics* tile_in)
{
  eparam=param_in;
  tile=tile_in;
}


//debug function
void tile_worker::print_out_map()
{
  for (int i=0; i < eparam->requestedwidth*eparam->requestedlength; i++)
  if (tile->buf[i] > -32767)
    cout << tile->buf[i]<<endl;
}

//debug function
void tile_worker::check_element_access()
{
  for (int y=0; y < eparam->requestedlength; y++)
    for(int x=0; x < eparam->requestedwidth; x++)
    {
      float temp = access_single_element(x,y);
     if (temp > -32767)
      cout << temp<<endl;
    }
}

float tile_worker::access_single_element(int x, int y)
{
  if (eparam->requestedwidth*y+x < eparam->requestedwidth*eparam->requestedlength)
    return(tile->buf[eparam->requestedwidth*y+x]);
  else
   return numeric_limits<float>::min();

}


void tile_worker::durchmustere_kachel()
{
  //initialization
  //int** kachel;
  //int width = 100;
  //int height = 100;
  //kachel = new int*[width];
  //for (int i = 0;i<width;i++)
  //  kachel[i] = new int[height];
  //
  // set random values
  // for (int i = 0; i<width; i++)
  //   for( int j = 0; j<height;j++)
  //      kachel[i][j]= rand() % 100;
  //
  // check
                           check_steigungen(1);
                           check_steigungen(2);
                           check_steigungen(3);
                           check_steigungen(4);
                           check_steigungen(5);
                           check_steigungen(6);
                           check_steigungen(7);
                           check_steigungen(8);
  return;
}

void tile_worker::check_steigungen(const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */)
{
  int incx=0;
  int incy=0;
  int startx;
  int starty;

  int orth_x=0;
  int orth_y=0;
  switch (direction){
    case 1:
        startx=0;
        starty=0;
        incx=0;
        incy=1;
        orth_x=1;
        orth_y=0;
        break;
    case 2:
        startx=0;
        starty=0;
        incx=-1;
        incy=1;
        orth_x=1;
        orth_y=-1;
        break;
    case 3:
        startx=tile->outwidth-1;
        starty=0;
        incx=-1;
        incy=0;
        orth_x=0;
        orth_y=1;
        break;
    case 4:
        startx=tile->outwidth-1;
        starty=0;
        incx=-1;
        incy=-1;
        orth_x=1;
        orth_y=-1;
        break;
    case 5:
        startx=0;
        starty=tile->outlength-1;
        incx=0;
        incy=-1;
        orth_x=1;
        orth_y=0;
         break;
    case 6:
        startx=0;
        starty=0;
        incx=1;
        incy=-1; 
        orth_x=1;
        orth_y=1;
        break;
    case 7:
        startx=0;
        starty=0;
        incx=1;
        incy=0;
        orth_x=0;
        orth_y=1;
        break;
    case 8:
        startx=0;
        starty=tile->outlength-1;
        incx=1;
        incy=1;
        orth_x=1;
        orth_y=-1;
        break;
    default:
        return ; 
  }

  int completed=0;

  int i=startx;
  int j=starty;
  int checksum = 0;
  
  int previous_x=0;
  int previous_y=0;
  int previous_valid =0;
  int accept_short_slope=35;
  int accept_orthogonal_slope=10;
  while (! completed)
  {
    ++checksum;
    printf("aktuell %d und %d mit %lf\n",i,j,access_single_element(i,j));

    if (previous_valid)
    {
      printf("compare point %d,%d with %d,%d\n",i,j,previous_x,previous_y);
      if (abs(access_single_element(i,j) - access_single_element(i,j)) < accept_short_slope)
      {
        printf("accept  sh.sl. %lf und %lf\n",access_single_element(i,j),access_single_element(previous_x,previous_y));
        int new_x=i+orth_x;
        int new_y=j+orth_y;
        int ok=1;
        if (((new_x>=0) && (new_x<tile->outwidth)) && ((new_y>=0) && (new_y < tile->outlength)))
          if (abs(access_single_element(i,j)-access_single_element(new_x,new_y)) > accept_orthogonal_slope)
            ok=0;
        if (ok)
        {
          new_x=i-orth_x;
          new_y=j-orth_y;
          if (((new_x>=0) && (new_x<tile->outwidth)) && ((new_y>=0) && (new_y < tile->outlength)))
            if (abs(access_single_element(i,j)-access_single_element(new_x,new_y)) > accept_orthogonal_slope)
              ok=0;

        }
        if (ok)
          printf("Ein fertiger Punkt ist %d, %d\n",i,j);  
      }
      else
        printf("not accept sh.sl. %lf und %lf\n",access_single_element(i,j),access_single_element(previous_x,previous_y)); 
    }
    previous_x=i;
    previous_y=j;
    i +=incx;
    j +=incy;

   
   previous_valid=1;
   if (direction == 1)
   {
     if (j >= tile->outlength)
     {
       i++;
       j=0;
       previous_valid=0;
     }
   }
   
   if (direction == 2)
   {
     if ((i<0) || (j>=tile->outlength))
     {
       if (startx < tile->outwidth -1)
         startx++;
       else
         starty++;

       i=startx;
       j=starty;
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
     }
   }


   if (direction == 4)
   {
     if ((i<0) || (j < 0))
     {
       if (starty < tile->outlength  -1)
         ++starty;
       else
         --startx;

       i=startx;
       j=starty;
       previous_valid=0;
     }
   }

   if (direction == 5)
   {
     if (j < 0)
     {
       j=tile->outlength-1;
       ++i;
       previous_valid=0;
     }
   }

   if (direction == 6)
   {
     if ((i>tile->outwidth -1) || (j < 0))
     {
       if (starty < tile->outlength  -1)
         ++starty;
       else
         ++startx;

       i=startx;
       j=starty;
       previous_valid=0;
     }

   }
   
   if (direction == 7)
   {
     if (i > tile->outwidth -1)
     {
       i=0;
       ++j;
     }
   }

   if (direction == 8)
   {
     if ((i> tile->outwidth -1) || (j > tile->outlength -1))
     {
       if (starty > 0)
         --starty;
       else
         ++startx;

       i=startx;
       j=starty;
       previous_valid=0;
     }

   }

   //completed check

     if( i < 0)
       completed =1;
  
      if (i >= tile->outwidth)
        completed = 1;

      if (j >= tile->outlength)
        completed=1;
  }

  printf("Checksum is %d\n",checksum);
 
  if (checksum == tile->outwidth*tile->outlength) 
   printf("OK\n");
  else
    printf("FATAL\n");
  return;
}
