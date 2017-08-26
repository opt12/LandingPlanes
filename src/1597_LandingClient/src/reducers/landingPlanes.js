import createReducer from './utils';
import * as types from '../actions/types';

const initialState = {
    processingState: 'initial',
    geoPolygon: {},
    landingPlanes: [],
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
        cnt: state.cnt+1,
    };
};

const getDbEntriesFailed = (state, action) => {
    return {
        ...initialState,
        processingState: 'failedfetchingDbEntries',
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
    [types.REQUEST_DROP_DB]: requestDropDb,
    [types.DROP_DB_SUCCEEDED]: dropDbSucceeded,
    [types.DROP_DB_FAILED]: dropDbFailed,
});

export default getDbEntriesReducer;
