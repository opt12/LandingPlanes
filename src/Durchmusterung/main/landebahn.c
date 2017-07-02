#include <stdlib.h> 
#include <stdio.h>
#include "landebahn.h"
#include "tile_manager.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


/*! \brief check whether a given file is accessible for reading
 *   
 *
 * This function checks whether the given input file is readable
 */

int file_readable(string infile) 
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

int main(int argc, char* argv[]) {  	

  int arg = 1;
  string tiff_in;
  double landing_plane_length=0.0;
  double short_range_slope=100;
  double long_range_slope=100;
  double *not_defined = NULL;
  while (arg < argc && argv[arg][0] == '-') {     //so lange noch Argumente da sind, die mit '-' beginnen
     if (argv[arg][1] == 'E')
     {
       tiff_in = argv[arg+1];
       ++arg;
     }
     else if (argv[arg][1] == 'L') // length of landing plane
     {
       landing_plane_length=atof(argv[arg+1]);
       ++arg;
     }
     else if (argv[arg][1] == 'S') //short range slope in percent
     {
       short_range_slope=atof(argv[arg+1]);
       ++arg;
     }
     else if (argv[arg][1] == 'T') // long range slope in percent
     {
       long_range_slope=atof(argv[arg+1]);
       ++arg;

     }
     else if (argv[arg][1] == 'N') // not defined
     {
       not_defined = new double();
       *not_defined = atof(argv[arg+1]);
       ++arg;
     }

    ++arg;
  }

  if (landing_plane_length == 0.0)
  {
    cout << "Fatal Error: landing plane length not given or set to zero"<<endl;
    exit(1);
  }

  if (!file_readable(tiff_in))
  {
    cout << "Fatal Error: Input geo tiff file does not exist or is not readable"<<endl;
    exit(2);
  }
  cout << "Minimum landing plane length is "<<landing_plane_length<<" m"<<endl;
  cout << "Input file: "<<tiff_in<<endl;
  cout <<  "short range slope: "<<short_range_slope<<" %"<<endl;
  cout << "long range slope: "<<long_range_slope<<" %"<<endl;
  if (not_defined != NULL)
    cout << "not defined ist "<<*not_defined<<endl;
  tile_worker worker1;

                        

  tile_manager central_manager(tiff_in,landing_plane_length,short_range_slope,long_range_slope,not_defined);
  central_manager.get_tile(worker1,0,0,10000,10000);
  //worker1.check_element_access();
  //return 6;
  //worker1.print_out_map();
  //return 5;

  worker1.durchmustere_kachel();

	return 0;
}
