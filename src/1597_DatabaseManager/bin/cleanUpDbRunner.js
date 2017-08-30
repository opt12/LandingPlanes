"use strict";

var cleanUp = require('../commands/cmdCleanUpDB').cleanUp;

console.log("Starting database cleanUp with Merging");

cleanUp()
    .then(()=> {
        console.log("finished cleanUp");
        process.exit();
    });
