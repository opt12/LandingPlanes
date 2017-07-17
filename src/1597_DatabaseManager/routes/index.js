var express = require('express');
var router = express.Router();

var connectedIPC = require('../ipc/ipc').getConnectedIPC();

var counter = 0;

/*
/!* GET home page. *!/
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

/!* GET home page. *!/
router.get('/ipc', function(req, res, next) {
  let result = connectedIPC.emit('message', {counter: counter++});
  res.json(result);
});
*/


module.exports = router;
