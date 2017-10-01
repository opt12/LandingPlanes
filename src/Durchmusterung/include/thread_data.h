#include "tile_worker.h"

/*! \file thread_data.h */

class thread_data {
    public:
        thread_data(tile_worker* tile_worker);
        static void* check_single_plane(void* x_void_ptr);

    private:
        tile_worker* my_tile_worker; ///< pointer to the tile_worker class. Read only usage by thread
};
