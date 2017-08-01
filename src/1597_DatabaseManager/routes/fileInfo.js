var express = require('express');
var router = express.Router();

var getGDALInfo = require('../commands/cmdFileInfo').getGDALInfo;
var getWGS84ExtentPolygon = require('../commands/cmdFileInfo').getWGS84ExtentPolygon;


/* GET fileInfo for Tiff file as JSON. */
router.post('/', function (req, res, next) {

    getGDALInfo(req.body.filename)
        .then((results) =>
            res.json(results))
        .catch((error) => {
            console.log('Fehler: ', error);
            next(error);
        });
});

/* GET the convex polygon describing the extent on the map. */
router.post('/extent', function (req, res, next) {

    getWGS84ExtentPolygon(req.body.filename)
        .then((results) =>
            res.json(results))
        .catch((error) => {
            console.log('Fehler: ', error);
            next(error);
        });
});




module.exports = router;
