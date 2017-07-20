var GeoJSON = require('mongoose-geojson-schema');
var mongoose = require('mongoose');
var Schema = mongoose.Schema;
var connection = require('../data-access/mongoose').connectionLandingPlanes;

var propertiesSchema = new Schema({
    heading: {
        type: Number,
        required:true,
    },
    minimumLength: {
        type: Number,
        required:true,
    },
    minimumWidth: {
        type: Number,
        required:true,
    },
    maxVariance: {
        type: Number,
        required:true,
    },
    maxRise: {
        type: Number,
        required:true,
    },
}, {_id: false});


var planesSchema = new Schema({
    _id: {
        type: String,
        required: true,
        index: true,
        unique: true
    },
    feature: mongoose.Schema.Types.Feature,
    properties: {
        type: propertiesSchema,
    },
});

//the schema is useless so far
//we need to create a model using it
var LandingPlanes = connection.model('LandingPlanes', planesSchema);

//make this available to our Node aplication
module.exports = LandingPlanes;