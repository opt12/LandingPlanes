"use strict";

var HttpError = require('http-error-constructor');
var R = require('ramda');

var LandingPlanes = require('../models').LandingPlanes;

const getDbEntries = (geoPolygon, showMergedAreas, showMinVariancePlanes) => {

    let excludeList = {};  //add fields to include with name:0 ...

    let queryObjectGeometry = {
        "geoJSON.geometry": {
            $geoIntersects: {
                $geometry: geoPolygon
            },
        },
    };

    let queryObjectUntouchedRawPlanes = {
        "geoJSON.properties.mergeable": true,   //zuerst alle Bahnen, die noch nicht weiter verarbeitet wurden
        "geoJSON.properties.isMergeResult": false,
        "geoJSON.properties.mergePass": false,
    };

    let queryObjectMergedPlanes = {
        "geoJSON.properties.isMergeResult": true,   //und jetzt noch alle Bereiche, die durch Merge entstanden sind
    };

    let queryObjectRawPlanes = {
        "geoJSON.properties.mergeable": true,   //alle Bahnen, die keine MinVarianz Bahnen sind
        "geoJSON.properties.isMergeResult": false,  //und nicht durch Merge entstanden sind
    };

    let queryObjectMinimumVarianceRawPlanes = {
        "geoJSON.properties.mergeable": false,   //die Bahnen, die nicht gemerged werden dürfen sind die minimum Varianz Bahnen
        "geoJSON.properties.isMergeResult": false,
    };

    let queryOrArray=[];

    if(showMergedAreas){
        queryOrArray.push(queryObjectUntouchedRawPlanes, queryObjectMergedPlanes);
    } else {
        queryOrArray.push(queryObjectRawPlanes)
    }

    if(showMinVariancePlanes)
        queryOrArray.push(queryObjectMinimumVarianceRawPlanes);



    console.log("Database query for this region:");
    console.log(JSON.stringify(geoPolygon, null, 2));

    return LandingPlanes.find({$and:[queryObjectGeometry, {$or:queryOrArray}]}, excludeList).lean()
        .then(landingPlanes => {
            return landingPlanes;
        });

};


const getUnmergedPlanesWithHeading = (geoPolygon, heading) => {

    var excludeList = {};  //add fields to include with name:0 ...

    let queryObjectGeometry= {
        "geoJSON.geometry": {
            $geoIntersects: {
                $geometry: geoPolygon
            }
        },
    };

    let queryObjectBasePlanes = {
        "geoJSON.properties.actualHeading": heading,
        "geoJSON.properties.mergeable": true,
        "geoJSON.properties.mergePass": false,
        "geoJSON.properties.isMergeResult": false,
    };

    let queryObjectMergedPlanes = {
        "geoJSON.properties.actualHeading": heading,
        "geoJSON.properties.mergeable": true,
        // "geoJSON.properties.mergePass": false,
        "geoJSON.properties.isMergeResult": true,
    };



    // console.log("Database query for Heading " + heading + " intersecting this region:");
    // console.log(JSON.stringify(geoPolygon, null, 2));

    return LandingPlanes.find({$and:[queryObjectGeometry, {$or:[queryObjectMergedPlanes, queryObjectBasePlanes]}]}, excludeList).lean();

};


const getUnmergedPlane = () => {

    var excludeList = {};  //add fields to include with name:0 ...
    var queryObject;

    queryObject = {
        "geoJSON.properties.mergeable": true,
        "geoJSON.properties.mergePass": false,
    };


    return LandingPlanes.findOne(queryObject, excludeList).lean()
};


module.exports = {
    getDbEntries: getDbEntries,
    getUnmergedPlanesWithHeading: getUnmergedPlanesWithHeading,
    getUnmergedPlane: getUnmergedPlane,
};
