import createReducer from './utils';
import * as types from '../actions/types';

const initialState = {
    processingState: 'initial',
    fileInfo: {},
    filename: 'nothing selected yet',
    fileExtent: {},
};

const requestGetFileInfo = (state, action) => {
    return {
        ...state,
        processingState: 'fetchingFileInfo',
        filename: action.filename,
    };
};

const getFileInfoSucceeded = (state, action) => {
    return {
        ...state,
        processingState: 'fetchedFileInfo',
        fileInfo: action.fileinfo,
        filename: action.filename,
    };
};

const getFileInfoFailed = (state, action) => {
    return {
        ...initialState,
        processingState: 'failed',
        filename: "invlaid filename",
    };
};

const requestGetFileExtent = (state,) => {
    return {
        ...state,
        processingState: 'fetchingFileExtent',
    };
};

const getFileExtentSucceeded = (state, action) => {
    return {
        ...state,
        processingState: 'fetchedFileExtent',
        fileExtent: action.fileextent,
    };
};

const getFileExtentFailed = (state,) => {
    return {
        ...initialState,
        processingState: 'failed',
        filename: "invalid filename",
    };
};

export const getFileinfoReducer = createReducer(initialState, {
    [types.REQUEST_GET_FILEINFO]: requestGetFileInfo,
    [types.GET_FILEINFO_SUCCEEDED]: getFileInfoSucceeded,
    [types.GET_FILEINFO_FAILED]: getFileInfoFailed,
    [types.REQUEST_GET_FILEEXTENT]: requestGetFileExtent,
    [types.GET_FILEEXTENT_SUCCEEDED]: getFileExtentSucceeded,
    [types.GET_FILEEXTENT_FAILED]: getFileExtentFailed,
});

export default getFileinfoReducer;
