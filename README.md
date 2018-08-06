# Realtime Message Broker
*Realtime Message Broker* is a message broker designed for realtime communication (OSC, HTTP, Websockets, Serial/Arduino) in creative technologies.

# Syntax
All the messages adressed to the broker must start with :
- `/osc <IP dest> <Port number dest> <OSC adress> <OSC arg1> <OSC arg2>…` to send message through OSC packet
- `/http <IP dest> <Port number dest> <HTTP GET arg1> <HTTP GET arg2>` to send an HTTP Get message **not yet working**
- `/serial <data>` to send message through Serial/Arduino interface (115200 bauds)
- `/websockets <data>` to send message to all websockets clients

# Default syntax
If you don't specify the protocol (OSC, HTTP…), all the incoming OSC messages will be prepended with `/websockets` and all the incoming Websockets, HTTP and Serial/Arduino messages will be prepended with `/osc 127.0.0.1 <defaultOSCPortOut>` (`defaultOSCPortOut` can be set in *Settings* page)

# Ports and settings
Ports for incoming messages/connexions can be set in the *Settings* page (trayicon —> *Settings*)

# Webserver
Webpages can be hosted with *Realtime Message Broker* (HTML, CSS, JS, no PHP nor MySQL). Just drop a `index.html` in the app directory and click on *Open hosted webpage* in the trayicon. Note that a page names `index.html` will be automaticaly opened on startup ; name it `index.htm` if you don't want this feature.
