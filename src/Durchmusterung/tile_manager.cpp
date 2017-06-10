#include "tile_manager.h"


/*! \brief constructor of tile_manager
 *   
 *
 * 
 */
tile_manager::tile_manager()
{

}

void tile_manager::get_tile()
{
  get_tile_array();
}


void tile_manager::get_tile_array()
{
  char* inputfilename = "unkownn";
  tileCharacteristics info;
  int retcode =  getImageInformation(&info,  inputfilename);

  //int makeExtractFromTIFFFile(const extractionParameters p, tileCharacteristics* tile, const char * infilename);

  return;
}
