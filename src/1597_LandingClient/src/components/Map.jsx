import {connect} from 'react-redux';
import React, {Component} from 'react';
import {Map, TileLayer, Marker, Popup, Polygon, GeoJSON} from 'react-leaflet';
// import {GeoJsonCluster} from 'react-leaflet-geojson-cluster';
import {Form, ControlLabel, FormControl, FormGroup, Col, Checkbox} from 'react-bootstrap'
import {scanForLandingPlanes, save2MFileOnServer} from '../actions/sendTask2Server';
import {map, partition, filter} from 'ramda';


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

        this.queryLandingPlanesDB = props.queryLandingPlanesDB;
        this.dropLandingPlanesDB = props.dropLandingPlanesDB;


        this.state = {
            topLeftLat: this.boundingBox[1][0],
            topLeftLng: this.boundingBox[0][1],
            botRightLat: this.boundingBox[0][0],
            botRightLng: this.boundingBox[1][1],
            showMergedPlanes: true,
            showMinVariance: false,
            viewFilter: [],
            planesFeatureCollectionKey: 0,
        };
    }

    componentDidMount() {
        //fire one first data request after showing up
        this.queryLandingPlanesDB(this.getExtentGeoJSON(), this.state.showMergedPlanes, this.state.showMinVariance)
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

    getExtentGeoJSON = () => (
        {
            type: "Polygon",
            coordinates: [[
                //top left:
                [this.state.topLeftLng, this.state.topLeftLat],
                //bottom left
                [this.state.topLeftLng, this.state.botRightLat],
                //bottom right
                [this.state.botRightLng, this.state.botRightLat],
                //top right
                [this.state.botRightLng, this.state.topLeftLat],
                //top left:
                [this.state.topLeftLng, this.state.topLeftLat],
            ]],
        }
    );

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
        this.queryLandingPlanesDB(this.getExtentGeoJSON(), this.state.showMergedPlanes, this.state.showMinVariance)
    };

    handleChangeChk = (e) => {
        const chkboxes = {
            showMergedPlanes: document.getElementById('showMergedPlanes').checked,
            showMinVariance: document.getElementById('showMinVariance').checked,
            planesFeatureCollectionKey: this.state.planesFeatureCollectionKey+1,
        };
        this.setState(chkboxes, () =>    //update landingPlanes after updating the state
            this.queryLandingPlanesDB(this.getExtentGeoJSON(),
                this.state.showMergedPlanes,
                this.state.showMinVariance));
    };

    changeViewFilter = (e) => {
        e.preventDefault();
        // console.log("New Filter: ", e.target.value);
        if (e.target.value.trim() === "") {
            // console.log("empty filter");
            this.setState({viewFilter: [], planesFeatureCollectionKey: this.state.planesFeatureCollectionKey+1});
        }
        try {
            let filter = JSON.parse(e.target.value);
            filter = filter.constructor === Array ? filter : [filter];
            // console.log("Parsed filter Array = ", filter);
            this.setState({viewFilter: filter, planesFeatureCollectionKey: this.state.planesFeatureCollectionKey+1});
        } catch (e) {
            //obviously there is no valid array in the entry field
            //silently swallow the error and don't touch the current filter
        }
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
        console.log("tiffinfo: ", JSON.stringify(this.props.tiffInfo, null, 2));
        console.log("mapExtent: ", JSON.stringify(mapExtent, null, 2));
        console.log("scanParameter: ", JSON.stringify(scanParameter, null, 2));
        scanForLandingPlanes(this.props.tiffInfo, mapExtent, scanParameter, scanHeadings);
    };

    requestLandingPlanes = () => {
        let requestArea = this.getExtentGeoJSON();
        console.log(JSON.stringify(requestArea));
        this.queryLandingPlanesDB(requestArea, this.state.showMergedPlanes, this.state.showMinVariance);
    };

    dropDb = (e) => {
        e.preventDefault();
        this.dropLandingPlanesDB();
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


    onEachFeature(feature, layer) {
        layer.setStyle({color: feature.properties.mergeable ? "blue" : "green"});
        if (feature.properties.mergeable) {
            //these are merged planes
            layer.bindPopup(`<span>Landing Plane:
                <br/>Heading: ${feature.properties.actualHeading}
                <br/>min. Length: ${feature.properties.actualLength.toFixed(0)}
                <br/>max. Variance: ${feature.properties.actualVariance.toFixed(4)}</span>`)
        } else {
            //these are minimal Variance Runways
            layer.bindPopup(`<span>min. Variance Strip:
                <br/>Heading: ${feature.properties.actualHeading}
                <br/>Length: ${feature.properties.actualLength.toFixed(0)}
                <br/>max. Variance: ${feature.properties.actualVariance.toFixed(4)}</span>`)
        }
    }

    render() {
        const {extent} = this.props;
        const position = [this.centerLatLng.lat, this.centerLatLng.lng];
        let mFileName;

        //partitioniere um erst die Merged Areas und oben drauf die minVarianz zu haben
        let [mergedAreas, rawPlanes] = partition(el => el.geoJSON.properties.mergeable)(this.props.landingPlanes.landingPlanes);

        let planesFeatureCollection = this.state.viewFilter.length > 0 ?
            {
                type: "FeatureCollection",
                features: filter(el => this.state.viewFilter.indexOf(el.properties.actualHeading) != -1)(map(p => p.geoJSON)(mergedAreas.concat(rawPlanes))),
            } : {
                type: "FeatureCollection",
                features: map(p => p.geoJSON)(mergedAreas.concat(rawPlanes)),
            };

        // console.log("planesFeaturecollection: ");
        // console.log(planesFeatureCollection);

        return (
            <div>
                <Map id="map" bounds={this.boundingBox} draw="true" onMoveend={this.moveend} onLoad={this.moveend}>
                    <TileLayer
                        attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
                        url='http://{s}.tile.osm.org/{z}/{x}/{y}.png'
                    />
                    {/*<Marker position={position}>*/}
                        {/*<Popup>*/}
                            {/*<span>Center of the Map:<br/>Lat: {position[0]}<br/>Long: {position[1]}</span>*/}
                        {/*</Popup>*/}
                    {/*</Marker>*/}
                    {/*<Polygon color="lime" positions={this.extentPolygon}/>*/}
                    {planesFeatureCollection.features.length !== 0 &&
                    <GeoJSON key={this.state.planesFeatureCollectionKey + this.props.landingPlanes.cnt}
                             data={planesFeatureCollection}
                             onEachFeature={this.onEachFeature.bind(this)}
                    />}
                </Map>
                Map shows: NorthWest: Lat: {this.state.topLeftLat.toFixed(6)},
                Lng: {this.state.topLeftLng.toFixed(6)};
                SouthEast: Lat: {this.state.botRightLat.toFixed(6)}, Lng: {this.state.botRightLng.toFixed(6)};
                <span
                    class="pull-right"> Amount of Landing Planes in viewport:
                    {planesFeatureCollection.features.length}/{this.props.landingPlanes.landingPlanes.length} </span>

                <Form horizontal>
                    <FormGroup>
                        <Col componentClass={ControlLabel} sm={2}>min. Länge [m]:</Col>
                        <Col sm={2}>
                            <FormControl id="minLength" type="number" defaultValue="2000" step="1"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={2}>min. Breite [m]:</Col>
                        <Col sm={1}>
                            <FormControl id="minWidth" type="number" defaultValue="30" step="1"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={2}>Richtungen [°, °, ]:</Col>
                        <Col sm={3}>
                            <FormControl id="headings" type="text" defaultValue="[45, 135]"/>
                        </Col>
                    </FormGroup>
                    <FormGroup>
                        <Col componentClass={ControlLabel} sm={2}>max. Steigung [%]:</Col>
                        <Col sm={1}>
                            <FormControl id="maxSlope" type="number" defaultValue="5.0" step="0.01"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={2}>max. Steig.: (längs)</Col>
                        <Col sm={1}>
                            <FormControl id="maxShortSlopeLong" type="number" defaultValue="2.1" step="0.01"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={2}>max. Steig.: (quer)</Col>
                        <Col sm={1}>
                            <FormControl id="maxShortSlopeTrans" type="number" defaultValue="3.10" step="0.01"/>
                        </Col>
                        <Col componentClass={ControlLabel} sm={1}>Threads</Col>
                        <Col sm={1}>
                            <FormControl id="numThreads" type="number" defaultValue="8" step="1"/>
                        </Col>

                        <Col sm={1}>
                            <button class="btn btn-primary"
                                    onClick={e => {
                                        let scanParameter = {
                                            minLength: parseInt(document.getElementById('minLength').value),
                                            minWidth: parseInt(document.getElementById('minWidth').value),
                                            maxSlope: parseFloat(document.getElementById('maxSlope').value),
                                            maxShortSlopeLong: parseFloat(document.getElementById('maxShortSlopeLong').value),
                                            maxShortSlopeTrans: parseFloat(document.getElementById('maxShortSlopeTrans').value),
                                            numThreads: parseInt(document.getElementById('numThreads').value),
                                        };
                                        let scanHeadings = JSON.parse(document.getElementById('headings').value);
                                        scanHeadings = scanHeadings.constructor === Array ? scanHeadings : [scanHeadings];

                                        this.startScan(e, scanParameter, scanHeadings);
                                    }}>
                                Scan
                            </button>
                        </Col>
                    </FormGroup>
                    <FormGroup>
                        <Col smOffset={0} sm={2}>
                            <button class="col-sm-12 btn btn-danger"
                                    onClick={e => {
                                        this.dropDb(e);
                                    }}>
                                Drop DB
                            </button>
                        </Col>
                        <Col smOffset={0} sm={3}>
                            <Checkbox inline id="showMergedPlanes"
                                      defaultChecked={this.state.showMergedPlanes}
                                      onChange={this.handleChangeChk}>
                                Zusammenfassung
                            </Checkbox>
                            {' '}
                            <Checkbox inline id="showMinVariance" defaultChecked={this.state.showMinVariance}
                                      onChange={this.handleChangeChk}>
                                Beste Streifen
                            </Checkbox>
                        </Col>
                        <Col componentClass={ControlLabel} sm={2}>Filter Ansicht [°, °, ]:</Col>
                        <Col sm={3}>
                            <FormControl id="headingsViewFilter" type="text"
                                         onChange={e => this.changeViewFilter(e)}
                                         onBlur={() => this.requestLandingPlanes()}
                                         defaultValue=""/>
                        </Col>

                        <Col smOffset={0} sm={2}>
                            <button class="col-sm-12 btn btn-success"
                                    onClick={(e) => {
                                        e.preventDefault();
                                        this.requestLandingPlanes();
                                    }}>
                                request Results
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
                                        tiffFilePath = tiffFilePath.substring(0, tiffFilePath.lastIndexOf("/") + 1);
                                        console.log("M-file-full PAth: " + tiffFilePath + mFileName);
                                        this.saveToMFile(e, tiffFilePath + mFileName);
                                    }}>
                                Save to M-File
                            </button>
                        </Col>
                    </FormGroup>
                </Form>
            </div>
        );
    }
}

const mapStateToProps = state => ({
    extent: state.tiffinfo.fileExtent,
    tiffInfo: state.tiffinfo,
    landingPlanes: state.landingPlanes,
});


export default connect(mapStateToProps)(MapOverview);