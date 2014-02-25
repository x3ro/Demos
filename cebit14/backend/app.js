/*
 * Copyright (C) 2013 Freie Universität Berlin
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
const FRONTEND_PORT = 23230;
const NETWORK_PORT  = 12345;
const WEBROOT       = __dirname + '/webroot/';


/**
 * include packages
 */
var express = require('express');
var app = express();
var httpServer = require('http').createServer(app);
var io = require('socket.io').listen(httpServer);


/**
 * configure web server, setup of static and dynamic routes
 */
app.use('/img', express.static(WEBROOT));
app.use('/js', express.static(WEBROOT));
app.use('/css', express.static(WEBROOT));
app.use(express.favicon(WEBROOT + 'favicon.ico'));
app.get('/*', function(req, res) {
    res.sendfile(WEBROOT + 'index.html');
});
httpServer.listen(FRONTEND_PORT, function() {
    console.info('WEBSERVER: Running at http://127.0.0.1:' + FRONTEND_PORT + '/');
});


/**
 * Configure the socket.io interface
 */
io.set('log level', 0);
io.sockets.on('connection', function(socket) {
    console.log("new ws connection");
});