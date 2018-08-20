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

#include "udp_raw.h"

UdpRaw::UdpRaw(QWidget *_uiFeedback, QObject *parent) :
    QObject(parent), SenderInterface(_uiFeedback) {
    allowLog = true;

    socket = new QUdpSocket(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
}

void UdpRaw::setPort(quint16 port) {
    if(port != socket->localPort()) {
        setUiFeedback(false);

        if((socket->isValid()) && (socket->localPort())) {
            qDebug("Fermeture de UDP sur %d (%d)", socket->localPort(), socket->isValid());
            socket->close();
        }
        if(socket->bind(port)) {
            setUiFeedback(true);
            qDebug("Ouverture UDP sur %d", port);
        }
        else
            qDebug("Echec de l'ouverture TCP sur %d", port);
    }
    else
        qDebug("Port UDP brut déjà ouvert sur %d (%d)", socket->localPort(), socket->isValid());
}

void UdpRaw::socketReadyRead() {
    QString message = QString(socket->readAll());
    MainWindowInterface::main->dispatch(message, QVariantList() << "/osc" << "127.0.0.1" << MainWindowInterface::defaultUdpPort);
}

QString UdpRaw::send(const QByteArray &message, const QString &ip, quint16 port, void *) {
    socket->connectToHost(ip, port);
    socket->waitForConnected(2000);
    socket->write(qPrintable(message));
    socket->disconnectFromHost();
    socket->waitForDisconnected(2000);

    return QString("%3: TCP sent to %1:%2: %4\n").arg(ip).arg(port).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")).arg(QString(message));
}
