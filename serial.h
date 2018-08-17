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

#ifndef SERIAL_H
#define SERIAL_H

#include <QWidget>
#include <QByteArray>
#include <QUdpSocket>
#include <QHash>
#include "qextserialport/qextserialport.h"
#include "qextserialport/qextserialenumerator.h"
#include "global.h"

class Serial : public QObject, public SenderInterface {
    Q_OBJECT

private:
    bool allowLog;
private:
    QextSerialPort *serialPort;
    QByteArray      reception;
    QString         lastPortName;

public:
    explicit Serial(QWidget *_uiFeedback, QObject *parent = 0);

public:
    void setPort(const QString &portName);
    QStringList getPorts();

protected:
    void timerEvent(QTimerEvent *);

public:
    void open();
    QString send(const QVariantList &valeurs, const QString &ip = "", quint16 port = 0, const QString &destination = "", void *sender = 0);

signals:
    void outgoingMessage(const QString &log);

public slots:
    void incomingMessage(const QString &, quint16, const QString &, const QVariantList &) {}
private slots:
    void parseSerial();
};

#endif // SERIAL_H
