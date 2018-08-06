#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <QWidget>
#include <QBuffer>
#include <QTcpServer>
#include <QUrl>
#include <QUrlQuery>
#include "qwebsockets/websocketserver.h"
#include "qwebsockets/websocket.h"
#include "global.h"

class WebSockets : public QObject, public SenderInterface {
    Q_OBJECT

public:
    explicit WebSockets(QWidget *_uiFeedback, QObject *parent = 0);
    ~WebSockets();

private:
    WebSocketServer webSocketServer;
    QList<WebSocket*> webSocketClients;
private slots:
    void webSocketsNewConnection();
    void webSocketsProcessMessage(const QString &message);
    void webSocketsProcessBinaryMessage(const QByteArray &message);
    void webSocketsSocketDisconnected();
    void webSocketsUpdateConnectedClients();
public:
    void close();

public:
    QString send(const QVariantList &valeurs, const QString &ip = "", quint16 port = 0, const QString &destination = "", void *sender = 0);
    void setPort(quint16 port);

signals:
    void outgoingMessage(const QString &log);
};

#endif // WEBSOCKETS_H
