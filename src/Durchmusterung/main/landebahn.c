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
  central_manager.get_tile(worker1,0,0,100,100);
  //worker1.check_element_access();
  //return 6;
  //worker1.print_out_map();
  //return 5;

  worker1.durchmustere_kachel();

	return 0;
}
