"use strict";


var HttpError = require('http-error-constructor');
var R = require('ramda');

var LandingPlanes = require('../models').LandingPlanes;


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
 * Takes care of creating a new Allocation.
 * Checks the input params and in case that the tests are passed creates a new allocation entry in the database.
 * In case of success, the newly created database entry is fed into the callback function.
 *
 * @param params
 * @param {String} params.name
 * @param {String} [params.description]
 * @param {ObjectId} params.ownerId
 * @param {[ParticipantsObject]} [params.participants][]
 * @callback callback
 * @returns {*}
 */
function handleCreateLandingPlane(params) {
    function checkCreateLandingPlaneParameter(params) {
        //TODO: Verfifcation einbauen
        return (!R.isEmpty(params.feature) && !R.isEmpty(params.properties));
    }

    if (!checkCreateLandingPlaneParameter(params)) {
        return Promise.reject(new HttpError(400, `Landing Plane data invalid`));
    }

    let newLandingPlane = new LandingPlanes({
        //TODO hier muss natürlich noch tiefer runter zugewiesen werden um keine falsch geformten datensätze zu speichern
        geoJSON: {
            type: "Feature",
            geometry: params.geometry,
            properties: params.properties,
        }
    });


    //TODO und hier würde noch der Merge eingebaut werden.

    //the test of the params passed
    return LandingPlanes.create(newLandingPlane).then((landingPlane) => {
        // console.log("created new newLandingPlane:\n", JSON.stringify(landingPlane.toObject().geoJSON, null, 2));
        return newLandingPlane;
    }, (error) => {
        return Promise.reject(new HttpError(500, 'Unknown error while trying to store new landing plane\n' + error.message));
    })
}

function handleDropDb(params){

    return LandingPlanes.remove({}, function(err) {
        console.log('collection removed from Database')
    });
}

module.exports = {
    handleCreateLandingPlane: handleCreateLandingPlane,
    handleDropDb: handleDropDb,
};
