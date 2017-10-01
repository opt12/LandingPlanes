#include "tile_worker.h"
#include "global.h"

int search_for_planes(const tileData* actualTile /** [in] pointer to tile object including linear array of geo values  */, GeoTiffHandler* myGeoTiffHandler /** [in] pointer to GeoTiffHandler for performing GeoTiff translations  */, float heading /** [in] angle for plane orientation */, float minLength /** [in] minimum length of planes */, float width /** [in] desired width or plane */, int commSocket /** [out] socket for MongoDB  */, const json* taskDescription /** [in] task description json object for identification purpose in MongoDB */, float noDataValue /** [in] definition of the not defined value  */ , rectSize pixelSize /** [in] scaling information on pixel densitiy  */, double short_range_slope /** [in] max allowed slope between neighbour tiles */, double long_range_slope /** [in] max allowed slope between starting and endpoint of plane */, double orthogonal_slope /** [in] maximum allowed slope in orthogonal direction of neighboured tiles */, int numThreads /** [in] number of threads that should be used */)
{
    double* not_defined = new double();
    *not_defined = noDataValue ;
    minLength = minLength;
    sem_t semaphore;
    sem_init(&semaphore, 0, numThreads);
    tile_worker* worker1 = new tile_worker(actualTile, minLength, short_range_slope, long_range_slope, not_defined, heading, myGeoTiffHandler, width, orthogonal_slope, commSocket, taskDescription, &semaphore, pixelSize );
    worker1->durchmustere_kachel();
    delete (worker1);
    return 1;
}
