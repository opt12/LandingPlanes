import * as types from './types';
import * as api from '../utils/api';


const scanForLandingPlanes = (tiffInfo, mapExtent, scanParameters, scanHeadings) => {
    let scanTask = {
        type: "SCAN",
        data: {
            TiffFileName: tiffInfo.fileInfo.files[0],
            mapExtent: mapExtent,
            scanParameters: scanParameters,
            scanHeadings: scanHeadings,
        },
    };

    return api.sendTask2Server(scanTask)
        .then(retVal => {
                console.log("scan task sent to server: ", retVal);
            },
            (errorResponse) => {
                console.log("scan task sent to server failed: ", errorResponse);
            })
};

const save2MFileOnServer = (tiffInfo, mapExtent, mFileName) => {
    let scanTask = {
        type: "SAVE_2_M_FILE",
        data: {
            TiffFileName: tiffInfo.fileInfo.files[0],
            mapExtent: mapExtent,
            mFileName: mFileName
        },
    };

    return api.sendTask2Server(scanTask)
        .then(retVal => {
                console.log("scan task sent to server: ", retVal);
            },
            (errorResponse) => {
                console.log("scan task sent to server failed: ", errorResponse);
            })
};

module.exports = {
    scanForLandingPlanes: scanForLandingPlanes,
    save2MFileOnServer: save2MFileOnServer,
};
