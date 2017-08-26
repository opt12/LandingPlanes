"use strict";
var HttpError = require('http-error-constructor');
var R = require('ramda');

var turfUnion = require('@turf/union');
var turfSimplify = require('@turf/simplify');

var LandingPlanes = require('../models').LandingPlanes;
var qrsDataBase = require('../queries/qrsDataBase');
var cmdDataBase = require('../commands/cmdDataBase');

const mergeLandingPlanes = (planesToMerge) => {

    if (planesToMerge.length < 2) return planesToMerge;

    let [union, ...rest] = planesToMerge.map(p => p.geoJSON);

    rest.forEach((plane, idx) => union = turfUnion(union, plane));

    union = turfSimplify(union, 0.00001, false);

    //now get the minimum length of all merged planes
    let minLength = Math.min.apply(Math, R.map(pl => pl.geoJSON.properties.actualLength)(planesToMerge));
    let maxVariance = Math.max.apply(Math, R.map(pl => pl.geoJSON.properties.actualVariance)(planesToMerge));
    let maxRise = Math.max.apply(Math, R.map(pl => pl.geoJSON.properties.actualRise)(planesToMerge));

    union.properties.actualLength = minLength;
    union.properties.actualVariance = maxVariance;
    union.properties.actualRise = maxRise;

    return union;
};

async function cleanUpDbWithPlaneMerging() {

    /* solange noch mindestens eine Bahn gefunden wird mit
     * - mergeable === true;
     * - mergePass === false;
     */
    let nextPlane;
    let intersecting;
    let newMergedPlane = {};

    try {
        nextPlane = await qrsDataBase.getUnmergedPlane();
    } catch (error) {
        console.log("Something went terribly wrong:");
        console.log(error);
        throw(error);
    }

    while (nextPlane !== null) {

        // console.log("unmerged Plane gefunden: ", JSON.stringify(nextPlane, null, 2));

        //Suche alle intersecting Planes mit der gleichen Richtung
        try {
            intersecting = await qrsDataBase.getUnmergedPlanesWithHeading(nextPlane.geoJSON.geometry, nextPlane.geoJSON.properties.actualHeading);

            //Partitioniere das intersecting array in "Basisbahnen" und solche, die aus einem alten Merge hervorgegangen waren
            let [intersectingMerged, intersectingBase] = R.partition(pl => pl.geoJSON.properties.isMergeResult)(intersecting);

            if (intersecting.length == 1) {
                //es gibt nur eine Bahn in dem Bereich
                if (!intersecting[0].geoJSON.properties.isMergeResult) {
                    //das ist eine "neue" Bahn, die als .isMergeResult angelegt werden muss
                    newMergedPlane = intersecting[0].geoJSON;
                    await cmdDataBase.handleAddMergedPlane(newMergedPlane);
                    //setze die Property der übrigen .geoJSON.properties.mergePass auf true, damit merged Bahnen nicht immer wieder betrachtet werden
                    await Promise.all(intersectingBase.map(int => cmdDataBase.handleSetProperty(int._id, 'geoJSON.properties.mergePass', true)));
                } else {
                    //setze die Property .geoJSON.properties.mergePass von allen auf true, damit merged Bahnen nicht immer wieder betrachtet werden
                    await Promise.all(intersecting.map(int => cmdDataBase.handleSetProperty(int._id, 'geoJSON.properties.mergePass', true)));
                }
            } else {
                try {
                    newMergedPlane = mergeLandingPlanes(intersecting);
                    await cmdDataBase.handleAddMergedPlane(newMergedPlane);
                    //lösche Bahnen, die selber aus einem Merge hervorgegangen waren. Die werden nicht mehr gebraucht:
                    await Promise.all(intersectingMerged.map(int => cmdDataBase.handleDeletePlaneById(int._id)));
                    //setze die Property der übrigen .geoJSON.properties.mergePass auf true, damit merged Bahnen nicht immer wieder betrachtet werden
                    await Promise.all(intersectingBase.map(int => cmdDataBase.handleSetProperty(int._id, 'geoJSON.properties.mergePass', true)));
                } catch (error){
                    //from time to time, turf.union fails with TypeError: this.seg.p1.equals2D is not a function
                    console.log("An error  occurred with turf.union: ", error);
                    console.log("swallow that error silently and try to recover. No better idea for the moment");
                    //mark the affected planes as already mergePass:true
                    //setze die Property .geoJSON.properties.mergePass von allen auf true, damit merged Bahnen nicht immer wieder betrachtet werden
                    await Promise.all(intersecting.map(int => cmdDataBase.handleSetProperty(int._id, 'geoJSON.properties.mergePass', true)));
                }
            }

            nextPlane = await qrsDataBase.getUnmergedPlane();
        } catch (error) {
            console.log("Something went terribly wrong:");
            console.log(error);
            throw(error);
        }
    }

    console.log("No more planes to merge");
    return;
};

module.exports = {
    cleanUp: cleanUpDbWithPlaneMerging,
};

