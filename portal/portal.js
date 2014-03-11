/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    This portal acts as a back-end of a reduced border router for the RIOT CeBIT demo.
 * 
 * @date            Feb 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const DEFAULT_HOST = 'localhost';               /* host address for the demo server */
const DEFAULT_PORT = 11111;                     /* host port for the demo server */
const DEFAULT_UART = '/dev/ttyUSB0';            /* default serial port connected to a msba2 */
const WATCH_PORT   = 44445;                     /* listen to connections from watch on this port */

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
 * Library imports
 */
var net = require('net');
var JsonSocket = require('json-socket');
var serialPort = require('serialport')
var SerialPort = serialPort.SerialPort;
var readline = require('readline');

/**
 * Global variables
 */
var host = DEFAULT_HOST;
var port = DEFAULT_PORT;
var dev = DEFAULT_UART;
var watch_socket = net.createServer();
var socket = new JsonSocket(new net.Socket());  /* connection to the demo host */
var isConnected = false;                        /* flag signals when connected to the host */
var uart = null;
var rl = readline.createInterface({'input': process.stdin, 'output': process.stdout});

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
    var line = "fw " + data.dst + " " + data.id + " " + data.data + "\n";
    uart.write(line);
    process.stdout.write("WRITE: " + line);
    if (data.id == EVT_ALARM || data.id == EVT_CONFIRM || data.id == EVT_WARN) {
        var viz = {'id': VIZ_DTA_RCVD, 'data': 23, 'src': 'web'};
        if (isConnected) {
            socket.sendMessage(viz);
        }
    }
});

/**
 * Parse a line from the serial port into a JSON object.
 * Format {'id': BYTE0, 'data': BYTE1, 'time': Date.currentTime()}
 */
function parseEvent(data) {
    var res = null;
    if (data.match(/^bw \d+ \d+ \d+\s?/)) {
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
 * Setup connection to watches
 */
watch_socket.on('connection', function(sock) {
    console.log("WATCH:    New connection from" + sock);
    sock = new JsonSocket(sock);
    sock.on('message', function(evt) {
        console.log("WATCH:  Event - id:" + evt.id + ", data:" + evt.data + " --- DST:" + evt.dst);
        if (evt.dst == 'web' && isConnected) {
            socket.sendMessage(evt);
        } else {
            var line = "fw " + data.dst + " " + data.id + " " + data.data + "\n";
            uart.write(line);
            console.log(line);
        }
        var viz = {'id': VIZ_DTA_RCVD, 'data': 23, 'src': 'watch'};
        if (isConnected) {
            socket.sendMessage(viz);
        }
    });
});

/**
 * Bootstrapping and starting the portal
 */
console.log("RIOT Portal - CeBIT edition");
console.log("Usage: $node portal.js [DEV] [HOST] [PORT]\n");
parseCommandLineArgs();
console.log("INFO:   Opening RIOT at " + dev + " and socket to " + host + ":" + port + "\n");
watch_socket.listen(WATCH_PORT, function() {
    console.info("WATCH:  Listening on port " + WATCH_PORT);
});
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
    console.log('SERIAL: Connection opened - portal open for data travel');
    uart.on('data', function(data){
        console.log('READ:  ' + data);
        data = parseEvent(data);
        if (data && isConnected) {
            socket.sendMessage(data);
        }
    });
});
