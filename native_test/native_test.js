/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    The viz proxy acts as proxy between a serial port and the visualization back-end.
 * 
 * @date            Mar 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const NATIVE_PORT = 12345;               /* host address for the demo server */

/**
 * Library imports
 */
var net = require('net');
var readline = require('readline');

/**
 * Global variables
 */
var port = NATIVE_PORT;
var socket = null;
var rl = readline.createInterface({'input': process.stdin, 'output': process.stdout});



/**
 * Parse serial port, host and port from command line arguments
 *
 * Usage: node viz.js [DEV] [HOST] [PORT]
 */
function parseCommandLineArgs() {
    process.argv.forEach(function(arg, index) {
        switch (index) {
            case 2:
                dev = arg;
            break;
        }
    });
}


var socket = net.connect({'port': NATIVE_PORT}, function() {
    console.log('connected to native RIOT');
    socket.write('help\n');
});

socket.on('data', function(data) {
    console.log(data.toString());
});


rl.on('line', function(data) {
    socket.write(data + "\n");
});