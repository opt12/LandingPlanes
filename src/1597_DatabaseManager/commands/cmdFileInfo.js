var exec = require('child_process').exec;
var HttpError = require('http-error-constructor');
var R = require('ramda');

var turfHelpers = require('@turf/helpers');
var turfConvex = require('@turf/convex');


const getGDALInfo = function (fileName) {

    fileName = fileName.replace(/.*[\/\\]/, '');

    var filePath = process.env.TIFFPATH + '/' + fileName;
    console.log("Checking Info for file: ", filePath);

    return new Promise((resolve, reject) => {
        exec('gdalinfo "' + filePath + '" -json', function callback(error, stdout, stderr) {
            if (error) {
                reject(new HttpError(400, stderr));
            }
            else {
                console.log("file info is: ", stdout);
                resolve(JSON.parse(stdout));
            }
        });
    });
};

const getWGS84ExtentPolygon = function (filename) {
    return getGDALInfo(filename)
        .then(resultJSON =>{
            var fourPoints = resultJSON.wgs84Extent.coordinates[0];
            if (fourPoints.length != 5){
                throw new HttpError(400, 'problem with wgs84Extent.coordinates; (supposed to contain 4 points):\n',fourPoints);
            }
            fourPoints = fourPoints.splice(0,4);    //get the non closed polygon;
            var points = fourPoints.map(p => turfHelpers.point(p));
            var hull = turfConvex(turfHelpers.featureCollection(points));

            return hull;
        })
}

module.exports = {
    getGDALInfo: getGDALInfo,
    getWGS84ExtentPolygon: getWGS84ExtentPolygon,
};
