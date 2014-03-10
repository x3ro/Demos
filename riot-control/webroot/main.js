/*
 * Copyright (C) 2013 Freie UniversitÃ¤t Berlin
 *
 * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
 * top level directory for more details.
 */

/**
 * @fileoverview    Basic functionality for RIOT TV
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */
const GRAPH_PANE = '#riot-tv';

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
 * Some global variables
 */
var graph = undefined;
var socket = undefined;
var isConnected = false;
var stats = {};                 // object saves status information about nodes, as parent, rank and message counter
var ignoreList = {};            // a list containing ignored edges, events on those will not be displayed

// show a popup when hovering graph nodes
var popUp;

/**
 * Pre-Defined coloring for the visulization
 */
var colors = {
    'rpl_parent':   '#004CFF',
    'rpl_dio':      '#004CFF',
    'udp':          '#B3FA00',
    'color0':  '#004CFF',       // blue (fast) fading arrow (-> send DIO)
    'color1':  '#FFCC00',       // orange (fast) fading arrow (-> send TVO upwards: TRAIL request)
    'color2':  '#54E600',       // green (fast) fading arrow (-> send TVO downwards: TRAIL reply)
    'color3':  '#FFCC00',       // orange fading (-> send TVO-ACK)
    'color4':  '#2EE600',       // 
    'color5':  '#2EE600',       // 
    'color6':  '#004CFF',       // blue fading arrow (-> receive DIO: accept)
    'color7':  '#004CFF',       // blue fading arrow (-> receive DIO: ignore since TRAIL verification pending)
    'color8':  '#FFCC00',       // orange fading arrow (-> receive TVO upwards)
    'color9':  '#54E600',       // green fading arrow (-> receive TVO downwards)
    'color10': '#FFCC00',       // orange fading arrow (-> receive TVO Duplicate)
    'color11': '#FFCC00',       // orange fading (-> receive TVO-ACK)
    'color12': '#2EE600',       // 
    'color13': '#2EE600',       // 
    'color14': '#2EE600',       // 
    'color15': '#009DFF',       // thin bright blue line (-> selected parent)
    'color20': '#536236',       // light green -> camera traffic
    'color30': '#54E600',       // event traffic
};

/**
 * Pre-Defined fading intervals for the visualization
 */
var fading = {                  // fading speed in ms
    'superfast': 50,
    'fast': 500,
    'normal': 2000,
    'slow': 5000,
};

/**
 * Bootstrap the whole javascript klim-bim only after the page was fully loaded
 */
$(document).ready(function() {
    calcSizes();
    initGraph();
    bindCommands();
    initSocket();
    $(window).resize(calcSizes);
});

/**
 * Set bindings for buttons etc.
 */
function bindCommands() {
    $("#cmd-evt-clear").click(cmd_clearEvents);
    $("#cmd-evt-alarm").click(cmd_alarm);
    $("#cmd-evt-warn").click(cmd_warn);
    $("#cmd-evt-confirm").click(cmd_confirm);
};

/**
 * Set the size of the RIOT-TV screen
 */
function calcSizes() {
    $("#riot-tv").height($(document).height() - 180);
};

/**
 * Setup a websocket connection to the anchor for receiving events and register event handlers.
 */
function initSocket() {
    socket = io.connect();
    socket.on('connect', function() {
        isConnected = true;
    });
    socket.on('connect_failed', function() {
        console.log('Connection to localhost failed');
    });
    socket.on('error', function(error) {
        console.log('Error: ' + error);
    });
    // handle events
    socket.on('init', onInit);
    socket.on('event', onEvent);

    // old stuff
    socket.on('ignore', onIgnore);
};

/**
 * Initialize the display graph.
 */
function initGraph() {
    graph = sigma.init($(GRAPH_PANE).get(0));
    graph.drawingProperties({
        defaultLabelColor : '#fff',
        defaultLabelSize : 14,
        defaultLabelBGColor : '#fff',
        defaultLabelHoverColor : '#000',
        labelThreshold : 6,
        defaultEdgeType : 'line',           // curve or line
        defaultEdgeArrow: 'target'
    }).graphProperties({
        minNodeSize : 10,
        maxNodeSize : 15,
//      minEdgeSize : 1,
//      maxEdgeSize : 1
    }).mouseProperties({
//      maxRatio : 1,
    });
    graph.activateChange();
    //graph.activateFishEye();

    graph.bind('overnodes', showNodeInfo);
    graph.bind('outnodes', hideNodeInfo);
};

function showNodeInfo(evt) {
    popUp && popUp.remove();

    // make html tag
    var node;
      graph.iterNodes(function(n){
        node = n;
      },[evt.content[0]]);
    var data = stats[node.id];
    var html = '<div class="popup"><ul>';
    html += '<li>Address: ' + data.address + '</li>';
    html += '<li>Parent: ' + data.parent + '</li>';
    html += '<li>Root: ' + data.root + '</li>';
    html += '<li>Send: ' + data.send + '</li>';
    html += '<li>Received: ' + data.rec + '</li>';
    html += '</ul></div>';

    popUp = $(html).attr('id', 'popup' + graph.getID()
        ).css({
            'left': node.displayX - 5,
            'top': node.displayY + 9,
            'z-index': -0
        });
    $(GRAPH_PANE).append(popUp);
};

function hideNodeInfo(evt) {
      popUp && popUp.remove();
      popUp = false;
};



function onInit(data) {
    if (data) {
        data.nodes.forEach(function(n) {
            stats[n.id] = {'address': n.id, 'rank': '-', 'parent': '-', 'root': '-', 'send': 0, 'rec': 0};
            graph.addNode(n.id, n.params);
        });
        data.edges.forEach(function(e) {
            graph.addEdge(e.id, e.src, e.dst, e.params);
        });
        graph.draw(2, 2, 2);
    }
};

/**
 * @brief   Add an ignore entry for a given edge
 *
 * @param Object {'id': ... , 'ignores': ...}
 */
function onIgnore(data) {
    var id = data.ignores + "_" + data.id;
    ignoreList[id] = true;
};

/**
 * Parse incoming events
 */
function onEvent(evt) {
    console.log(evt);
    switch (evt.id) {
        case VIZ_PARENT_SELECT:             // RPL parent select
            evt_psel(evt);
        break;
        case VIZ_PARENT_DESELECT:           // RPL parent de-select
            evt_pdesel(evt);
        break;
        case VIZ_DIO_RCVD:                  // RPL DIO received
            evt_dio(evt);
        break;
        case VIZ_DTA_RCVD:
            evt_udp(evt);
        break;
        case EVT_ALARM:
            evt_alarm(evt);
        break;
        case EVT_CONFIRM:
            evt_confirm(evt);
        break;
        case EVT_WARN:
            evt_warn(evt);
        break;
        case EVT_DISARMED:

        break;
    }
};

/**
 * RPL parent select
 */
function evt_psel(evt) {
    var id = evt.src + "_" + evt.data + "-parent";
    stats[evt.src].parent = evt.data;
    graph.showLink(evt.src, evt.data, id, colors.rpl_parent, fading.fast, 5);
};

/**
 * RPL parent deselect
 */
function evt_pdesel(evt) {
    var id = evt.src + "_" + evt.data + "-parent";
    stats[evt.src].parent = '-';
    graph.hideLink(id, fading.fast);
};

/**
 * RPL DIO received
 */
function evt_dio(evt) {
    var id = evt.src + "_" + evt.data + "-dio";
    stats[evt.data].send ++;
    stats[evt.src].rec ++;
    graph.fadeLink(evt.src, evt.data, id, colors.rpl_dio, fading.fast, 30, 0);
}

/**
 * UDP packet received
 */
function evt_udp(evt) {
    var id = evt.src + "_" + evt.data + "-udp";
    stats[evt.data].send ++;
    stats[evt.src].rec ++;
    graph.fadeLink(evt.src, evt.data, id, colors.udp, fading.fast, 30, 0);
}

/**
 * Some alarm was triggered in the network!
 */
function evt_alarm(evt) {
    if (!evt.data) {
        evt.data = new Date();
    }
    var box = $(
        '<div id="event-' + evt.data + '" class="panel panel-danger">' + 
          '<div class="panel-heading">' +
            '<div class="row">' +
              '<div class="col-md-4">' + new Date(evt.time).toLocaleTimeString() + '</div>' +
              '<div class="col-md-5">Alarm</div>' +
              '<div class="col-md-3 text-right">' +
                '<button type="button" class="btn btn-success btn-xs">Accept</button>' +
              '</div>' +
            '</div>' +
          '</div>' +
          '<div class="panel-body">Intrusion Alert</div>' +
        '</div>');
    box.find("button").click(function() {
        cmd_confirm(evt.data);
    })

    box.hide();
    $("#riot-events").prepend(box);
    box.show(400);
}

/**
 * Display a warning
 */
function evt_warn(evt) {
    if (!evt.data) {
        evt.data = new Date();
    }
    var id = "event-" + evt.data;
    var box = $(
        '<div id="' + id + '" class="panel panel-warning">' + 
          '<div class="panel-heading">' +
            '<div class="row">' +
              '<div class="col-md-4">' + new Date(evt.time).toLocaleTimeString() + '</div>' +
              '<div class="col-md-5">Warning</div>' +
              '<div class="col-md-3 text-right">' +
                '<button id="btn-' + id + '" type="button" class="btn btn-success btn-xs">Accept</button>' +
              '</div>' +
            '</div>' +
          '</div>' +
          '<div class="panel-body">Something serious might have happened</div>' +
        '</div>');
    box.find("button").click(function() {
        cmd_confirm(evt.data);
    })

    box.hide();
    $("#riot-events").prepend(box);
    box.show(400);
}

/**
 * Confirm an event
 */
function evt_confirm(evt) {
    var box;
    if (typeof(evt.data) != 'number') {
        box = $("#riot-events > div:first");
    } else {
        box = $("#event-" + evt.data);
    }

    var newbox = box.clone();
    newbox.hide();
    newbox.removeClass("panel-warning");
    newbox.removeClass("panel-danger");
    newbox.addClass("panel-default");
    $("#riot-events").prepend(newbox);
    newbox.find("button").remove();
    box.slideUp(200, function() {
        box.remove();
    });
    newbox.slideDown(200);
}


/**
 * Commands
 */
function cmd_clearEvents() {
    $("#riot-events").empty();
}

function cmd_alarm(id) {
    var evt = {'src': 99, 'id': EVT_ALARM};
    evt.data = (typeof(id) == 'number') ? id : 0;
    evt.time = new Date();

    socket.emit('event', evt);
}

function cmd_warn(id) {
    var evt = {'src': 99, 'id': EVT_WARN};
    evt.data = (typeof id == 'number') ? id : 0;
    evt.time = new Date();

    socket.emit('event', evt);
}

function cmd_confirm(id) {
    var evt = {'src': 99, 'id': EVT_CONFIRM};
    if (typeof(id) != 'number') {
        evt.data = 0;
    } else {
        evt.data = id;
    }
    // send out event
    socket.emit('event', evt);
}
