#include "tile_manager.h"

/*! \brief constructor of tile_manager
 *   
 *
 * This constructors sets all class members to default 
 */
tile_manager::tile_manager()
{
 tiff_input_file="";
}


/*! \brief constructor of tile_manager
 *    
 *
 * This constructors sets all class members to default and already predefines the input tiff file
 */
tile_manager::tile_manager(string tiff_input_file, double landing_plane_length, double short_range_slope, double long_range_slope)
{
  this->tiff_input_file=tiff_input_file;
  this->landing_plane_length=landing_plane_length;
  this->short_range_slope=short_range_slope;
  this->long_range_slope=long_range_slope;
}

/*! \brief read from tiff to tile_worker
 *    
 *
 * This method reads in a tile from a tiff file to the internal strcutre of the tile_worker
 */
void tile_manager::get_tile(tile_worker &worker_in, int xmin, int ymin, int width, int length)
{
  get_tile_array(worker_in, xmin, ymin, width, length);
}

/*! \brief reads in tile 
 *    
 *
 * This method reads in a tile defined by min x and miny and the dimensions width (x dimension) and length (y dimension). Pointer to extraction parameters and tile characteristics are given to the requesting tile_worker instance and in addition stored to the tile_manager global_map, so that additional workers can reuse the occupied memory (read only).
 */
void tile_manager::get_tile_array(tile_worker &worker_in, int xmin, int ymin, int width, int length)
{
  tileCharacteristics *info;
  info = new tileCharacteristics();
  
  extractionParameters* p;
  p = new extractionParameters();

  p->requestedxmin = xmin;
  p->requestedymin = ymin;
  p->requestedwidth = width;
  p->requestedlength = length;
  int retcode =  getImageInformation(info, tiff_input_file.c_str() );
#ifdef DEBUG
  cout << "Retcode ist "<<retcode<<endl;
  cout << info->spp<<endl;
  cout << info->bitspersample<<endl;
  cout << info->outlength<<endl;
  cout << info->outwidth<<endl;
#endif
  retcode = makeExtractFromTIFFFile(*p, info,tiff_input_file.c_str());
#ifdef DEBUG
  cout << "Retcode ist "<<retcode<<endl;
  for (int i=0; i < p->requestedwidth*p->requestedlength; i++)
    cout << info->buf[i]<<endl;
#endif
  global_map_map[*p] = info;
  worker_in.set_param_and_tile(p, info);
  cout << "before call to worker"<<endl;
  worker_in.set_x_resolution(20.0);
  worker_in.set_y_resolution(20.0); // ask Felix how to retrieve this information from tiff
  worker_in.set_landing_plane_length(landing_plane_length);
  cout << "hier ist slope "<<short_range_slope<<endl;
  worker_in.set_short_range_slope(short_range_slope);
  worker_in.set_long_range_slope(long_range_slope);
  return;
}
