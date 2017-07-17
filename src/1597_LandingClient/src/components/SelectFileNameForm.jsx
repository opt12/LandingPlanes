import React, {} from 'react';
import PropTypes from 'prop-types';

import {Form, FormGroup, FormControl, ControlLabel} from 'react-bootstrap'


const SelectFileNameForm = ({filename, getFileInfo, processingState}) => {
    console.log("selectFilenameForm loaded with filename = " + filename);

    return (
        <div>
            {!(processingState === "fetchedFileExtent") &&
            <div><h1>Wähle GeoTIFF Datei aus!</h1></div>
            }
            {(processingState === "fetchedFileExtent") &&
            <div>
                <h1>Willkommen zur Landing Planes Search Engine </h1>
                <h2> Aktuelle Datei ist: {filename}</h2>
            </div>}
            <div>
                <Form horizontal>
                    <ControlLabel class="col-sm-offset-0 col-sm-2">GeoTIFF:</ControlLabel>
                    {' '}
                    <FormControl class="col-sm-offset-0 col-sm-8" id="GeoTIFFFileSelector" type="file"/>
                    <button class="col-sm-offset-0 col-sm-2 btn btn-primary"
                            onClick={e => {
                                filename = document.getElementById('GeoTIFFFileSelector').value.replace(/.*[\/\\]/, '');
                                console.log("value: " + filename);
                                getFileInfo(e, filename)
                            }}>
                        Get File Info
                    </button>
                </Form>
            </div>
        </div>
    );
};


export default SelectFileNameForm;
