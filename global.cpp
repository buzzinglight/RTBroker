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

#include "global.h"

//Static variables
MainWindowInterface* MainWindowInterface::main = nullptr;
QFileInfo MainWindowInterface::pathApplication;
quint16 MainWindowInterface::defaultUdpPort = 0;
quint16 MainWindowInterface::defaultHttpPort = 0;
QFileInfo MainWindowInterface::defaultHttpPath;

//Dispatch a message on the desired interface - Split a string into QStringList, according to dble-quotes
const QString MainWindowInterface::dispatch(const QString &message, const QVariantList &defautProtocol, void *sender) {
    QStringList messages = message.split(QRegExp(" +(?=(?:[^\']*\'[^\']*\')*[^\']*$)"), QString::SkipEmptyParts);
    for(quint16 i = 0 ; i < messages.count() ; i++)
        messages[i] = messages[i].remove("\'");
    return MainWindowInterface::dispatch(messages, defautProtocol, sender);
}
//Dispatch a message on the desired interface - Converts a QStringList argument into QVariantList argument
const QString MainWindowInterface::dispatch(const QStringList &message, const QVariantList &defautProtocol, void *sender) {
    QVariantList messageV;
    foreach(const QString &item, message) {
        bool ok = false;
        qreal floatVal = item.toFloat(&ok);
        if(ok) {
            qint32 intVal = item.toInt(&ok);
            if(intVal == floatVal)
                messageV.append(intVal);
            else
                messageV.append(floatVal);
        }
        else
            messageV.append(item);
    }
    return dispatch(messageV, defautProtocol, sender);
}
//Dispatch a message on the desired interface
const QString MainWindowInterface::dispatch(QVariantList message, const QVariantList &defautProtocol, void *sender) {
    QString log;
    if(message.count() > 0) {
        QString protocol = message.first().toString();

        if(protocol == "/rtbroker/opensettings")
            openSettings();
        else if(protocol == "/rtbroker/opennetwork")
            openNetwork();
        else if(protocol == "/rtbroker/openwebpage")
            openWebpage();
        else if(protocol == "/rtbroker/openquit")
            openQuit();
        else if((!protocol.contains("/osc")) && (!protocol.contains("/tcp")) && (!protocol.contains("/http")) && (!protocol.contains("/websockets")) && (!protocol.contains("/serial"))) {
            //Add procotol before
            message = QVariantList() << defautProtocol << message;
            protocol = message.first().toString();
        }
        message.removeFirst();

        //Debug / log all arguments
        if(false) {
            qDebug("— %s -", qPrintable(protocol));
            foreach(const QVariant &messageItem, message)
                qDebug("%s", qPrintable(messageItem.toString()));
            qDebug("—");
        }

        //Send accord to desired protocol
        if(((protocol.contains("/osc")) || (protocol.contains("/tcp")) || (protocol.contains("/http"))) && (message.count() >= 3)) {
            QString ip      = message.first().toString();
            message.removeFirst();
            quint16 port    = message.first().toInt();
            message.removeFirst();
            QString destination = message.first().toString();
            message.removeFirst();

            if(protocol.contains("/osc"))
                log += udp ->send(message, ip, port, destination);
            else if(protocol.contains("/http"))
                log += http->send(message, ip, port, destination);
            else if(protocol.contains("/tcp"))
                log += tcp->send(message, ip, port, destination);
        }
        else if((protocol.contains("/websockets")) || (protocol.contains("/serial"))) {
            if(protocol.contains("/websockets"))
                log += webSockets->send(message, "", 0, "", sender);
            else if((protocol.contains("/serial")) || (protocol.contains("/serial1")))
                log += serial1->send(message, "", 0, "", sender);
            else if(protocol.contains("/serial2"))
                log += serial2->send(message, "", 0, "", sender);
            else if(protocol.contains("/serial3"))
                log += serial3->send(message, "", 0, "", sender);
            else if(protocol.contains("/serial4"))
                log += serial4->send(message, "", 0, "", sender);
        }
    }
    return log;
}


//Launch a bash command and return returned string
QPair<QString,QString> MainWindowInterface::launchCommand(const QString &command, const QStringList &arguments, qint32 waitForStarted, qint32 waitForFinished) {
    QPair<QString,QString> retour;
    QString fullCommand = command;
    foreach(const QString &argument, arguments)
        fullCommand += " " + argument;

    QApplication::processEvents();
    QProcess process;
    process.start(command, arguments);
    process.waitForStarted(waitForStarted);
    process.waitForFinished(waitForFinished);
    retour.second += process.readAllStandardOutput().trimmed();
    retour.second += process.readAllStandardError() .trimmed();
    retour.first = fullCommand;
    QApplication::processEvents();
    return retour;
}

