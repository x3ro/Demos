/*
 * Copyright (C) 2015 Freie Universität Berlin
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
const UDP_HOST = "abcd::10";
const UDP_PORT = 5555;

/**
 * define global variables from dependencies
 */
const dgram = require('dgram');
var express = require('express');
var app = express();
var sock = dgram.createSocket('udp6');

/* setup webserver */
app.get('*', function (req, res) {
    if (req.url == "/party") {
        sock.send("party", 0, 5, UDP_PORT, UDP_HOST, function() {
            console.log("send PARTY");
        });
    }
    else if (req.url == "/lame") {
        sock.send("lame", 0, 4, UDP_PORT, UDP_HOST, function() {
            console.log("send LAME");
        });
    }
    res.sendfile('index.html');
});

var server = app.listen(3000, '::', function () {
  var host = server.address().address;
  var port = server.address().port;

  console.log('Example app listening at http://%s:%s', host, port);
});
