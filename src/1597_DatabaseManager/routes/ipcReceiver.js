var connectedIPC = require('../ipc/ipc').getConnectedIPC();
var cmdDataBase = require('../commands/cmdDataBase');

function register () {
    for(route in ipcRoutes){
        ipcRoutes[route]();
    }
};

const taskReceiptListener = function() {
    connectedIPC.on('taskReceipt',
        function (data) {
            console.log("Got a taskReceipt!");
            console.log('data content is : ');
            console.log(JSON.stringify(data, null, 2));
        }
    );
    console.log("connected listener for 'taskReceipt'");
};

const messageListener = function() {
    connectedIPC.on('message',
        function (data) {
            console.log("Got a message!");
            console.log('data content is : ')
            console.log(JSON.stringify(data, null, 2));
        }
    );
    console.log("connected listener for 'message'");
};

const landingPlaneListener = function() {
    connectedIPC.on('landingPlane',
        function (data) {
            // console.log("Got a landingPlane!");
            // console.log('data content is : ')
            // console.log(JSON.stringify(data, null, 2));

            cmdDataBase.handleCreateLandingPlane(data)
                .then(function ({result, newPlane = true}) {
                    if(newPlane) {
                        // console.log("added landingPlane to database: ");
                        // console.log(JSON.stringify(result, null, 2))
                    } else {
                        console.log("plane already exists in database");
                        console.log(JSON.stringify(result, null, 2))
                    }
                })
                .catch(function (err) {
                    console.log("adding landingPlane to database FAILED: ");
                    console.log(err);
                })
            return;
        }
    );
    console.log("connected listener for 'landingPlane'");
};

const ipcRoutes={
    taskReceipt: taskReceiptListener,
    message: messageListener,
    landingPlane: landingPlaneListener,
};


module.exports = register;




