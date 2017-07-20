var mongoose = require('mongoose'),
    config = require('../config/config'),
    connectionLandingPlanes;

connectionLandingPlanes = mongoose.createConnection(config.planesDb);

// mongoose.set('debug', true);

mongoose.Promise = global.Promise;

module.exports = {
    connectionLandingPlanes: connectionLandingPlanes,
};




