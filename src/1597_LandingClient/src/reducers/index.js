/* eslint-disable indent */
import {combineReducers} from 'redux';
import {routerReducer as routing} from 'react-router-redux';

import fileinfo from './fileinfo';

const rootReducer = combineReducers({
    tiffinfo: fileinfo,
    routing,
});

export default rootReducer;
