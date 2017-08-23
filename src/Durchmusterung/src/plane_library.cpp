#include "tile_worker.h"
#include "global.h"

int search_for_planes(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler, float heading, float minLength, float width, int commSocket,const json *taskDescription, float noDataValue, rectSize pixelSize )
{
  cout << "in search for planes"<<endl;
  double short_range_slope = (*taskDescription)["scanParameters"]["maxVarianceLong"];
  double long_range_slope = (*taskDescription)["scanParameters"]["maxRise"];
  double *not_defined = new double();
  *not_defined= noDataValue ;
  double orthogonal_slope =  (*taskDescription)["scanParameters"]["maxVarianceCross"];
  minLength=minLength;
  sem_t semaphore;
  sem_init(&semaphore,0,(*taskDescription)["scanParameters"]["numThreads"]);
  cout << "before worker init"<<endl;
  tile_worker *worker1 = new tile_worker(actualTile, minLength, short_range_slope, long_range_slope, not_defined, heading, myGeoTiffHandler, width, orthogonal_slope, commSocket,taskDescription,&semaphore,pixelSize );
  cout << "before durchmustere"<<endl;
  worker1->durchmustere_kachel();
  cout << "nach durchmustern"<<endl;
  delete(worker1);
  return 1;
}
