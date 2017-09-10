struct datasetInfo {
	pixelPair extent = {0,0};	
		//the extent of the current dataset
	rectSize pixelSize = {0,0}; 	
		//dimension of a single pixel in [meter]
	float noDataValue = 0;	//marker for "no data" in the dataset;
		//the marker value for invalid/non existant pixels
};	