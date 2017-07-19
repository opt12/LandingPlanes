import {connect} from 'react-redux';
import React, {Component} from 'react';
import {Map, TileLayer, Marker, Popup, Polygon} from 'react-leaflet';
import {Form, ControlLabel, FormControl, FormGroup, Col, Row} from 'react-bootstrap'
import {scanForLandingPlanes, save2MFileOnServer} from '../actions/sendTask2Server';


const turfInv = require('turf-invariant');
const turfCenter = require('turf-center');
const bbox = require('turf-bbox');

class MapOverview extends Component {
    constructor(props) {
        super(props);

        this.centerLatLng = this.getCenterOfMap(props.extent);
        this.boundingBox = this.getBoundingBox(props.extent);
        this.extentPolygon = this.getExtentPolygon(props.extent);

        this.getCenterOfMap = this.getCenterOfMap.bind(this);
        this.getBoundingBox = this.getBoundingBox.bind(this);
        this.getExtentPolygon = this.getExtentPolygon.bind(this);

        this.state = {
            topLeftLat: this.boundingBox[1][0],
            topLeftLng: this.boundingBox[0][1],
            botRightLat: this.boundingBox[0][0],
            botRightLng: this.boundingBox[1][1],

        };

    }

    getCenterOfMap(extent) {
        const centerOfMap = turfInv.getCoord(turfCenter(extent));
        console.log("center Of Map is: ", centerOfMap);
        return {lat: centerOfMap[1], lng: centerOfMap[0]};
    };

    getBoundingBox(extent) {
        const turfList = bbox(extent);
        return [[turfList[1], turfList[0]], [turfList[3], turfList[2]]];
    }

    getExtentPolygon(extent) {
        const flippedPoints = extent.geometry.coordinates[0];
        return flippedPoints.map(pt => ([pt[1], pt[0]]));
    }

    moveend = (e) => {
        const zoomText = e.target.getZoom();
        console.log("Zoom: " + zoomText);
        const mapBounds = e.target.getBounds();
        const area = {
            topLeftLat: mapBounds.getNorth(),
            topLeftLng: mapBounds.getWest(),
            botRightLat: mapBounds.getSouth(),
            botRightLng: mapBounds.getEast(),
        };
        this.setState(area);
    };

    startScan = (e, scanParameter, scanHeadings) => {
        e.preventDefault();
        let mapExtent = {
            topLeftLat: this.state.topLeftLat,
            topLeftLng: this.state.topLeftLng,
            botRightLat: this.state.botRightLat,
            botRightLng: this.state.botRightLng,
        };

        console.log("sending Scan request to server;");
        console.log("tiffinfo: ", this.props.tiffInfo);
        console.log("mapExtent: ", mapExtent);
        scanForLandingPlanes(this.props.tiffInfo, mapExtent, scanParameter, scanHeadings);
    };

    saveToMFile = (e, mFileName) => {
        e.preventDefault();
        let mapExtent = {
            topLeftLat: this.state.topLeftLat,
            topLeftLng: this.state.topLeftLng,
            botRightLat: this.state.botRightLat,
            botRightLng: this.state.botRightLng,
        };

        console.log("sending Scan request to server;");
        console.log("tiffinfo: ", this.props.tiffInfo);
        console.log("mapExtent: ", mapExtent);
        save2MFileOnServer(this.props.tiffInfo, mapExtent, mFileName);
    };


    render() {
        const {extent} = this.props;
        const position = [this.centerLatLng.lat, this.centerLatLng.lng];
        let mFileName;

        return (
            <div>
                <Map bounds={this.boundingBox} draw="true" onMoveend={this.moveend} onLoad={this.moveend}>
                    <TileLayer
                        attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
                        url='http://{s}.tile.osm.org/{z}/{x}/{y}.png'
                    />
                    <Marker position={position}>
                        <Popup>
                            <span>Center of the Map:<br/>Lat: {position[0]}<br/>Long: {position[1]}</span>
                        </Popup>
                    </Marker>
                    <Polygon color="lime" positions={this.extentPolygon}/>
                </Map>
                Map shows: NorthWest: Lat: {this.state.topLeftLat.toFixed(6)},
                Lng: {this.state.topLeftLng.toFixed(6)};
                SouthEast: Lat: {this.state.botRightLat.toFixed(6)}, Lng: {this.state.botRightLng.toFixed(6)};
                <Form horizontal>
                    <FormGroup>
                        <Col componentClass={ControlLabel} sm={1}>min. Länge [m]:</Col>
                        <Col sm={2}>
                            <FormControl id="minLength" type="number" defaultValue="2000"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={1}>max. Steigung [%]:</Col>
                        <Col sm={2}>
                            <FormControl id="maxRise" type="number" defaultValue="10.0"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={1}>max. Varianz:</Col>
                        <Col sm={2}>
                            <FormControl id="maxVariance" type="number" defaultValue="5.5"/>
                        </Col>
                        <Col sm={2}>
                            <button class="col-sm-12 btn btn-primary"
                                    onClick={e => {
                                        let scanParameter = {
                                            minLength: parseInt(document.getElementById('minLength').value),
                                            maxRise: parseFloat(document.getElementById('maxRise').value),
                                            maxVariance: parseFloat(document.getElementById('maxVariance').value),
                                        };
                                        let scanHeadings = [0.0, 30.0, 60.0, 90.0, 22.5, 270.0];
                                        this.startScan(e, scanParameter, scanHeadings);
                                    }}>
                                Scan for Landing Planes
                            </button>
                        </Col>
                    </FormGroup>
                    <FormGroup>
                        <Col componentClass={ControlLabel} sm={2}>Matlab Filename:</Col>
                        <Col sm={2}>
                            <FormControl id="mfileName" type="text" defaultValue="test.m"/>
                        </Col>
                        <Col sm={2}>
                            <button class="col-sm-12 btn btn-primary"
                                    onClick={e => {
                                        mFileName = document.getElementById('mfileName').value.replace(/.*[\/\\]/, '');
                                        let tiffFilePath = this.props.tiffInfo.fileInfo.description;
                                        tiffFilePath = tiffFilePath.substring(0,tiffFilePath.lastIndexOf("/")+1);
                                        console.log("M-file-full PAth: " +tiffFilePath+ mFileName);
                                        this.saveToMFile(e, tiffFilePath+ mFileName);
                                    }}>
                                Save to M-File
                            </button>
                        </Col>
                    </FormGroup>
                </Form>
            </div>
        )
            ;
    }
}

const mapStateToProps = state => ({
    extent: state.tiffinfo.fileExtent,
    tiffInfo: state.tiffinfo,
});


export default connect(mapStateToProps)(MapOverview);