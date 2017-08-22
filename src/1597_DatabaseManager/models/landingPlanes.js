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
    actualRise: {
        type: Number,
        required: true,
    },
    actualVariance: {
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
    maxVariance: {
        type: Number,
        required: true,
    },
    maxRise: {
        type: Number,
        required: true,
    },
    mergeable: {
        type: Boolean,
        required: true,
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
    }
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
// var LandingPlanes = connection.model('LandingPlanes', planesSchema);
var LandingPlanes = connection.model('LandingPlanes', schema);

//make this available to our Node aplication
module.exports = LandingPlanes;