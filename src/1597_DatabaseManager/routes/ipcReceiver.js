var connectedIPC = require('../ipc/ipc').getConnectedIPC();

function register () {
    for(route in ipcRoutes){
        ipcRoutes[route]();
    }
};

const taskReceiptListener = function() {
    connectedIPC.on('taskReceipt',
        function (data) {
            console.log("Got a taskReceipt!");
            console.log('data content is : ', data);
        }
    );
    console.log("connected listener for 'taskReceipt'");
};

const messageListener = function() {
    connectedIPC.on('message',
        function (data) {
            console.log("Got a message!");
            console.log('data content is : ', data);
        }
    );
    console.log("connected listener for 'message'");
};

const ipcRoutes={
    taskReceipt: taskReceiptListener,
    message: messageListener,
};


module.exports = register;




