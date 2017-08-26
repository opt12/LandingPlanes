"use strict";

var express = require('express');
var router = express.Router();
var getDbEntries = require('../queries/qrsDataBase').getDbEntries;
var getDbEntriesWithMinVariance = require('../queries/qrsDataBase').getDbEntriesWithMinVariance;

/* POST database query. */
router.post('/', function (req, res, next) {
    let geoPolygon = req.body;
    let result;

    getDbEntries(geoPolygon)
        .then(result => {
            console.log("Query yielded "+result.length+" entries in this region.");
            res.json(result);
        });

});

router.post('/bestPlanes', function (req, res, next) {
    let geoPolygon = req.body;
    let result;

    getDbEntriesWithMinVariance(geoPolygon)
        .then(result => {
            console.log("Query yielded "+result.length+" minimum variance entries in this region.");
            res.json(result);
        });

});



module.exports = router;
