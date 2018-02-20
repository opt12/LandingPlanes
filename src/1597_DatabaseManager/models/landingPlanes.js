var mongoose = require('mongoose');
var Schema = mongoose.Schema;
var connection = require('../data-access/mongoose').connectionLandingPlanes;


var propertiesSchema = new Schema({
    actualHeading: {
        type: Number,
        required: true,
    },
    actualLength: {
        type: Number,
        required: true,
    },
    actualSlope: {
        type: Number,
        required: true,
    },
    actualVariance: {
        type: Number,
        required: true,
    },
    actualStartElevation: {
        type: Number,
        required: true,
    },
    minLength: {
        type: Number,
        required: true,
    },
    minWidth: {
        type: Number,
        required: true,
    },
    maxShortSlopeLong: {
        type: Number,
        required: true,
    },
    maxShortSlopeTrans: {
        type: Number,
        required: true,
    },
    maxSlope: {
        type: Number,
        required: true,
    },
    mergeable: {    //darf diese Bahn mit anderen gemerged werden um weniger Geometrien zu bekommen
        type: Boolean,  //true: Sie darf zusammengefasst werden
        required: true, //false: es ist eine Minimum-Varianz Bahn innerhalb einer längeren möglichen Bahn. -->Nicht zusammenfassen
    },
    isMergeResult: {    //Wurde diese Bahn im Rahmen eines Merge erzeugt?
        type: Boolean,  //true: ja, ist das Resultat eines Merge
        required: true, //false: Nein, es ist eine originale Bahn
    },
    mergePass: {    //wurde die Bahn im Rahmen eines Merge schon betrachtet?
        type: Boolean,  //true: ja, ist schon in einen Merge eingeflossen
        required: true, //false: Nein, die habe ich zum ersten Mal
    },
}, {_id: false});


var schema = new Schema({
    geoJSON: {
        type: {
            type: String,
        },
        geometry: {
            coordinates: [],
            type: {
                type: String,
            }
        },
        properties: {
            type: propertiesSchema,
        },
    },
},{
    timestamps: true
});
schema.index({"geoJSON.geometry": '2dsphere'});

// var planesSchema = new Schema({
//     feature: new GeoJSON('Feature'),
//     properties: {
//         type: propertiesSchema,
//     },
// });

//the schema is useless so far
//we need to create a model using it
// var LandingPlanes = condnection.model('LandingPlanes', planesSchema);
var LandingPlanes = connection.model('LandingPlanes', schema);
var mergeableLandingPlanes = connection.model('mergeableLandingPlanes', schema);


//make this available to our Node aplication
module.exports = {
    LandingPlanes: LandingPlanes,
    mergeableLandingPlanes: mergeableLandingPlanes,
};