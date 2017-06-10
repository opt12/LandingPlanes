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

void tile_manager::get_tile(tile_worker &worker_in,int xmin, int ymin, int width, int length)
{
  get_tile_array(worker_in,xmin,ymin,width,length);
}


void tile_manager::get_tile_array(tile_worker &worker_in, int xmin, int ymin, int width, int length)
{
  char* inputfilename = "unkownn";
  tileCharacteristics *info;
  info = new tileCharacteristics();
  
  extractionParameters* p;
  p = new extractionParameters();
  p->requestedxmin =xmin;
  p->requestedymin = ymin;
  p->requestedwidth = width;
  p->requestedlength=length;
  int retcode =  getImageInformation(info, tiff_input_file.c_str() );
  cout << "Retcode ist "<<retcode<<endl;
  cout << info->spp<<endl;
  cout << info->bitspersample<<endl;
  cout << info->outlength<<endl;
  cout << info->outwidth<<endl;
  retcode =  makeExtractFromTIFFFile(*p, info,tiff_input_file.c_str());
  cout << "Retcode ist "<<retcode<<endl;
  //for (int i=0; i < p->requestedwidth*p->requestedlength; i++)
  //  cout << info->buf[i]<<endl;
  global_map_map[*p]=info;
  worker_in.set_param_and_tile(p,info);
  return;
}
