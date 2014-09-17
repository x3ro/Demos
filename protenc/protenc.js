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
const HOST_GOOGLE = 'www.google.com';
const HOST_SPIEGEL = 'www.spiegel.de';
const PATH_DEFAULT = '/';
const HOST_WEATHER = 'api.openweathermap.org';
const PATH_WEATHER = 'data/2.5/weather?lat=52.474404&lon=13.403812';

/**
 * Library imports
 */
var http = require('http');
var net = require('net');
var cbor = require('cbor');

/**
 * Some global variables to be used
 */
var host = 'api.openweathermap.org';
var path = '/data/2.5/weather?lat=52.474404&lon=13.403812';

var regex = /([A-Za-z 0-9,:\r\n-_]+)\r\n\r\n(.+)/g;

var getHeader = "\
GET " + path + " HTTP/1.1\r\n\
Host: " + host + "\r\n\r\n";

console.log(getHeader);

var client = net.connect({'host': '144.76.102.166', 'port': 80}, function() {
    client.write(getHeader);
});

var buf = "";
client.on('data', function(data) {
    buf += data;
    client.end();
});

client.on('end', function() {
    //console.log(buf.toString());
    match = regex.exec(buf.toString());

    var cbor_data = cbor.encode(JSON.parse(match[2].toString()));

    console.log(R'esponse size:  ', buf.length, 'byte');
    console.log('HTTP header size: ', match[1].length + 5);
    console.log('JSON body size:   ', match[2].length);
    console.log('CBOR body size:   ', cbor_data.length);
});


// http.request({
//         'host': 'www.spiegel.de',
//         'path': ''
//     }, function(resp) {
//     var buf = '';

//     resp.on('data', function(data) {
//         buf += data;
//     });

//     resp.on('end', function() {
//         console.log(buf.length);
//     });
// }).end();
