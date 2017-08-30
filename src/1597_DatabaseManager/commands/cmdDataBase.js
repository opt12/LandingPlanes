"use strict";


var HttpError = require('http-error-constructor');
var R = require('ramda');
var turfDistance = require('@turf/distance');
var turfHelpers = require('@turf/helpers');

var LandingPlanes = require('../models').LandingPlanes;


var getDbEntriesWithHeading = require('../queries/qrsDataBase').getDbEntriesWithHeading;


// var GeoJSON = require('mongoose-geojson-schema');
// var mongoose = require('mongoose');
//
// var schema = new mongoose.Schema({
//     polygon: mongoose.Schema.Types.Polygon,
//     feature: mongoose.Schema.Types.Feature,
// });
//
// var db = mongoose.createConnection('localhost', 'test');
// var model = db.model('GeoJSON', schema);

/**
 * Takes care of merging a new landing plane with overlapping planes of the same heading already in the DB.
 *
 * @param params The properties and the geometry of the landing plane
 *
 * @returns Promise([newPlane, planeIdsToDelete]) the new polygon object for the merged plane and an array of planeIds to delete from DB
 */

function getMergedLandingPlane(params) {

    if (!params.properties.mergeable) {
        //obviously, we are not allowed to merge
        return Promise.resolve([params, []]);
    }

    //get the planes intersecting the new plane
    return getDbEntriesWithHeading(params.geometry, params.properties.actualHeading)
        .then(intersectingPlanes => {
            //merge the new and the old planes to a new object
            console.log("found intersecting planes", intersectingPlanes);

            //merge the outline
            //set the parameters correctly

            //create the planeId list to delete from the DB

            //return the new parameters for creation and the IDs for deletion

            return [params, intersectingPlanes];
        });
}


/**
 * Takes care of creating a new LandingPlane.
 * checks if the new plane is marked mergeable. In that case it is merged with other
 * mergeable planes with the same heading that are already in the DB
 *
 *
 * @param params The properties and the geometry of the landing plane
 * @returns {*}
 */
function handleCreateLandingPlane(params) {
    function checkCreateLandingPlaneParameter(params) {
        //TODO: Verification einbauen
        return (!R.isEmpty(params.feature) && !R.isEmpty(params.properties));
    }

    function getActualLengthFromCoordinates(coords) {
        var point1 = turfHelpers.point(coords[0]); //bottom_left
        var point2 = turfHelpers.point(coords[3]); //top left
        var units = "kilometers";

        let distance = turfDistance(point1, point2, units);
        return Math.round(distance * 1000);   //length in meters
    }

    if (!checkCreateLandingPlaneParameter(params)) {
        return Promise.reject(new HttpError(400, `Landing Plane data invalid`));
    }

    //inzwischen kommt die Länge der Landebahn im GeojSOn der Durchmusterung mit.
    //TODO aus zu untersuchenden Gründen ist die Länge abweichend von der hier möglichen Berechnung. :-(
    //aktuell wird die Landebahnlönmge noch nicht korrekt gesetzt. Dann rechne ich die halt aus.
    // params.properties.actualLength = getActualLengthFromCoordinates(params.geometry.coordinates[0]);


    //standardmässig mergen wir. Die Bahnen mit der besten Varianz werden explizit mit
    // 'mergeable': 'false', angeliefert
    params.properties.mergeable = R.propOr('true', 'mergeable')(params.properties) === 'true' ? true : false; //Achtung: Wert mergeable als String
    params.properties.mergePass = false;    //wurde die Bahn im Rahmen eines Merge schon betrachtet?
    params.properties.isMergeResult = false;   //Wurde diese Bahn im Rahmen eines Merge erzeugt? Zunächst natürlich nicht!

    let newLandingPlane = new LandingPlanes({
        //TODO hier muss natürlich noch tiefer runter zugewiesen werden um keine falsch geformten Datensätze zu speichern
        geoJSON: {
            type: "Feature",
            geometry: params.geometry,
            properties: params.properties,
        }
    });


    //the test of the params passed
    return LandingPlanes.create(newLandingPlane)
        .then(
            (landingPlane) => {
                // console.log("created new newLandingPlane:\n", JSON.stringify(landingPlane.toObject().geoJSON, null, 2));
                return {result: newLandingPlane, newPlane: true};
            },
            (error) => {
                return Promise.reject(new HttpError(500, 'Unknown error while trying to store new landing plane\n' + error.message));
            });
}

function handleDropDb(params) {

    return LandingPlanes.remove({}, function (err) {
        console.log('collection removed from Database')
    });
}

const handleSetProperty = (planeId, propertyString, newValue) => {
    let setData = {};
    setData[propertyString] = newValue;
    return LandingPlanes.findByIdAndUpdate(planeId, {$set: setData}, {new: true});
};

const handleAddMergedPlane = (newPlane) => {

    newPlane.properties.isMergeResult = true;
    newPlane.properties.mergeable = true;
    newPlane.properties.mergePass = false;

    // console.log("add merged plane: ", JSON.stringify(newPlane, null, 2));

    return LandingPlanes.create({geoJSON: newPlane})
        .then(
            (landingPlane) => {
                // console.log("created new newLandingPlane:\n", JSON.stringify(landingPlane.toObject().geoJSON, null, 2));
                return landingPlane;
            },
            (error) => {
                return Promise.reject(new HttpError(500, 'Unknown error while trying to store new merged landing plane\n' + error.message));
            });
};

const handleDeletePlaneById = (planeId) => {
    return LandingPlanes.findByIdAndRemove(planeId);
};

module.exports = {
    handleCreateLandingPlane: handleCreateLandingPlane,
    handleDropDb: handleDropDb,
    handleSetProperty: handleSetProperty,
    handleAddMergedPlane: handleAddMergedPlane,
    handleDeletePlaneById: handleDeletePlaneById,
};
