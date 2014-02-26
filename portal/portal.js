/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    This portal acts as a backend of a reduced border router for the RIOT cebit demo.
 * 
 * @date            Feb 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const DEFAULT_HOST = 'localhost';               /* host address for the demo server */
const DEFAULT_PORT = 11111;                     /* host port for the demo server */
const DEFAULT_UART = '/dev/ttyUSB0';            /* defeul serial port connected to a msba2 */

/**
 * Library imports
 */
var net = require('net');
var JsonSocket = require('json-socket');
var serialPort = require('serialport');
var SerialPort = serialPort.SerialPort;

/**
 * Global variables
 */
var host = DEFAULT_HOST;
var port = DEFAULT_PORT;
var dev = DEFAULT_UART;
var socket = new JsonSocket(new net.Socket());  /* connection to the demo host */
var isConnected = false;                        /* flag signals when connected to the host */
var uart;

/**
 * Parse serial port, host and port from command line arguments
 *
 * Usage: node portal.js [DEV] [HOST] [PORT]
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
function connect() {
    if (!isConnected) {
        console.log("SOCKET: Trying to connect to " + host + ':' + port);
        socket.connect(port, host);
    }
}

socket.on('connect', function() {
    console.log('SOCKET: Successfully connected to ' + host + ':' + port);
    isConnected = true;
});

socket.on('close', function(error) {
    if (!error) {
        console.log('SOCKET: Connection was terminated, we\'ll see each other again.');
        isConnected = false;
        connect();
    }
});

socket.on('error', function() {
    console.log('SOCKET: Unable to connect, next departure in 1s');
    setTimeout(connect, 1000);
});

/** 
 * Handle incoming traffic (and commands to the MSBA2)
 */
socket.on('message', function(data) {
    console.log('DATA:    ' + data.data);
    uart.write("fw " + data.dst + " " + data.id + " " + data.data\n);
});

/**
 * Bootstrapping and starting the reporter
 */
console.log("RIOT Portal - Cebit edition");
console.log("Usage: $node portal.js [DEV] [HOST] [PORT]");
parseCommandLineArgs();
console.log("INFO:   Opening RIOT at " + dev + " and socket to " + host + ":" + port);
connect();
uart = new SerialPort(dev, {
    'baudrate': 115200, 
    'databits': 8, 
    'parity': 'none', 
    'stopbits': 1, 
    'parser': serialPort.parsers.readline("\n")},
    false);                                     /* connection to the sensor node over UART */


/**
 * Parse a line from the serial port into a json object.
 * Format {'id': BYTE0, 'data': BYTE1, 'time': Date.currentTime()}
 */
function parseEvent(data) {
    var res = {};
    if (data.match(/^FW:\s+\d+\s+\d+\s+\d+\s?/)) {
        var split = data.split(" ");
        res.src = parseInt(split[1]);
        res.id = parseInt(split[2]);
        res.data = parseInt(split[3]);
        res.time = new Date().getTime();
    }
    return res;
}

/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
uart.open(function() {
    console.log('SERIAL: Connection opened - portal open for data travel');
    uart.on('data', function(data){
        console.log('UART:    ' + data);
        if (isConnected) {
            data = parseEvent(data);
            socket.sendMessage(data);
        }
    });
});


// test cases for event parsing
// var t1 = [
//     "FW: 12 23 88",
//     "NO: 12 23 88",
//     " FW: 32 32 32",
//     "FW: 986 aa 88",
//     "-FW: aa 99 23",
//     "FW: 8 21 0 99",
//     "FW: 12312 1288381 111",
//     "23"];

// t1.forEach(function(test){
//     console.log("Test: " + test);
//     console.log(parseEvent(test));
//     console.log("--------");
// });