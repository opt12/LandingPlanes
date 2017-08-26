import * as types from './types';
import * as api from '../utils/api';

const requestGetDbEntries = (geoPolygon) => ({
    type: types.REQUEST_GET_DB_ENTRIES,
    geoPolygon: geoPolygon,
});

const getDbEntriesSucceeded = (landingPlanes) => ({
    type: types.GET_DB_ENTRIES_SUCCEEDED,
    landingPlanes: landingPlanes,
});

const getDbEntriesFailed = () => ({
    type: types.GET_DB_ENTRIES_FAILED,
});

const requestGetMinVarianceDbEntries = (geoPolygon) => ({
    type: types.REQUEST_GET_MIN_VARIANCE_DB_ENTRIES,
    geoPolygon: geoPolygon,
});

const getMinVarianceDbEntriesSucceeded = (landingPlanes) => ({
    type: types.GET_MIN_VARIANCE_DB_ENTRIES_SUCCEEDED,
    minVarianceLandingPlanes: landingPlanes,
});

const getMinVarianceDbEntriesFailed = () => ({
    type: types.GET_MIN_VARIANCE_DB_ENTRIES_FAILED,
});

const requestDropDb = () => ({
    type: types.REQUEST_DROP_DB,
});

const dropDbSucceeded = () => ({
    type: types.DROP_DB_SUCCEEDED,
});

const dropDbFailed = () => ({
    type: types.DROP_DB_FAILED,
});


const queryLandingPlanesDB = (geoPolygon) =>
    (dispatch) => {
        dispatch(requestGetDbEntries(geoPolygon));

        return api.requestGetDbEntries(geoPolygon)
            .then(
                landingPlanes => {
                    console.log("received landing planes from Database: ", landingPlanes);
                    dispatch(getDbEntriesSucceeded(landingPlanes));
                },
                (errorResponse) => {
                    console.log("getDbEntries failed: ", errorResponse);
                    dispatch(getDbEntriesFailed())
                })
            .catch((error) => {
                dispatch(getDbEntriesFailed());
                console.log(`Other error: Fetching landing Planes for \n${geoPolygon}\n failed. Try again.`);
                console.log('error: ', error);
            })
    };

const queryMinVarianceLandingPlanesDB = (geoPolygon) =>
    (dispatch) => {
        dispatch(requestGetMinVarianceDbEntries(geoPolygon));

        return api.requestGetMinVarianceDbEntries(geoPolygon)
            .then(
                landingPlanesMinVariance => {
                    console.log("received MinVariance landing planes from Database: ", landingPlanesMinVariance);
                    dispatch(getMinVarianceDbEntriesSucceeded(landingPlanesMinVariance));
                },
                (errorResponse) => {
                    console.log("getMinVarianceDbEntries failed: ", errorResponse);
                    dispatch(getMinVarianceDbEntriesFailed())
                })
            .catch((error) => {
                dispatch(getMinVarianceDbEntriesFailed());
                console.log(`Other error: Fetching MinVariance landing Planes for \n${geoPolygon}\n failed. Try again.`);
                console.log('error: ', error);
            })
    };


const dropLandingPlanesDB = () =>
    (dispatch) => {
        dispatch(requestDropDb());

        return api.requestDropDb()
            .then(
                () => {
                    console.log("dropped Landing Planes Database: ");
                    dispatch(dropDbSucceeded()); //reset the state
                },
                (errorResponse) => {
                    console.log("dropping Landing Planes Database failed: ", errorResponse);
                    dispatch(dropDbFailed())
                })
            .catch((error) => {
                dispatch(dropDbFailed());
                console.log(`Other error: dropping Landing Planes Database failed. Try again.`);
                console.log('error: ', error);
            })
    };

export {
    queryLandingPlanesDB,
    queryMinVarianceLandingPlanesDB,
    dropLandingPlanesDB,
};