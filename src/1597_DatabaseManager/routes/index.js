var express = require('express');
var router = express.Router();

var connectedIPC = require('../ipc/ipc').getConnectedIPC();

var counter = 0;


module.exports = router;
