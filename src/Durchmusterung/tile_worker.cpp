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
