#include "websockets.h"

WebSockets::WebSockets(QWidget *_uiFeedback, QObject *parent) :
    QObject(parent), SenderInterface(_uiFeedback) {
    connect(&webSocketServer, SIGNAL(newConnection()), SLOT(webSocketsNewConnection()));
}

QString WebSockets::send(const QByteArray &message, const QString &, quint16, void *sender) {
    foreach(WebSocket *webSocket, webSocketClients)
        if(webSocket != sender)
            webSocket->send(QString(message));

    return QString("%1 à %2 websockets\n").arg(QString(message)).arg(webSocketClients.count());
}
void WebSockets::setPort(quint16 port) {
    if(port != webSocketServer.serverPort()) {
        setUiFeedback(false);
        if((webSocketServer.isListening()) && (webSocketServer.serverPort())) {
            qDebug("Fermeture de Websockets sur %d", webSocketServer.serverPort());
            webSocketServer.close();
        }
        if(webSocketServer.listen(QHostAddress::Any, port)) {
            setUiFeedback(true);
            qDebug("Ouverture Websockets sur %d", port);
        }
        else
            qDebug("Echec de l'ouverture Websockets sur %d", port);
    }
    else
        qDebug("Port Websockets déjà ouvert sur %d (%d)", webSocketServer.serverPort(), webSocketServer.isListening());
}

void WebSockets::close() {
    foreach(WebSocket *webSocket, webSocketClients) {
        webSocket->disconnect(SIGNAL(textMessageReceived(QString)));
        webSocket->disconnect(SIGNAL(binaryMessageReceived(QByteArray)));
        webSocket->disconnect(SIGNAL(disconnected()));
    }
}

WebSockets::~WebSockets() {
    close();
}

void WebSockets::webSocketsNewConnection() {
    WebSocket *webSocket = webSocketServer.nextPendingConnection();
    connect(webSocket, SIGNAL(textMessageReceived(QString)),      SLOT(webSocketsProcessMessage(QString)));
    connect(webSocket, SIGNAL(binaryMessageReceived(QByteArray)), SLOT(webSocketsProcessBinaryMessage(QByteArray)));
    connect(webSocket, SIGNAL(disconnected()),                    SLOT(webSocketsSocketDisconnected()));
    webSocketClients << webSocket;
    webSocketsUpdateConnectedClients();
    webSocket->send(QString("/connect %1 %2").arg(webSocket->peerAddress().toString()).arg(webSocket->peerPort()));
}
void WebSockets::webSocketsProcessMessage(const QString &message) {
    WebSocket *webSocketSender = qobject_cast<WebSocket *>(sender());
    if(webSocketSender)
        MainWindowInterface::main->dispatch(message, QVariantList() << "/osc" << "127.0.0.1" << MainWindowInterface::defaultUdpPort, webSocketSender);
}
void WebSockets::webSocketsProcessBinaryMessage(const QByteArray &) {
    WebSocket *webSocketSender = qobject_cast<WebSocket *>(sender());
    if(webSocketSender) {

    }
}
void WebSockets::webSocketsSocketDisconnected() {
    WebSocket *webSocket = qobject_cast<WebSocket *>(sender());
    if(webSocket) {
        webSocketClients.removeAll(webSocket);
        webSocket->deleteLater();
    }
    webSocketsUpdateConnectedClients();
}
void WebSockets::webSocketsUpdateConnectedClients() {
    QString clientsWebSockets;
    foreach(WebSocket *webSocket, webSocketClients)
        clientsWebSockets += QString("%1:%2, ").arg(webSocket->peerAddress().toString()).arg(webSocket->peerPort());
    clientsWebSockets.chop(2);
    if     (webSocketClients.count() == 0)   qDebug("%s", qPrintable(tr("No websocket connected")));
    else if(webSocketClients.count() == 1)   qDebug("%s", qPrintable(tr("1 websocket connected : %1").arg(clientsWebSockets)));
    else                                     qDebug("%s", qPrintable(tr("%1 websockets connected : %2").arg(webSocketClients.count()).arg(clientsWebSockets)));
}
