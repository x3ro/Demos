/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    The space knows everything, contains everything and is not a cloud.
 * 
 * @date			Feb 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const PORTAL_PORT = 11111;                      /* port opened for incoming portals */
const ANY_PORT = 99999;                         /* port to the rest of the verse */

/**
 * Library imports
 */
var net = require('net');
var	JsonSocket = require('json-socket');

/**
 * Global variables
 */
var portal_port = PORTAL_PORT;
var any_port = ANY_PORT;
var portal_socket = net.createServer();


/**
 * Parse command line arguments: portal port and web port 
 *
 * Usage: node space.js [PORTAL_PORT] [WEB_PORT]
 */
function parseCommandLineArgs() {
	process.argv.forEach(function(arg, index) {
		switch (index) {
			case 2:
				portal_port = arg;
			break;
			case 3:
				any_port = arg;
			break;
		}
	});
}

/**
 * Bootstrap socket connection to the portal
 * (event handlers for the TCP connection to the portal)
 */
portal_socket.on('connection', function(socket) {
    socket = new JsonSocket(socket);
    socket.on('message', function(msg) {
        
    });
});

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
    uart.write(data.data + "\n");
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
	false);										/* connection to the sensor node over UART */

/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
uart.open(function() {
	console.log('SERIAL: Connection opened - portal open for data travel');
	uart.on('data', function(data){
		var time = new Date().getTime();
		console.log('UART:    ' + data);
		if (isConnected) {
			socket.sendMessage({'data': data, 'time': time});
		}
	});
});
