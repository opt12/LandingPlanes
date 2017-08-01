#include "tile_manager.h"
#include "error.h"


/*geoCoord tile_manager::cal_geo_from_pixel(pixelCoord pix)
{
return myGeoTiffHandler.pixel2Geo( pix);
}*/



int printMatrixToFile(const char* filename, const char* matrixName,
                const int tileSizeX, const int tileSizeY, float *matrix) {
    FILE* outputFile1 = fopen(filename,"w");
    if(!outputFile1)
    {
            fprintf(stderr,"Could not open file %s for writing\n", filename);
            return 1;
    }

    fprintf(outputFile1, "# Created by testReadInTiff.c\n"
                "# name: %s\n"
                "# type: matrix\n"
                "# rows: %d\n"
                "# columns: %d\n",
                        matrixName, tileSizeY, tileSizeX);

        for(int y=0; y < tileSizeY; y++){
                for(int x= 0; x< tileSizeX; x++){
                const int lin = y*tileSizeX+x;
                        fprintf(outputFile1, "%f ", matrix[lin]);
                }
                fprintf(outputFile1,"\n");
        }
        fprintf(outputFile1,"\n\n");

    fclose(outputFile1);
    return EXIT_SUCCESS;
}


/*! \brief constructor of tile_manager
 *   
 *
 * This constructors sets all class members to default 
 */
tile_manager::tile_manager()
{
 tiff_input_file="";
   GDALAllRegister();
}


/*! \brief constructor of tile_manager
 *    
 *
 * This constructors sets all class members to default and already predefines the input tiff file
 */
tile_manager::tile_manager(string tiff_input_file, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double start_angle_of_plane, double angle_increment, double width_of_plane, double orthogonal_slope)
{
   GDALAllRegister();
  this->tiff_input_file=tiff_input_file;
  this->landing_plane_length=landing_plane_length;
  this->short_range_slope=short_range_slope;
  this->long_range_slope=long_range_slope;
  this->not_defined=not_defined;
  if (not_defined!= NULL)
    cout << "not defined in constructor "<<*this->not_defined<<endl;
  this->start_angle_of_plane = start_angle_of_plane;
  this->angle_increment=angle_increment;
  this->current_angle=start_angle_of_plane;
  this->width_of_plane=width_of_plane;
  this->orthogonal_slope=orthogonal_slope;
}

int tile_manager::init_geo_handler()
{
  cout << "would like to open "<<tiff_input_file<<endl;
  if (myGeoTiffHandler.openGeoTiff(tiff_input_file.c_str()) != SUCCESS)
			return LP_ERR_INIT_GEO ;
  if(myGeoTiffHandler.getDatasetInfo(&info) != SUCCESS) {
                        return LP_ERR_DAT_SET_INFO;
                }

  not_defined = new double();
  *not_defined=info.noDataValue;
  return 0;

}


tile_manager::~tile_manager()
{
  delete(tileChar);
  delete(not_defined);
}

int tile_manager::select_area(int xmin, int xmax, int ymin, int ymax)
{
  cout << "aha"<<endl;
  tileChar = new tilingCharacteristics();

  pixelCoord pix = { xmax, ymax};
  geoCoord geoBottomRight = myGeoTiffHandler.pixel2Geo( pix);
  pix = { xmin, ymin};
  geoCoord geoTopLeft = myGeoTiffHandler.pixel2Geo( pix);
  myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBottomRight, 0, MAX_SIZE, tileChar);
  cout << tileChar <<endl;

 return 0;  
}

/*! \brief read from tiff to tile_worker
 *    
 *
 * This method reads in a tile from a tiff file to the internal strcutre of the tile_worker
 */
void tile_manager::get_tile(tile_worker *worker_in, int x, int y)
{
  get_tile_array(worker_in, x,y);
}

/*! \brief returns number of tiles in X
 *    
 *
 * This function returns the number of tiles in x dimension
 */

int tile_manager::get_tiles_X()
{
  return tileChar->tilesInX;
}


/*! \brief returns number of tiles in Y
 *    
 *
 * This function returns the number of tiles in y dimension
 */

int tile_manager::get_tiles_Y()
{
  return tileChar->tilesInY;
}



/*! \brief reads in tile 
 *    
 *
 * This method reads in a tile defined by min x and miny and the dimensions width (x dimension) and length (y dimension). Pointer to extraction parameters and tile characteristics are given to the requesting tile_worker instance and in addition stored to the tile_manager global_map, so that additional workers can reuse the occupied memory (read only).
 */
void tile_manager::get_tile_array(tile_worker *worker_in, int idxX, int idxY)
{
/*  cout << "aha"<<endl;
  tileChar = new tilingCharacteristics();
  
  pixelCoord pix = { xmin+width, ymin+length};
  geoCoord geoBottomRight = myGeoTiffHandler.pixel2Geo( pix);
  pix = { xmin, ymin};
  geoCoord geoTopLeft = myGeoTiffHandler.pixel2Geo( pix);
  myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBottomRight, 0, MAX_SIZE, tileChar);
  cout << tileChar <<endl;
  cout << "Tiles in Y "<<tileChar->tilesInY<<endl;
  cout << "Tiles in X "<<tileChar->tilesInX<<endl;
                for(int idxX=0; idxX< tileChar->tilesInX; idxX++){
                        for(int idxY=0; idxY< tileChar->tilesInY; idxY++){*/
  //                              tileData* tile;
                                tile = new tileData();
                                cout << "Fetch tile"<<idxX<< " and " <<idxY<<endl;
                                int retcode = myGeoTiffHandler.getTile(idxX, idxY, tile);
                                cout << "Der Retcode ist "<<retcode<<endl;
                                cout << *tile;
                                string matrixName= "tile_"+to_string(idxX)+"_"+to_string(idxY);
                                string fileNameOut = matrixName+string(".m");
                              printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
                                                                            tile->width.x, tile->width.y, tile->buf);
     cout << "tile offset  = "<<tile->offset.x<< " "<<tile->offset.y<< endl;
     cout << "tile width = "<<tile->width.x<<" " <<tile->width.y<<endl;
                                                             //                         }
//}

/*
  int retcode =  getImageInformation(info, tiff_input_file.c_str() );
#ifdef DEBUG
  cout << "Retcode ist "<<retcode<<endl;
  cout << info->spp<<endl;
  cout << info->bitspersample<<endl;
  cout << info->outlength<<endl;
  cout << info->outwidth<<endl;
#endif
  retcode = makeExtractFromTIFFFile(*p, info,tiff_input_file.c_str());

cout << "Check2: "<<p->requestedxmin << " und " <<p->requestedymin<<" und " <<p->requestedwidth<<" und " <<p->requestedlength<<endl;
#ifdef DEBUG
  cout << "Retcode ist "<<retcode<<endl;
  for (int i=0; i < p->requestedwidth*p->requestedlength; i++)
    cout << info->buf[i]<<endl;
#endif
  global_map_map[*p] = info;
*/
  worker_in->set_param_and_tile(tile);
  cout << "before call to worker"<<endl;
  worker_in->set_x_resolution(20.0);
  worker_in->set_y_resolution(20.0); // ask Felix how to retrieve this information from tiff
  worker_in->set_landing_plane_length(landing_plane_length);
  cout << "hier ist slope "<<short_range_slope<<endl;
  worker_in->set_short_range_slope(short_range_slope);
  worker_in->set_long_range_slope(long_range_slope);
  worker_in->set_not_defined(not_defined);
  worker_in->set_angle(current_angle);
  worker_in->set_GeoTiffHandler(&myGeoTiffHandler);
  worker_in->set_width_of_plane(width_of_plane);
  worker_in->set_orthogonal_slope(orthogonal_slope);
  return;
}

void tile_manager::release_tile(int i, int j)
{
 myGeoTiffHandler.releaseTile(i, j);   
  return;
}
