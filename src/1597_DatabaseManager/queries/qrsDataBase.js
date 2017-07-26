"use strict";

var HttpError = require('http-error-constructor');
var R = require('ramda');

var LandingPlanes = require('../models').LandingPlanes;


const getDbEntries = (geoPolygon) => {

    var excludeList = {};  //add fields to include with name:0 ...
    var queryObject;

    queryObject = {
        "geoJSON.geometry": {
            $geoIntersects: {
                $geometry: geoPolygon
            }
        }
    };


    console.log("Database query for this region:");
    console.log(JSON.stringify(geoPolygon, null, 2));

    //TODO hier jetzt schnell die Datenbank abfragen und dann geht's vielleicht schon...

    return LandingPlanes.find(queryObject, excludeList);

};

module.exports = {
    getDbEntries: getDbEntries,
};
