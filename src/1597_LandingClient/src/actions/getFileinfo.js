import * as types from './types';
import * as api from '../utils/api';

const requestGetFileInfo = (filename) => ({
    type: types.REQUEST_GET_FILEINFO,
    filename: filename,
});

const getFileInfoSucceeded = (filename, fileinfo) => ({
    type: types.GET_FILEINFO_SUCCEEDED,
    filename: filename,
    fileinfo: fileinfo,
});

const getFileInfoFailed = () => ({
    type: types.GET_FILEINFO_FAILED,
});

const requestGetFileExtent = (filename) => ({
    type: types.REQUEST_GET_FILEEXTENT,
    filename: filename,
});

const getFileExtentSucceeded = (fileextent) => ({
    type: types.GET_FILEEXTENT_SUCCEEDED,
    fileextent: fileextent,
});

const getFileExtentFailed = () => ({
    type: types.GET_FILEEXTENT_FAILED,
});

const getFileInfo = (filename) =>
    (dispatch) => {
        dispatch(requestGetFileInfo(filename));

        return api.getFileInfo(filename)
            .then(
                fileinfo => {
                    console.log("received fileinfo: ", fileinfo);
                    dispatch(getFileInfoSucceeded(filename, fileinfo));
                    dispatch(requestGetFileExtent());
                    return api.getFileExtent(filename)
                        .then(
                            fileextent => {
                                console.log("received file extents: ", fileextent);
                                dispatch(getFileExtentSucceeded(fileextent))
                            },
                            (errorResponse) => {
                                console.log("getFileExtent failed: ", errorResponse);
                                dispatch(getFileExtentFailed())
                            });
                },
                (errorResponse) => {
                    console.log("getFileInfo failed: ", errorResponse);
                    dispatch(getFileInfoFailed())
                })
            .catch((error) => {
                dispatch(getFileInfoFailed());
                console.log(`Other error: Fetching file info for ${filename} failed. Try again.`)
                console.log('error: ', error);
            })
    };


export default getFileInfo;
