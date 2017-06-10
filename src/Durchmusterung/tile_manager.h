extern "C" {
#include "readInTiff.h"
}

#include <string>
#include <iostream>

using namespace std;


class tile_manager{

  private:
    void get_tile_array(); 
    string tiff_input_file; 

  public:
    tile_manager();
    tile_manager(string source_file_in);
    void get_tile();

};
