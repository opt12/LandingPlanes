var express = require('express');
var router = express.Router();

var connectedIPC = require('../ipc/ipc').getConnectedIPC();
var handleDropDb = require('../commands/cmdDataBase').handleDropDb;

var counter = 0;

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

/* POST task. */
router.post('/', function(req, res, next) {
    let type = req.body.type;
    let result;

    switch (type) {
        case 'SCAN':
            result = connectedIPC.emit('SCAN', req.body.data);
            break;
        case 'SAVE_2_M_FILE':
            result = connectedIPC.emit('SAVE_2_M_FILE', req.body.data);
            break;
        default:
            result = connectedIPC.emit('OTHER_MESSAGE', req.body.data);
    }

    res.json(req.body);
});

router.delete('/drop', function(req, res, next) {

    console.log("Dropping Db. You want it that way...");

    let result = handleDropDb();

    res.json({result:"DB dropped"});
});


module.exports = router;
