/*
    This file is part of RTBroker
    Made by Buzzing Light 2014-2015 — Buzzing Light

    Development:     Guillaume Jacquemin (https://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    HTTP2OSC is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tcp.h"

Tcp::Tcp(QWidget *_uiFeedback, QObject *parent) :
    QTcpServer(parent), SenderInterface(_uiFeedback) {
    allowLog = true;

    sendSocket = new QTcpSocket(this);
    sendSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(sendSocket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
    connect(sendSocket, SIGNAL(connected()),    SLOT(socketConnected()));
    connect(sendSocket, SIGNAL(readyRead()),    SLOT(socketReadyRead()));
    connect(sendSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
}

void Tcp::setPort(quint16 port) {
    if(port != serverPort()) {
        setUiFeedback(false);

        if((isListening()) && (serverPort())) {
            qDebug("Fermeture de TCP sur %d (%d)", serverPort(), isListening());
            close();
        }
        if(listen(QHostAddress::Any, port)) {
            setUiFeedback(true);
            qDebug("Ouverture TCP sur %d (%d)", port, isListening());
        }
        else
            qDebug("Echec de l'ouverture TCP sur %d", port);
    }
    else
        qDebug("Port TCP déjà ouvert sur %d (%d)", serverPort(), isListening());
}


void Tcp::socketDisconnected() {
    QTcpSocket *socket = (QTcpSocket*)sender();
    qDebug("Déconnexion TCP %s:%d", qPrintable(socket->localAddress().toString()), socket->localPort());
}
void Tcp::socketConnected() {
    QTcpSocket *socket = (QTcpSocket*)sender();
    qDebug("Connexion TCP %s:%d", qPrintable(socket->localAddress().toString()), socket->localPort());
}
void Tcp::socketError(QAbstractSocket::SocketError err) {
    qDebug("Erreur TCP %d", err);
}
void Tcp::socketReadyRead() {
    QTcpSocket *socket = (QTcpSocket*)sender();
    QString message = QString(socket->readAll()).trimmed();
    MainWindowInterface::main->dispatch(message, QVariantList() << "/osc" << "127.0.0.1" << MainWindowInterface::defaultUdpPort);
}

QString Tcp::send(const QByteArray &message, const QString &ip, quint16 port, void *) {
    sendSocket->connectToHost(ip, port);
    sendSocket->waitForConnected(2000);
    sendSocket->write(qPrintable(message));
    sendSocket->disconnectFromHost();
    sendSocket->waitForDisconnected(2000);

    return QString("%3: TCP sent to %1:%2: %4\n").arg(ip).arg(port).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(QString(message));
}


void Tcp::incomingConnection(qintptr handle) {
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()),    this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(discardClient()));
    socket->setSocketDescriptor(handle);
}
void Tcp::discardClient() {
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}
