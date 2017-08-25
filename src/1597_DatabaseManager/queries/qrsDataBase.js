"use strict";

var HttpError = require('http-error-constructor');
var R = require('ramda');

var turfUnion = require('turf-union');
var turf = require('turf');

var LandingPlanes = require('../models').LandingPlanes;

const mergeLandingPlanes = (planesToMerge) => {

    if(planesToMerge.length < 2) return planesToMerge;

    let [union, ...rest] = planesToMerge.map(p => p.geoJSON);

    rest.forEach((plane, idx) => union = turfUnion(union, plane));

    // union = turf.simplify(union, 0.0001, false);

    return planesToMerge;
}


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

    return LandingPlanes.find(queryObject, excludeList).lean()
        .then(landingPlanes =>{
            //eliminiere Duplikate
            let {mergedPlanes, planesToDelete} = mergeLandingPlanes(landingPlanes);
            return landingPlanes;
        });

};

const getDbEntriesWithHeading = (geoPolygon, heading) => {

    var excludeList = {};  //add fields to include with name:0 ...
    var queryObject;

    queryObject = {
        "geoJSON.geometry": {
            $geoIntersects: {
                $geometry: geoPolygon
            }
        },
        "geoJSON.properties.actualHeading":heading,
        "geoJSON.properties.mergeable":true,
    };


    console.log("Database query for Heading "+heading+" intersecting this region:");
    console.log(JSON.stringify(geoPolygon, null, 2));

    //TODO hier jetzt schnell die Datenbank abfragen und dann geht's vielleicht schon...

    return LandingPlanes.find(queryObject, excludeList);

};


module.exports = {
    getDbEntries: getDbEntries,
    getDbEntriesWithHeading: getDbEntriesWithHeading,
};
