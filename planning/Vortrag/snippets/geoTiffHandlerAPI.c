resultType getDatasetInfo(datasetInfo *info);
resultType getPixelExtent(rectSize *pixelSize);
resultType getTilingInfo(const geoCoord topLeft, const geoCoord bottomRight, const float overlap, const size_t maxSize, tilingCharacteristics *tilingResult);
resultType getTile(const int xTile, const int yTile, tileData *tile);
resultType releaseTile(const int xTile, const int yTile);

geoCoord pixel2Geo(const pixelCoordFloat source);
geoCoord pixel2Geo(const int xTile, const int yTile, const pixelCoordFloat source);
pixelCoord geo2Pixel(const geoCoord source);
pixelCoord geo2Pixel(const int xTile, const int yTile, const geoCoord source);
