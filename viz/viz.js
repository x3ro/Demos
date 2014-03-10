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
const DEFAULT_HOST = 'localhost';               /* host address for the demo server */
const DEFAULT_PORT = 22222;                     /* host port for the demo server */
const DEFAULT_UART = '/dev/ttyUSB0';            /* default serial port connected to a msba2 */

/**
 * Library imports
 */
var net = require('net');
var JsonSocket = require('json-socket');
var serialPort = require('serialport');
var SerialPort = serialPort.SerialPort;
var readline = require('readline');

/**
 * Global variables
 */
var host = DEFAULT_HOST;
var port = DEFAULT_PORT;
var dev = DEFAULT_UART;
var socket = new JsonSocket(new net.Socket());  /* connection to the demo host */
var isConnected = false;                        /* flag signals when connected to the host */
var uart = null;
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
 * Parse a line from the serial port into a JSON object.
 * Command line format: "fw SRC ID DATA"
 */
function parseEvent(data) {
    var res = null;
    if (data.match(/^fw \d+ \d+ \d+\s?/)) {
        var split = data.split(" ");
        res = {};
        res.src = parseInt(split[1]);
        res.id = parseInt(split[2]);
        res.data = parseInt(split[3]);
        res.time = new Date().getTime();
    }
    return res;
}

/**
 * Connect UART to std-in
 */
rl.on('line', function(data) {
    if (uart) {
        uart.write(data + '\n');
    }
});

/**
 * Bootstrapping and starting the viz-proxy
 */
console.log("RIOT viz-proxy - CeBIT edition");
console.log("Usage: $node viz.js [DEV] [HOST] [PORT]\n");
parseCommandLineArgs();
console.log("INFO:   Opening RIOT at " + dev + " and socket to " + host + ":" + port + "\n");
connect();

/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
var uart = new SerialPort(dev, {
        'baudrate': 115200, 
        'databits': 8, 
        'parity': 'none', 
        'stopbits': 1, 
        'parser': serialPort.parsers.readline("\n")
    },
    false);                                     /* connection to the sensor node over UART */

uart.open(function() {
    console.log('SERIAL: Connection opened - viz open for data travel');
    uart.on('data', function(data){
        console.log('READ:  ' + data);
        data = parseEvent(data);
        if (data && isConnected) {
            socket.sendMessage(data);
        }
    });
});
