import {connect} from 'react-redux';

import getFileinfo from '../actions/getFileinfo';

import React, {Component,} from 'react';
import PropTypes from 'prop-types';


import SelectFileNameForm from './SelectFileNameForm';
import Map from './Map';

const Startpage = (props) => {
    console.log("Startpage props are: ", props);
    return (
        <div>
            <div>
                <SelectFileNameForm filename={props.tiffinfo.filename} getFileInfo={props.getFileinfo}
                                    processingState={props.tiffinfo.processingState}/>
            </div>
            {(props.tiffinfo.processingState === "fetchedFileExtent") &&
            <Map  />
            }
        </div>
    );
};

const mapStateToProps = state => ({
    tiffinfo: state.tiffinfo,
});

const mapDispatchToProps = dispatch => ({
    getFileinfo: (e, filename) => {
        e.preventDefault();
        dispatch(getFileinfo(filename));
    }
});

Startpage.propTypes = {
    tiffinfo: PropTypes.object.isRequired,
    getFileinfo: PropTypes.func.isRequired,
};


export default connect(mapStateToProps, mapDispatchToProps)(Startpage);
