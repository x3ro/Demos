/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    Watch application reads watch adapter and creates events when buttons are pressed
 * 
 * @date            Mar 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const DEFAULT_HOST = 'localhost';               /* host address to the portal */
const DEFAULT_PORT = 44445;                     /* host port to the portal */
const DEFAULT_UART = '/dev/ttyACM0';            /* serial port of the watch adapter */

/**
 * Chronos commands
 */
const CMD_START = [0xFF, 0x07, 0x03];
const CMD_READ  = [0xFF, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00];

/**
 * Event IDs
 */
const VIZ_PARENT_SELECT     = 0;
const VIZ_PARENT_DESELECT   = 1;
const VIZ_DIO_RCVD          = 2;
const VIZ_DTA_RCVD          = 3;
const EVT_ALARM             = 4;
const EVT_CONFIRM           = 5;
const EVT_WARN              = 6;
const EVT_DISARMED          = 7;
const EVT_RESET             = 8;

/**
 * Interested nodes
 */
const LISTEN_NODES = ['web', 41];

/**
 * Library imports
 */
var net = require('net');
var JsonSocket = require('json-socket');
var SerialPort = require('serialport').SerialPort;

/**
 * Global variables
 */
var host = DEFAULT_HOST;
var port = DEFAULT_PORT;
var dev = DEFAULT_UART;
var socket = new JsonSocket(new net.Socket());  /* connection to the portal */
var uart = null;
var readbuf = [];
var debounce = [true, true, true];
var next_evt = 0;

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
            case 3:
                host = arg;
            break;
            case 4:
                port = arg;
            break;
        }
    });
}

/**
 * Socket connection section
 * (event handlers for the TCP connection to the host)
 */
socket.on('connect', function() {
    console.log('SOCKET: Successfully connected to ' + host + ':' + port);
});

socket.on('close', function(error) {
    console.log('SOCKET: Connection was terminated, we\'ll see each other again.');
});

socket.on('error', function() {
    console.log('SOCKET: Unable to connect to portal');
    // process.exit(1);
});

/**
 * Parse a line from the serial port into a JSON object.
 */
function readData(data) {
    for (var i = 0; i < data.length; i++) {
        readbuf.push(data[i]);
        if (readbuf.length == 7) {
            // button codes are 18 (0x12), 34 (0x22) and 50 (0x32), 255 (0xff) for nothing pressed
            var btn = ((readbuf[6]) >> 4) - 1;
            if (btn < 3) {
                btnEvent(btn);
            }
            readbuf = [];
        }
    }
}

function btnEvent(button) {
    if (debounce[button]) {
        debounce[button] = false;
        setTimeout(function() {
            debounce[button] = true;
        }, 500);
        console.log('BUTTON', button);
        switch (button) {
            case 0:
                sendEvent(EVT_ALARM);
                break;
            case 1:
                sendEvent(EVT_WARN);
                break;
            case 2:
                sendEvent(EVT_CONFIRM);
                break;
        }
    }
}

function sendEvent(id) {
    var evt = {'id': id, 'data': next_evt++, 'src': 'watch'};
    LISTEN_NODES.forEach(function(node) {
        evt.dst = node;
        socket.sendMessage(evt);
        console.log("Send evt " + evt.id + " to " + evt.dst);
    });
}

/**
 * Bootstrapping and starting the viz-proxy
 */
console.log("RIOT watch-proxy - CeBIT edition");
console.log("Usage: $node viz.js [DEV] [HOST] [PORT]\n");
parseCommandLineArgs();
console.log("INFO:   Opening the watch adapter at " + dev + " and socket to " + host + ":" + port + "\n");
socket.connect(port, host);

/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
var uart = new SerialPort(dev, {
        'baudrate': 115200, 
        'databits': 8, 
        'parity': 'none', 
        'stopbits': 1
    }, false);

uart.open(function() {
    console.log('SERIAL: Connection to watch bridge device open on ' + dev);
    uart.on('data', readData);
    uart.write(CMD_START);

    setInterval(function() {
        uart.write(CMD_READ);
    }, 100);
});
