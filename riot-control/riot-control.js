
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
const LAYOUTDIR = __dirname + '/layouts/';      /* path to the folder containing the layouts */
const LAYOUTFILE = LAYOUTDIR + 'cebit.json';    /* Layout to use for this app */
//const DATAROOT = __dirname + '/data/';          /* path to the database files */

/**
 * List of nodes that are interested into events
 */
const listen_nodes = [51, 61];

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
var rpl_parents = {};                           /* save rpl connection information */
var next_evt_id = 1;

/**
 * Global variables
 */
var portal_port = PORTAL_PORT;
var web_port = WEB_PORT;
var portal_socket = net.createServer();
var portal_client = null;
var layout = null;

/**
 * Parse command line arguments: portal port and web port 
 *
 * Usage: node riot-control.js [WEB_PORT] [PORTAL_PORT]
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
 * Read layout file
 */
fs.readFile(LAYOUTFILE, 'utf8', function(err, data) {
    if (err) {
        console.log('LAYOUT:    ERROR - could not find layout file');
        process.exit(1);
    }
    try {
        layout = JSON.parse(data);
    } catch (e) {
        console.log('LAYOUT:    Unable to parse JSON file: ' + e);
        process.exit(1);
    }
    console.log('LAYOUT:    Successfully read layout data from ' + LAYOUTFILE);
});


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
    socket.on('reset', cli_onReset);
    socket.on('event', cli_onEvent);
    cli_onConnect(socket);
});

/**
 * Handle connection to the client
 */
function cli_onConnect(sock) {
    // parse layout file
    sock.emit('init', layout);
    // send known RPL connection information
    rpl_parents['51'] = 32;
    rpl_parents['61'] = 32;
    rpl_parents['41'] = 33;
    rpl_parents['32'] = 31;
    rpl_parents['33'] = 31;
    rpl_parents['31'] = 23;
    rpl_parents['23'] = 'web';
    for (var src in rpl_parents) {
        sock.emit('event', {'id': 0, 'data': rpl_parents[src], 'src': src});
    };
};

function cli_onReset(data) {
    // do some resetting
    rpl_parents = {};
};

function cli_onEvent(data) {
    console.log("WEBSOCKET: Received data:");   // REMOVE
    console.log(data);                          // REMOVE
    // add event id
    if (data.data == 0) {
        data.data = next_evt_id++;
    }
    // send on event per listener to the portal
    if (portal_client) {
        listen_nodes.forEach(function(address) {
            data.dst = address;
            portal_client.sendMessage(data);
        });
    }
    // propagate event to web-listeners
    data.dst = 99;
    io.sockets.emit('event', data);
};

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
console.log("Usage: $node space.js [WEB_PORT] [PORTAL_PORT]\n");
parseCommandLineArgs();
httpServer.listen(web_port, function() {
    console.info('WEBSERVER: Listening on port ' + web_port);
});
portal_socket.listen(portal_port, function() {
    console.info('PORTAL:    Listening on port ' + portal_port);
});
