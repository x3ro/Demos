
/**
 * define some global variables
 */
var socket = io.connect();

/**
 * @brief bootstrap the user javascript
 */
$(document).ready(function() {
    setupTextsend();
});



var setupTextsend = function() {
    $("#sendstring").click(sendText);
    $("#sendstring-dst").keypress(sendKey);
    $("#sendstring-id").keypress(sendKey);
    $("#sendstring-data").keypress(sendKey);
};

var sendKey = function(e) {
    if (e.charCode == 13) {
        sendText();
    }
};

var sendText = function() {
    var obj = {};
    obj.dst = parseInt($("#sendstring-dst").val());
    obj.id = parseInt($("#sendstring-id").val());
    obj.data = parseInt($("#sendstring-data").val());
    if (obj.dst != NaN && obj.id  != NaN && obj.data != NaN) {
        console.log(obj);
        socket.emit('event', obj);
    }
};

/**
 * setup the receiving behavior when getting data from the web-socket
 */
socket.on('event', function(evt) {
    var ee = $('<div class\="event">Event: [' + evt.id + '] | Source: [' + evt.src + '] | Data: '
            + evt.data + ' | ' + new Date(evt.time).toLocaleString() + '</div>');
    $("#events").prepend(ee);
});