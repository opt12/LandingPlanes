#include "tile_worker.h"
#include "global.h"

int search_for_planes(const tileData *actualTile, GeoTiffHandler *myGeoTiffHandler, float heading, float minLength, float width, int commSocket,const json *taskDescription)
{
  cout << "in search for planes"<<endl;
  double short_range_slope = 3.0;
  double long_range_slope = 1.0;
  double *not_defined = new double();
  *not_defined= -32767;
  double orthogonal_slope = 3.0;
  minLength=3000;
  sem_t semaphore;
   sem_init(&semaphore,0,16);
  cout << "before worker init"<<endl;
  tile_worker *worker1 = new tile_worker(actualTile, minLength, short_range_slope, long_range_slope, not_defined, heading, myGeoTiffHandler, width, orthogonal_slope, commSocket,taskDescription,&semaphore);
  cout << "before durchmustere"<<endl;
  worker1->durchmustere_kachel();
  cout << "nach durchmustern"<<endl;
  delete(worker1);
  return 1;
}
