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

#include "serial.h"

Serial::Serial(QWidget *_uiFeedback, QObject *parent) :
    QObject(parent), SenderInterface(_uiFeedback) {
    allowLog = true;
    serialPort = new QextSerialPort(QextSerialPort::EventDriven);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(parseSerial()));
    startTimer(1000);
}

void Serial::timerEvent(QTimerEvent *) {
    if(serialPort->isOpen()) {
        if(!getPorts().contains(serialPort->portName()))
            serialPort->close();
    }
    else
        setPort(serialPort->portName());
}

QStringList Serial::getPorts() {
    QStringList serialPorts;
    QList<QextPortInfo> ports = (new QextSerialEnumerator())->getPorts();
    foreach(const QextPortInfo &port, ports)
        serialPorts << port.portName;
    return serialPorts;
}

void Serial::setPort(const QString &portName) {
    if((serialPort->isOpen()) && (serialPort->portName() == portName)) {
        qDebug("Port Serial déjà ouvert sur %s:%d", qPrintable(serialPort->portName()), serialPort->baudRate());
        return;
    }

    setUiFeedback(false);
    if(serialPort->isOpen()) {
        qDebug("Fermeture de Serial sur %s:%d", qPrintable(serialPort->portName()), serialPort->baudRate());
        serialPort->close();
    }

    BaudRateType serialBaudRate = BAUD115200;

    serialPort->setPortName(portName);
    serialPort->setFlowControl(FLOW_OFF);
    serialPort->setParity(PAR_NONE);
    serialPort->setDataBits(DATA_8);
    serialPort->setStopBits(STOP_1);
    serialPort->setBaudRate(serialBaudRate);
    if(serialPort->open(QIODevice::ReadWrite)) {
        setUiFeedback(true);
        qDebug("Ouverture Serial sur %s:%d", qPrintable(serialPort->portName()), serialPort->baudRate());
    }
    else
        qDebug("Echec de l'ouverture de Serial %s:%d : %s", qPrintable(serialPort->portName()), serialPort->baudRate(), qPrintable(serialPort->errorString()));
}

void Serial::parseSerial() {
    QByteArray receptionTemp;
    int a = serialPort->bytesAvailable();
    receptionTemp.resize(a);
    serialPort->read(receptionTemp.data(), receptionTemp.size());
    reception += receptionTemp;

    qint16 currentIndex = 0;
    while(currentIndex < reception.size()) {
        if(reception.at(currentIndex) == 0x0D) {
            QByteArray message = reception.mid(0, currentIndex).trimmed();
            reception = reception.remove(0, currentIndex);
            currentIndex = 0;
            if(!message.isEmpty()) {
                if(!message.startsWith("/"))
                    message = "/" + message;

                //Dispatching
                MainWindowInterface::main->dispatch(message, QVariantList() << "/osc" << "127.0.0.1" << MainWindowInterface::defaultUdpPort);
            }
        }
        currentIndex++;
    }
}

QString Serial::send(const QVariantList &valeurs, const QString &, quint16, const QString &destination, void *) {
    QString message = destination + " ";
    foreach(const QVariant &valeur, valeurs)
        message += valeur.toString() + " ";
    message = message.trimmed();

    qint64 bytesSent = serialPort->write(qPrintable(message + "\n"));
    if(bytesSent < 0)
        qDebug("%s : %lld", qPrintable(serialPort->errorString()), bytesSent);

    return QString("%1 (%2)\n").arg(message).arg(bytesSent);
}
