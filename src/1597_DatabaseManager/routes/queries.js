"use strict";

var express = require('express');
var router = express.Router();
var getDbEntries = require('../queries/qrsDataBase').getDbEntries;
var getDbEntriesWithMinVariance = require('../queries/qrsDataBase').getDbEntriesWithMinVariance;

/* POST database query. */
router.post('/', function (req, res, next) {
    let geoPolygon = req.body.geoPolygon;
    let showMergedAreas = req.body.showMergedAreas;
    let showMinVariancePlanes = req.body.showMinVariancePlanes ;
    let result;

    getDbEntries(geoPolygon, showMergedAreas, showMinVariancePlanes)
        .then(result => {
            console.log("Query yielded "+result.length+" entries in this region.");
            res.json(result);
        });

});

module.exports = router;
