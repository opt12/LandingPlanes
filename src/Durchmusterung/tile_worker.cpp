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


void tile_worker::print_out_map()
{
  for (int i=0; i < eparam->requestedwidth*eparam->requestedlength; i++)
  if (tile->buf[i] > -32767)
    cout << tile->buf[i]<<endl;
}
