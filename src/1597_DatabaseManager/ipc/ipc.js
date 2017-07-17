"use strict";

let ipc = require('node-ipc');
var exec = require('child_process').exec;
let child;


let connectedIPC = null;
const IpcId = '1597_IPC';


const getConnectedIPC = () => {
    if (connectedIPC) {
        return connectedIPC;    //if the connection is already established, it shall be a singleton
    }

    //OK, we need a new socket connection;

    let socketId = process.env.SOCKET_ID || '1597-ipc';
    ipc.config.id = IpcId;
    ipc.config.socketRoot = '/tmp/';
    ipc.config.appspace = '';
    ipc.config.retry = 1500;

    //try to connect to ipc socket
    ipc.connectTo(
        socketId,
        function () {
            ipc.of[socketId].on(
                'connect',
                function () {
                    console.log("Connected!!");
                    ipc.log('## connected to world ##'.rainbow, ipc.config.delay);
                });
            ipc.of[socketId].on(
                'disconnect',
                function () {
                    console.log("Disconnected!!");
                    ipc.log('disconnected from world'.notice);
                    connectedIPC = null;
                });
            ipc.of[socketId].on(
                'message', //any event or message type your server listens for
                function (data) {
                    console.log("Got a message!!");
                    ipc.log('got a message from world : '.debug, data);
                }
            );
            ipc.of[socketId].on(
                'error',
                (err) => {
                    //check if we should start the server before...
                    if (err) console.log("error obj: ", err);
                    console.log("Error connecting to the IPC-socket " + ipc.config.socketRoot + socketId);
                    console.log("Start the 1597 searchWrapper now...");

                    child = exec(process.env.TERMINAL + process.env.WRAPPER,
                        function (error, stdout, stderr) {
                            console.log('stdout: ' + stdout);
                            console.log('stderr: ' + stderr);
                            if (error !== null) {
                                console.log('exec error: ' + error);
                            }
                        });
                }
            )
        }
    );

    connectedIPC = ipc.of[socketId];

    return connectedIPC;
};

const disconnectIPC = () => {

    if (!connectedIPC) {
        console.log("IPC is already disconnected. So nothing to do.")
        return;
    }

    connectedIPC.disconnect(IpcId);
    console.log("IPC disconnecting. Don't use it any more.");
    return;
};


module.exports = {
    getConnectedIPC: getConnectedIPC,
    disconnectIPC: disconnectIPC,
};