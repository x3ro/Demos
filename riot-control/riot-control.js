
/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    The space knows everything, contains everything and is not a cloud.
 * 
 * @date            Feb 2014
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/**
 * Configuration
 */
const PORTAL_PORT = 11111;                      /* port opened for incoming portals */
const WEB_PORT = 9999;                          /* port to the rest of the verse */
const WEBROOT = __dirname + '/webroot/';        /* path to the folder containing the web files */
const DATAROOT = __dirname + '/data/';          /* path to the database files */

/**
 * Library imports
 */
var fs = require('fs');
var net = require('net');
var JsonSocket = require('json-socket');
var express = require('express');
var app = express();
var httpServer = require('http').createServer(app);
var io = require('socket.io').listen(httpServer);

/**
 * Global variables
 */
var portal_port = PORTAL_PORT;
var web_port = WEB_PORT;
var portal_socket = net.createServer();
var portal_client = null;

/**
 * Parse command line arguments: portal port and web port 
 *
 * Usage: node space.js [WEB_PORT] [PORTAL_PORT]
 */
function parseCommandLineArgs() {
    process.argv.forEach(function(arg, index) {
        switch (index) {
            case 2:
                web_port = arg;
            break;
            case 3:
                portal_port = arg;
            break;
        }
    });
}


/**
 * configure web server, setup of static and dynamic routes
 */
app.use('/img', express.static(WEBROOT + 'img/'));
app.use('/js', express.static(WEBROOT));
app.use('/css', express.static(WEBROOT));
app.use(express.favicon(WEBROOT + 'favicon.ico'));
app.get('/*', function(req, res) {
    if (req.params[0]) {
        res.sendfile(WEBROOT + req.params[0]);
    } else {
        res.sendfile(WEBROOT + 'index.html');
    }
});

/**
 * configure socket.io
 */
io.set('log level', 0);

io.sockets.on('connection', function(socket) {
    console.log("WEBSOCKET: New connection from "   // REMOVE
        + socket.handshake.address.address + ":" + socket.handshake.address.port);
    socket.on('event', function(data) {
        console.log("WEBSOCKET: Received data:");   // REMOVE
        console.log(data);                          // REMOVE
        if (portal_client) {
            portal_client.sendMessage(data);
        }
    });
});


/**
 * Bootstrap socket connection to the portal
 * (event handlers for the TCP connection to the portal)
 */
portal_socket.on('connection', function(socket) {
    if (!portal_client) {
        console.log("PORTAL:    New connection from "   // REMOVE
            + socket.address().address + ":" + socket.address().port);
        portal_client = new JsonSocket(socket);
        portal_client.on('message', function(evt) {
            console.log("PORTAL:    Received data:")    // REMOVE
            console.log(evt);                           // REMOVE
            io.sockets.emit('event', evt);
        });
        portal_client.on('close', function(e) {
            portal_client = null;
            console.log("PORTAL:    Connection closed");
        });
    }
});





/**
 * Bootstrapping and starting the reporter
 */
console.log("RIOT data sink - CeBIT edition");
console.log("Usage: $node space.js [WEB_PORT] [PORTAL_PORT]");
parseCommandLineArgs();
httpServer.listen(web_port, function() {
    console.info('WEBSERVER: Listening on port ' + web_port);
});
portal_socket.listen(portal_port, function() {
    console.info('PORTAL:    Listening on port ' + portal_port);
});
