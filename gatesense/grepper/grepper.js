/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * define configuration values
 */
const GATESENSE_WS   = 'ws://gatesense.com';
const GATESENSE_PORT = 9998;
const GATESENSE_RESS = 'c3b5e102-5128-4e22-b51e-67856a04a0ab';
const GATESENSE_URL  = 'http://gatesense.com/data/api/action/datastore_search?resource_id=';
const GATESENSE_QUERY= '&limit=1&sort=_id desc';
const UART_DEV       = '/dev/ttyUSB1';

/**
 * define global variables from dependencies
 */
const WebSocket = require("ws");
const http = require("http");
const serialport = require("serialport");
const SerialPort = serialport.SerialPort;

/**
 * some globally used variables
 */
var ws = null;

/**
 * connect to the Gatesense webservice and subscribe to receive update events
 */
var connectToWebservice = function() {
    ws = new WebSocket(GATESENSE_WS + ':' + GATESENSE_PORT);

    ws.on('message', function(data, flags) {
        // console.log("Flags:", flags);
        var jdata = JSON.parse(data);

        console.log("UPDATE EVENT from Gatesense");
        getLatestData();
    });

    ws.on('open', function(err) {
        subscribe(GATESENSE_RESS);
    });
}

var subscribe = function(resourceId) {
    var data = {
        type : 'datastoresubscribe',
        resource_id : resourceId,
    };
    ws.send(JSON.stringify(data));
}

var unsubscribe = function(resourceId) {
    var data = {
        'type' : 'datastoreunsubscribe',
        'resource_id' : resourceId,
    };
    ws.send(JSON.stringify(data));
}

/**
 * get the newest dataset from Gatesense
 */
var getLatestData = function(url) {
    http.get(GATESENSE_URL + GATESENSE_RESS + GATESENSE_QUERY, function(res) {
        var buffer = '';

        res.on('data', function(chunk) {
            buffer += chunk;
        });

        res.on('end', function(err) {
            jdata = JSON.parse(buffer);
            var temp = jdata.result.records[0].temperature;
            var humi = jdata.result.records[0].humidity;
            console.log('sensor ' + temp.toString() + ' ' + humi.toString());
            uart.write('sensor ' + temp.toString() + ' ' + humi.toString() + '\n');
        });
    })
}

/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
var uart = new SerialPort(UART_DEV, {
        'baudrate': 115200,
        'databits': 8,
        'parity': 'none',
        'stopbits': 1,
        'parser': serialport.parsers.readline("\n")
    }, false);

uart.open(function(error) {
    if (error) {
        console.log(error);
    } else {
        console.log('UART: Connected to Portal node via ' + UART_DEV);
        uart.on('data', function(line) {
            console.log(line);
        });
    }
});

/**
 * Start listening for updates
 */
connectToWebservice();
