/*
 * geojson_utils.cpp
 *
 *  Created on: 19.07.2017
 *      Author: eckstein
 */

#include <string>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

using namespace std;

string getGeoJsonString() {
	//this is the northern runway of DUS as an example
	json geoJ = {
					{ "type", "Feature" },
					{ "geometry", {
							{"type", "Polygon" },
							{ "coordinates", { {
//									{{ "lng", 6.748309135437012 }, { "lat", 51.28393048228927 } },
//									{{ "lng", 6.748652458190918 }, { "lat", 51.283608378404644 } },
//									{{ "lng", 6.779808998107910 }, { "lat", 51.29815448415222 } },
//									{{ "lng", 6.779379844665527 }, { "lat", 51.29861065279019 } },
//									{{ "lng", 6.748309135437012 }, { "lat", 51.28393048228927 } }
									{6.748309135437012, 51.28393048228927 },
									{6.748652458190918, 51.283608378404644},
									{6.779808998107910, 51.29815448415222 },
									{6.779379844665527, 51.29861065279019 },
									{6.748309135437012, 51.28393048228927 }
								}}
							}
					}},

			{ "properties",
					{ 		{ "heading", 50.0 },
							{ "minimumLength", 2000.0 },
							{ "minimumWidth", 30.0 },
							{ "maxVariance", 5.5 },
							{ "maxRise", 10.0 }
					}
			}
	};

	return geoJ.dump(4);
}

