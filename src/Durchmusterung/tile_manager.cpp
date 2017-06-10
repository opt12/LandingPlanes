#include "tile_manager.h"


/*! \brief constructor of tile_manager
 *   
 *
 * 
 */
tile_manager::tile_manager()
{
 tiff_input_file="";
}

tile_manager::tile_manager(string input_file)
{
  tiff_input_file=input_file;
}

void tile_manager::get_tile()
{
  get_tile_array();
}


void tile_manager::get_tile_array()
{
  char* inputfilename = "unkownn";
  tileCharacteristics *info;
  info = new tileCharacteristics();
  
  extractionParameters p;
  p.requestedxmin =0;
  p.requestedymin = 0;
  p.requestedwidth = 2000;
  p.requestedlength=2000;
  int retcode =  getImageInformation(info, tiff_input_file.c_str() );
  cout << "Retcode ist "<<retcode<<endl;
  cout << info->spp<<endl;
  cout << info->bitspersample<<endl;
  cout << info->outlength<<endl;
  cout << info->outwidth<<endl;
  retcode =  makeExtractFromTIFFFile(p, info,tiff_input_file.c_str());
  cout << "Retcode ist "<<retcode<<endl;
  for (int i=0; i < p.requestedwidth*p.requestedlength; i++)
    cout << info->buf[i]<<endl;
  global_map_map[p]=info;
  return;
}
