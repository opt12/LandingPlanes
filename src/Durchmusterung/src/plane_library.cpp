#include "tile_worker.h"
#include "global.h"

int search_for_planes(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler, float heading, float minLength, float width, int commSocket,const json *taskDescription)
{
  double short_range_slope = 3.0;
  double long_range_slope = 1.0;
  double *not_defined = new double();
  *not_defined= -32767;
  double orthogonal_slope = 3.0;
  minLength=3000;
  tile_worker *worker1 = new tile_worker(actualTile, minLength, short_range_slope, long_range_slope, not_defined, heading, myGeoTiffHandler, width, orthogonal_slope, commSocket,taskDescription);
  worker1->durchmustere_kachel();

  delete(worker1);
  return 1;
}
