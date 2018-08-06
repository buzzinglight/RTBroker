include(SingleApplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

QT        += core widgets network


unix:TARGET  = "RTBroker"
macx:TARGET  = "Realtime Message Broker"
win32:TARGET = "Realtime Message Broker"
TEMPLATE     = app

SOURCES   += main.cpp

RESOURCES += RTBroker.qrc

#Interfaces
SOURCES   += mainwindow.cpp interfacehttp.cpp serial.cpp udp.cpp  global.cpp
HEADERS   += mainwindow.h   interfacehttp.h   serial.h   udp.h    global.h
FORMS     += mainwindow.ui

#Websockets
SOURCES   += websockets.cpp qwebsockets/websocket.cpp qwebsockets/websocketserver.cpp qwebsockets/websocketprotocol.cpp qwebsockets/handshakerequest.cpp qwebsockets/handshakeresponse.cpp qwebsockets/dataprocessor.cpp
HEADERS   += websockets.h   qwebsockets/websocket.h   qwebsockets/websocketserver.h   qwebsockets/websocketprotocol.h   qwebsockets/handshakerequest.h   qwebsockets/handshakeresponse.h   qwebsockets/dataprocessor.h

#WebApp
HEADERS   += http/http.h   http/requestmapper.h   http/static.h   http/filecontroller.h
SOURCES   += http/http.cpp http/requestmapper.cpp http/static.cpp http/filecontroller.cpp
true {
    HEADERS += http/lib/dumpcontroller.h    http/lib/templatecontroller.h    http/lib/formcontroller.h    http/lib/fileuploadcontroller.h    http/lib/sessioncontroller.h
    SOURCES += http/lib/dumpcontroller.cpp  http/lib/templatecontroller.cpp  http/lib/formcontroller.cpp  http/lib/fileuploadcontroller.cpp  http/lib/sessioncontroller.cpp
    include(http/lib/qtservice/src/qtservice.pri)
    include(http/lib/bfLogging/src/bfLogging.pri)
    include(http/lib/bfHttpServer/src/bfHttpServer.pri)
    include(http/lib/bfTemplateEngine/src/bfTemplateEngine.pri)
}

#Serial
macx:LIBS           += -framework CoreFoundation -framework IOKit -framework Carbon
macx:SOURCES        += qextserialport/qextserialenumerator_osx.cpp
win32:SOURCES       += qextserialport/win_qextserialport.cpp qextserialport/qextserialenumerator_win.cpp
unix:SOURCES        += qextserialport/posix_qextserialport.cpp
unix:!macx:SOURCES  += qextserialport/qextserialenumerator_unix.cpp
SOURCES             += qextserialport/qextserialport.cpp
HEADERS             += qextserialport/qextserialport.h  qextserialport/qextserialenumerator.h  qextserialport/qextserialport_global.h
win32:DEFINES       += WINVER=0x0501
win32:LIBS          += -lsetupapi -ladvapi32 -luser32
#HEADERS             += qextserialport/qextserialport.h   qextserialport/qextserialenumerator.h   qextserialport/qextserialport_global.h qextserialport/qextserialport_p.h qextserialport/qextserialenumerator_p.h
#SOURCES             += qextserialport/qextserialport.cpp qextserialport/qextserialenumerator.cpp

#Icons & co
macx {
    ICON             = icons/icon.icns
    QMAKE_INFO_PLIST = Info.plist
}
win32:RC_FILE        = icon.rc

