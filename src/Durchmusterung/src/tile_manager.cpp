#include "tile_manager.h"
#include "error.h"





int printMatrixToFile(const char* filename, const char* matrixName,
                      const int tileSizeX, const int tileSizeY, float* matrix)
{
    FILE* outputFile1 = fopen(filename, "w");

    if (!outputFile1)
    {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return 1;
    }

    fprintf(outputFile1, "# Created by testReadInTiff.c\n"
            "# name: %s\n"
            "# type: matrix\n"
            "# rows: %d\n"
            "# columns: %d\n",
            matrixName, tileSizeY, tileSizeX);

    for (int y = 0; y < tileSizeY; y++)
    {
        for (int x = 0; x < tileSizeX; x++)
        {
            const int lin = y * tileSizeX + x;
            fprintf(outputFile1, "%f ", matrix[lin]);
        }

        fprintf(outputFile1, "\n");
    }

    fprintf(outputFile1, "\n\n");
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
    tiff_input_file = "";
    GDALAllRegister();
}


/*! \brief constructor of tile_manager
 *
 *
 * This constructors sets all class members to specified values and already predefines the input tiff file
 */
tile_manager::tile_manager(string tiff_input_file, double landing_plane_length, double short_range_slope, double long_range_slope, double* not_defined, double start_angle_of_plane, double angle_increment, double width_of_plane, double orthogonal_slope)
{
    GDALAllRegister();
    this->tiff_input_file = tiff_input_file;
    this->landing_plane_length = landing_plane_length;
    this->short_range_slope = short_range_slope;
    this->long_range_slope = long_range_slope;
    this->not_defined = not_defined;
    this->start_angle_of_plane = start_angle_of_plane;
    this->angle_increment = angle_increment;
    this->current_angle = start_angle_of_plane;
    this->width_of_plane = width_of_plane;
    this->orthogonal_slope = orthogonal_slope;
}

/*! \brief initialization of GeoTiffHandler
 *
 *
 * This function initializes the GeoTiffHandler for the current tiff file
 */
int tile_manager::init_geo_handler()
{
    if (myGeoTiffHandler.openGeoTiff(tiff_input_file.c_str()) != SUCCESS)
        return LP_ERR_INIT_GEO ;

    if (myGeoTiffHandler.getDatasetInfo(&info) != SUCCESS)
    {
        return LP_ERR_DAT_SET_INFO;
    }

    not_defined = new double();
    *not_defined = info.noDataValue;
    pixelSize = info.pixelSize;
    return 0;
}

/*! \brief destructor of tile_manager
 *
 *
 * The default destructor of tile_manager
 */
tile_manager::~tile_manager()
{
    delete (tileChar);
    delete (not_defined);
}

/*! \brief selects a sub area of the map
 *
 *
 * This function let you allow to scan a sub area of the tiff file. However default behaviour should be call with max::int as xmax and ymax.
@retval 0 OK
 */
int tile_manager::select_area(int xmin /** [in] min x value for area*/, int xmax /**[in] max x value for area*/, int ymin /**[in] min y value for area*/, int ymax/**[in] max y  value for area*/)
{
    tileChar = new tilingCharacteristics();
    pixelCoord pix = { xmax, ymax};
    geoCoord geoBottomRight = myGeoTiffHandler.pixel2Geo( pix);
    pix = { xmin, ymin};
    geoCoord geoTopLeft = myGeoTiffHandler.pixel2Geo( pix);
    myGeoTiffHandler.getTilingInfo(geoTopLeft, geoBottomRight, 0, MAX_SIZE, tileChar);
    cout << tileChar << endl;
    return 0;
}

/*! \brief read from tiff to tile_worker
 *
 *
 * This method reads in a subtile from a tiff file to the internal strcutre of the tile_worker
 */
void tile_manager::get_tile(tile_worker* worker_in /** reference to tile_worker*/, int x  , int y /** y index of subtile*/)
{
    get_tile_array(worker_in, x, y);
}


/*! \brief returns number of tiles in X
 *
 *
 * This function returns the number of tiles in x dimension
@retval number of tiles in X
 */
int tile_manager::get_tiles_X()
{
    return tileChar->tilesInX;
}


/*! \brief returns number of tiles in Y
 *
 *
 * This function returns the number of tiles in y dimension
@retval number of tiles in Y
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
void tile_manager::get_tile_array(tile_worker* worker_in/** pointer to the worker object*/, int idxX /** x index of subtile to be fetched*/, int idxY /** y index of subtile to be fetched*/)
{
    tile = new tileData();
    cout << "Fetch tile" << idxX << " and " << idxY << endl;
    int retcode = myGeoTiffHandler.getTile(idxX, idxY, tile);
    cout << "Der Retcode ist " << retcode << endl;
    cout << *tile;
    string matrixName = "tile_" + to_string(idxX) + "_" + to_string(idxY);
    string fileNameOut = matrixName + string(".m");
    printMatrixToFile(fileNameOut.c_str(), matrixName.c_str(),
                      tile->width.x, tile->width.y, tile->buf);
    cout << "tile offset  = " << tile->offset.x << " " << tile->offset.y << endl;
    cout << "tile width = " << tile->width.x << " " << tile->width.y << endl;

    worker_in->set_param_and_tile(tile);
    cout << "before call to worker" << endl;
    worker_in->set_x_resolution(pixelSize.x);
    worker_in->set_y_resolution(pixelSize.y);
    worker_in->set_landing_plane_length(landing_plane_length);
    cout << "hier ist slope " << short_range_slope << endl;
    worker_in->set_short_range_slope(short_range_slope);
    worker_in->set_long_range_slope(long_range_slope);
    worker_in->set_not_defined(not_defined);
    worker_in->set_angle(current_angle);
    worker_in->set_GeoTiffHandler(&myGeoTiffHandler);
    worker_in->set_width_of_plane(width_of_plane);
    worker_in->set_orthogonal_slope(orthogonal_slope);
    return;
}

/*! \brief release the tile
 *
 *
 * This method releases a tile which is no longer in use
 */
void tile_manager::release_tile(int i /** x index of subtile to be released*/, int j/** y index of subtile to be released*/)
{
    myGeoTiffHandler.releaseTile(i, j);
    return;
}
