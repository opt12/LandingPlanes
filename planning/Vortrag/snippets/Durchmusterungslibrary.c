int search_for_planes(const tileData *actualTile, 
                      GeoTiffHandler *myGeoTiffHandler,
                      float heading, 
                      float minLength, 
                      float width, 
                      int commSocket, 
                      const json *taskDescription, 
                      float noDataValue , 
                      rectSize pixelSize );