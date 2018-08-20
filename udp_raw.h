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

#ifndef UDP_RAW_H
#define UDP_RAW_H

#include <QWidget>
#include <QByteArray>
#include <QUdpSocket>
#include <QHash>
#include <QDateTime>
#include "global.h"

class UdpRaw : public QObject, public SenderInterface {
    Q_OBJECT

private:
    bool allowLog;
    QUdpSocket *socket;

public:
    explicit UdpRaw(QWidget *_uiFeedback, QObject *parent = nullptr);

public:
    void setPort(quint16 port);

public:
    void open();
    QString send(const QByteArray &message, const QString &ip = "", quint16 port = 0, void * = nullptr);

signals:
    void outgoingMessage(const QString &log);
public slots:
    void incomingMessage(const QString &, quint16, const QString &, const QVariantList &) {}
private slots:
    void socketReadyRead();
};

#endif // UDP_RAW_H
