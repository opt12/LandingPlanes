import createReducer from './utils';
import * as types from '../actions/types';

const initialState = {
    processingState: 'initial',
    geoPolygon: {},
    landingPlanes: [],
    minVarianceLandingPlanes: [],
    cnt: 0,
};

const requestGetDbEntries = (state, action) => {
    return {
        ...state,
        processingState: 'requestingDbEntries',
        geoPolygon: action.geoPolygon,
    };
};

const getDbEntriesSucceeded = (state, action) => {
    return {
        ...state,
        processingState: 'fetchedDbEntries',
        landingPlanes: action.landingPlanes,
        minVarianceLandingPlanes: [],
        cnt: state.cnt+1,
    };
};

const getDbEntriesFailed = (state, action) => {
    return {
        ...initialState,
        processingState: 'failedfetchingDbEntries',
    };
};

const requestGetMinVarianceDbEntries = (state, action) => {
    return {
        ...state,
        processingState: 'requestingMinVarianceDbEntries',
        geoPolygon: action.geoPolygon,
    };
};

const getMinVarianceDbEntriesSucceeded = (state, action) => {
    return {
        ...state,
        processingState: 'fetchedMinVarianceDbEntries',
        minVarianceLandingPlanes: action.minVarianceLandingPlanes,
        cnt: state.cnt+1,
    };
};

const getMinVarianceDbEntriesFailed = (state, action) => {
    return {
        ...initialState,
        processingState: 'failedfetchingMinVarianceDbEntries',
    };
};


const requestDropDb = (state) => {
    return {
        ...state,
        processingState: 'requestingDropDb',
    };
};

const dropDbSucceeded = () => {
    return {
        ...initialState,
        processingState: 'droppedLandingPlanesDb',
    };
};

const dropDbFailed = (state, action) => {
    return {
        ...initialState,
        processingState: 'failedDroppingLandingPlanesDb',
    };
};

export const getDbEntriesReducer = createReducer(initialState, {
    [types.REQUEST_GET_DB_ENTRIES]: requestGetDbEntries,
    [types.GET_DB_ENTRIES_SUCCEEDED]: getDbEntriesSucceeded,
    [types.GET_DB_ENTRIES_FAILED]: getDbEntriesFailed,
    [types.REQUEST_GET_MIN_VARIANCE_DB_ENTRIES]: requestGetMinVarianceDbEntries,
    [types.GET_MIN_VARIANCE_DB_ENTRIES_SUCCEEDED]: getMinVarianceDbEntriesSucceeded,
    [types.GET_MIN_VARIANCE_DB_ENTRIES_FAILED]: getMinVarianceDbEntriesFailed,
    [types.REQUEST_DROP_DB]: requestDropDb,
    [types.DROP_DB_SUCCEEDED]: dropDbSucceeded,
    [types.DROP_DB_FAILED]: dropDbFailed,
});

export default getDbEntriesReducer;
