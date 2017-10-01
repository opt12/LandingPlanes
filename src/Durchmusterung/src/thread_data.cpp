#include "thread_data.h"

/*! \brief constructor for the thread_data object
 *
 *
 *  The constructor requires a valid tile_worker object where it stores the pointer to. Only read access is needed.
 */
thread_data::thread_data(tile_worker* tile_worker)
{
    my_tile_worker = tile_worker;
}
