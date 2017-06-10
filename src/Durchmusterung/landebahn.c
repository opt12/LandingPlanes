#include <stdlib.h> 
#include <stdio.h>
#include "landebahn.h"
#include "tile_manager.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {  	

  int arg = 1;
  string tiff_in;
  while (arg < argc && argv[arg][0] == '-') {     //so lange noch Argumente da sind, die mit '-' beginnen
     if (argv[arg][1] == 'E')
     {
       tiff_in = argv[arg+1];
     }

    ++arg;
  }

  cout << "Input file: "<<tiff_in<<endl;


  tile_worker worker1;

                        

  tile_manager central_manager(tiff_in);
  central_manager.get_tile(worker1,0,0,20000,20000);

  worker1.print_out_map();
  return 5;

  durchmustere_kachel();

	return 0;
}

void check_steigungen(int** const kachel, const int width, const int height, const int direction /*1: N -> S, 2: NNO -> SSW, 3: O -> W, 4: SSO -> NNW, 5: S -> N, 6: SSW -> NNO, 7: W -> O, 8: NNW -> SSO */)
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
        startx=width-1;
        starty=0;
        incx=-1;
        incy=0;
        orth_x=0;
        orth_y=1;
        break;
    case 4:
        startx=width-1;
        starty=0;
        incx=-1;
        incy=-1;
        orth_x=1;
        orth_y=-1;
        break;
    case 5:
        startx=0;
        starty=height-1;
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
        starty=height-1;
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
    printf("aktuell %d und %d mit %d\n",i,j,kachel[i][j]);

    if (previous_valid)
    {
      printf("compare point %d,%d with %d,%d\n",i,j,previous_x,previous_y);
      if (abs(kachel[i][j] - kachel[previous_x][previous_y]) < accept_short_slope)
      {
        printf("accept  sh.sl. %d und %d\n",kachel[i][j],kachel[previous_x][previous_y]);
        int new_x=i+orth_x;
        int new_y=j+orth_y;
        int ok=1;
        if (((new_x>=0) && (new_x<width)) && ((new_y>=0) && (new_y < height)))
          if (abs(kachel[i][j]-kachel[new_x][new_y]) > accept_orthogonal_slope)
            ok=0;
        if (ok)
        {
          new_x=i-orth_x;
          new_y=j-orth_y;
          if (((new_x>=0) && (new_x<width)) && ((new_y>=0) && (new_y < height)))
            if (abs(kachel[i][j]-kachel[new_x][new_y]) > accept_orthogonal_slope)
              ok=0;

        }
        if (ok)
          printf("Ein fertiger Punkt ist %d, %d\n",i,j);  
      }
      else
        printf("not accept sh.sl. %d und %d\n",kachel[i][j],kachel[previous_x][previous_y]); 
    }
    previous_x=i;
    previous_y=j;
    i +=incx;
    j +=incy;

   
   previous_valid=1;
   if (direction == 1)
   {
     if (j >= height)
     {
       i++;
       j=0;
       previous_valid=0;
     }
   }
   
   if (direction == 2)
   {
     if ((i<0) || (j>=height))
     {
       if (startx < width -1)
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
       if (starty < height  -1)
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
       j=height-1;
       ++i;
       previous_valid=0;
     }
   }

   if (direction == 6)
   {
     if ((i>width -1) || (j < 0))
     {
       if (starty < height  -1)
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
     if (i > width -1)
     {
       i=0;
       ++j;
     }
   }

   if (direction == 8)
   {
     if ((i>width -1) || (j > height -1))
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
  
      if (i >= width)
        completed = 1;

      if (j >= height)
        completed=1;
  }

  printf("Checksum is %d\n",checksum);
 
  if (checksum == width*height) 
   printf("OK\n");
  else
    printf("FATAL\n");
  return;
}


void durchmustere_kachel()
{
  //initialization
  int** kachel;
  int width = 100;
  int height = 100;
  kachel = new int*[width];
  for (int i = 0;i<width;i++)
    kachel[i] = new int[height];

  // set random values
  for (int i = 0; i<width; i++)
   for( int j = 0; j<height;j++)
     kachel[i][j]= rand() % 100;  

  // check
  check_steigungen(kachel,width,height,1);
  check_steigungen(kachel,width,height,2);
  check_steigungen(kachel,width,height,3);
  check_steigungen(kachel,width,height,4);
  check_steigungen(kachel,width,height,5);
  check_steigungen(kachel,width,height,6);
  check_steigungen(kachel,width,height,7);
  check_steigungen(kachel,width,height,8); 
  return;
}
