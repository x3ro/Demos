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
const GATESENSE_HOST = "gatesencetest.cloudapp.net";
const GATESENSE_PORT = 7913;
const GATESENSE_RESS = 'sensor-data';
const PORTAL_DEV     = '/dev/ttyUSB0';

/**
 * define global variables from dependencies
 */
const coap = require('coap');
const cbor = require('cbor');
const serialport = require("serialport");
const SerialPort = serialport.SerialPort;

/**
 * parse strings
 */
const regexTemp = /net: got sensor data from ([a-f0-9:]+) - humidity: (\d+\.?\d*); temperature: (\d+\.?\d*)/g;


var gatesenseQuery = {
    'id' : '',
    'dt' : 'weather',
    'r' : [{'temperature': '', 'humidity': '', 'time': ''}],
}


var testdata = {
    'dt': 'mightysense',
    'id': '123fff456',
    'r': [{
        'temperature': 21.789,
        'humidity': 87.334,
        'time': 1403014161
    }]
}

var test1 = {
    'dt': 'mightysense',
    'id': '123fff456',
    'r': [{
        "key1": "23",
        "hello": 44,
        "key": "value",
        "blubb": "blubb blubb",
        "helpme": "23"
    } ]
}


/**
 * Open the serial port.
 * (event handlers for the serial port)
 */
var uart = new SerialPort(PORTAL_DEV, {
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
        console.log('UART: Connected to Portal node via ' + PORTAL_DEV);
        uart.on('data', parseUartData);
    }
});

var parseUartData = function(data) {
    var match = null;

    console.log(data);
    match = regexTemp.exec(data);
    if (match) {
        // query Gatesense with temperature and humidity data
        gatesenseQuery.id = match[1];
        gatesenseQuery.r[0] = {
            'temperature': Math.floor(match[3]).toString(),
            'humidity': match[2].toString(),
            'time': Math.round(new Date().getTime() / 1000).toString()
        };
        gatesenseSend(gatesenseQuery);
    }
}


var gatesenseSend = function(data) {
    var cbored = cbor.encode(data);

    var gatesenseRequest = coap.request({
        'hostname': GATESENSE_HOST,
        'port': GATESENSE_PORT,
        'pathname': GATESENSE_RESS,
        'method': 'POST'
    });

    gatesenseRequest.on('response', function(res) {
        console.log('Response from Gatesense:', res.payload.toString());
        res.on('end', function() {
            console.log('');
        })
    });

    gatesenseRequest.end(cbored);
}
