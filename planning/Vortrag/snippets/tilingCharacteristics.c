struct tilingCharacteristics {
	int overallXSize = 0, overallYSize = 0;	
		//the extent of the currently requested cutout of the dataset
	pixelCoord topLeftPix = {0,0};	
		//top left pixel coordinate of the requested area
	pixelCoord bottomRightPix = {0,0};	
		//bottom right pixel coordinate of the requested area
	int maxTileSizeXPix = 0, maxTileSizeYPix = 0;
	int overlapXPix =0, overlapYPix =0;
	int tilesInX = 0, tilesInY = 0;	
		//the amount of tiles in X- and Y-direction
	float overlap = 0;	
		//the overlap of the tiles in [meter]
	size_t maxTileMemsize = 0;	
		//the maximum amount of bytes a tile needs in memory
};