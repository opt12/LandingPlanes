"use strict";


var HttpError = require('http-error-constructor');
var R = require('ramda');

var LandingPlanes = require('../models').LandingPlanes;
var mergeableLandingPlanes = require('../models').mergeableLandingPlanes;


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

    if (!checkCreateLandingPlaneParameter(params)) {
        return Promise.reject(new HttpError(400, `Landing Plane data invalid`));
    }

    let dataAfterMerge; //Eine Promise, die entweder eine singlePlane oder eine mergedPlane mit planeIdsToDelete enthält

    //standardmässig mergen wir. Die Bahnen mit der besten Varianz werden explizit mit
    // 'mergeable': false, angeliefert
    params.properties.mergeable = R.propOr('true', 'mergeable')(params.properties);

    //checke ob die neue Bahn gemerged werden darf, oder nicht
    if (params.properties.mergeable) {
        dataAfterMerge = getMergedLandingPlane(params)
    } else {
        //das ist eine Landebahn, die einzeln gespeichert werden muss
        //aber vorher auf Dublette prüfen
        dataAfterMerge = getSingleLandingPlane(params);
    }

    return dataAfterMerge
        .then(([newPlane, planeIdsToDelete]) => {
            if (newPlane == {}) {
                return {
                    result: {
                        geoJSON: {
                            type: "Feature",
                            geometry: params.geometry,
                            properties: params.properties,
                        }
                    },
                    newPlane: false
                }
            }

            //implicit else

            //TODO und hier würde noch der Merge eingebaut werden.
            let newLandingPlane = new LandingPlanes({
                //TODO hier muss natürlich noch tiefer runter zugewiesen werden um keine falsch geformten Datensätze zu speichern
                geoJSON: {
                    type: "Feature",
                    geometry: params.geometry,
                    properties: params.properties,
                }
            });


            //the test of the params passed
            return mergeableLandingPlanes.create(newLandingPlane).then((landingPlane) => {
                // console.log("created new newLandingPlane:\n", JSON.stringify(landingPlane.toObject().geoJSON, null, 2));
                return {result: newLandingPlane, newPlane: true};
            }, (error) => {
                return Promise.reject(new HttpError(500, 'Unknown error while trying to store new landing plane\n' + error.message));
            })

        });
}

function handleDropDb(params) {

    return LandingPlanes.remove({}, function (err) {
        console.log('collection removed from Database')
    });
}

module.exports = {
    handleCreateLandingPlane: handleCreateLandingPlane,
    handleDropDb: handleDropDb,
};
