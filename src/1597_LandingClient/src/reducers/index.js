/* eslint-disable indent */
import {combineReducers} from 'redux';
import {routerReducer as routing} from 'react-router-redux';

import fileinfo from './fileinfo';
import landingPlanes from './landingPlanes';

const rootReducer = combineReducers({
    tiffinfo: fileinfo,
    landingPlanes: landingPlanes,
    routing,
});

export default rootReducer;
