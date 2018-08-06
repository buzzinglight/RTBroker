//Network
function Network() {
    this.URI                = "";
    this.websocket          = null;
    this.intervalId         = null;
    this.disconnectionAsked = false;
}
Network.prototype.connect = function(URI) {
    this.disconnectionAsked = false;
    if (typeof URI !== "undefined")
        this.URI = URI;
 
    try {
        if (this.websocket) {
            if (this.connected())
                this.websocket.close();
            delete this.websocket;
        }
         
        if (typeof MozWebSocket === 'function')
            WebSocket = MozWebSocket;
             
        this.websocket = new WebSocket(this.URI);
         
        this.websocket.onopen = function(evt) {
            websocketChanged(this.websocket);
        }.bind(this);
         
        this.websocket.onclose = function(evt) {
            websocketChanged(this.websocket);
            if (!this.disconnectionAsked) {
                setTimeout(this.connect.bind(this), 500);
            }
            delete this.websocket;
        }.bind(this);
         
        this.websocket.onmessage = function(evt) {
			//Cleaning des messages
			var message = evt.data.split(new RegExp(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)"));
			for(var i = 0 ; i < message.length ; i++) {
				if(message[i].indexOf("'") >= 0)
					message[i] = message[i].replaceAll("'", "");
			}
			websocketReception(message);
        }.bind(this);
        this.websocket.onerror = function(evt) {
            console.warn("Websocket error:", evt.data);
        };
        websocketChanged(this.websocket);
    }
    catch(exception) {
        console.error("Websocket fatal error, maybe your browser can't use websockets. You can look at the javascript console for more details on the error.");
        console.error("Websocket fatal error", exception);
    }
}
 
Network.prototype.connected = function() {
    if (this.websocket && this.websocket.readyState == 1)
        return true;
    return false;
};
 
Network.prototype.reconnect = function() {
    if (this.connected())
        this.disconnect();
    this.connect();
}
 
Network.prototype.disconnect = function() {
    this.disconnectionAsked = true;
    if (this.connected()) {
        this.websocket.close();
        websocketChanged(this.websocket);
    }
}
 
Network.prototype.send = function(message) {
    if (this.connected())
        this.websocket.send(message);
};
 
Network.prototype.checkSocket = function() {
    if (this.websocket) {
        var stateStr;
        switch (this.websocket.readyState) {
            case 0:
                stateStr = "CONNECTING";
                break;
            case 1:
                stateStr = "OPEN";
                break;
            case 2:
                stateStr = "CLOSING";
                break;
            case 3:
                stateStr = "CLOSED";
                break;
            default:
                stateStr = "UNKNOW";
                break;
        }
        console.log("Websocket state : " + this.websocket.readyState + " (" + stateStr + ")");
    }
    else
        console.log("Websocket is not initialised");
}
var readyStateBefore = -1;
var websocketIsOpened = false;
function websocketChanged(websocket) {
    if(websocket != null) {
        var stateStr;
        if(readyStateBefore != websocket.readyState) {
            readyStateBefore = websocket.readyState;
            switch (websocket.readyState) {
                case 0:
                    stateStr = "CONNECTING";
                    break;
                case 1:
                    stateStr = "OPEN";
                    websocketIsOpened = true;
                    try {
                        websocketOpened();
                    }
                    catch(e) {}
                    break;
                case 2:
                    stateStr = "CLOSING";
                    break;
                case 3:
                    stateStr = "CLOSED";
                    try {
                        websocketClosed();
                    }
                    catch(e) {}
                    websocketIsOpened = false;
                    break;
                default:
                    stateStr = "UNKNOW";
                    break;
            }
            console.log("Socket state changed : " + websocket.readyState + " (" + stateStr + ")");
        }
    }
}