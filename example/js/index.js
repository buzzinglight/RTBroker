//Page is ready
$(document).ready(function() {
	//Websocket connection
	var network = new Network();
	network.connect("ws://127.0.0.1:5553");
	
	//Send button
	$(".websocketsSend").click(function() {
		var message = $(this).parent().find(".code").text();
		logToWebsocketConsole("<— Send of " + JSON.stringify(message));
		network.send(message);
	});
	
	//Message for UI interaction
	$("*[send]").click(function() {
		network.send($(this).attr("send"));
	});
});

//Log to websockets console
function logToWebsocketConsole(message) {
	$("#websocketsConsole").prepend("<div><span class='date'>" + d_to_hms(new Date()) + "&nbsp;:&nbsp;</span><span class='message'>" + message + "</span></div>");
}

//Websockets reception
function websocketReception(message) {
	logToWebsocketConsole("—> Reception of " + JSON.stringify(message));
}
//Websocket status
function websocketOpened() {
	logToWebsocketConsole("Connected");
}
function websocketClosed() {
	logToWebsocketConsole("Disconnected");
}

//Date format
function d_to_hms(d) {
	var hh = d.getHours();
	if (hh < 10) hh = "0" + hh;
	var mm = d.getMinutes();
	if (mm < 10) mm = "0" + mm;
	var ss = d.getSeconds();
	if (ss < 10) ss = "0" + ss;
	return hh + ":" + mm + ":" + ss;
}