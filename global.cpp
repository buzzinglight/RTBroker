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
        else if((!protocol.contains("/osc")) && (!protocol.contains("/tcp")) && (!protocol.contains("/udp")) && (!protocol.contains("/http")) && (!protocol.contains("/websockets")) && (!protocol.contains("/serial"))) {
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
        if(((protocol.contains("/osc")) || (protocol.contains("/tcp")) || (protocol.contains("/udp")) || (protocol.contains("/http"))) && (message.count() >= 3)) {
            QString ip      = message.first().toString();
            message.removeFirst();
            quint16 port    = message.first().toInt();
            message.removeFirst();
            QString destination = message.first().toString();
            message.removeFirst();

            if(protocol.contains("/osc"))
                log += osc ->send(message, ip, port, destination);
            else if(protocol.contains("/http"))
                log += http->send(message, ip, port, destination);
            else {
                QByteArray messageStr = qPrintable(destination + " ");
                if(message.count()) {
                    foreach(const QVariant &valeur, message) {
                        QString valeurStr = valeur.toString();
                        if((valeur.type() == QVariant::String) && (valeurStr.contains(" ")))
                            messageStr += "\'" + valeurStr + "\' ";
                        else
                            messageStr += valeurStr + " ";
                    }
                }
                messageStr = messageStr.trimmed();
                messageStr = format(messageStr).first;

                if(protocol.contains("/tcp"))
                    log += tcp->send(messageStr, ip, port);
                else if(protocol.contains("/udp"))
                    log += tcp->send(messageStr, ip, port);
            }
        }
        else if((protocol.contains("/websockets")) || (protocol.contains("/serial"))) {
            QByteArray messageStr;
            if(message.count()) {
                foreach(const QVariant &valeur, message) {
                    QString valeurStr = valeur.toString();
                    if((valeur.type() == QVariant::String) && (valeurStr.contains(" ")))
                        messageStr += "\'" + valeurStr + "\' ";
                    else
                        messageStr += valeurStr + " ";
                }
            }
            messageStr = messageStr.trimmed();

            if(protocol.contains("/websockets"))
                log += webSockets->send(messageStr, "", 0, sender);
            else {
                messageStr = "";
                foreach(const QVariant &valeur, message)
                    messageStr += valeur.toString() + " ";
                messageStr = messageStr.trimmed();
                messageStr = format(messageStr).first;

                if((protocol.contains("/serial")) || (protocol.contains("/serial1")))
                    log += serial1->send(messageStr, "", 0, sender);
                else if(protocol.contains("/serial2"))
                    log += serial2->send(messageStr, "", 0, sender);
                else if(protocol.contains("/serial3"))
                    log += serial3->send(messageStr, "", 0, sender);
                else if(protocol.contains("/serial4"))
                    log += serial4->send(messageStr, "", 0, sender);
            }
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


QPair<QByteArray,QString> MainWindowInterface::format(const QByteArray &buffer) {
    if((buffer.startsWith("0x")) && (buffer.count() > 2)) {
        QStringList bufferSplit = QString(buffer).split(" ", QString::SkipEmptyParts);
        foreach(const QString &bufferItemStr, bufferSplit) {
            QByteArray bufferHex, bufferItem;
            QString bufferHexCheck, bufferItemCleaned = bufferItemStr;
            bool ok = false;

            //Macros
            QStringList bufferItemVars = bufferItemCleaned.toLower().split(QRegExp("(?=(?:[^\']*\'[^\']*\')*[^\']*$)"), QString::SkipEmptyParts);
            for(quint16 i = 2 ; i < bufferItemVars.count() ; ) {
                QString bytes = "";
                if(bufferItemVars.at(i).length() == 1) {
                    bytes = (bufferItemVars.at(i)+bufferItemVars.at(i+1));
                    i += 2;
                }
                else {
                    //Macros
                    QStringList commandArgs = bufferItemVars[i].remove("'").split("|");
                    if((commandArgs.first() == "mod") || (commandArgs.first() == "xor") || (commandArgs.first() == "2s")) {
                        quint16 kStart = 0, kEnd = bufferHex.length();
                        if(commandArgs.length() > 1) kStart = commandArgs.at(1).toInt();
                        if(commandArgs.length() > 2) kEnd   = commandArgs.at(2).toInt();

                        qint32 sum = 0, xorr = 0;
                        for(quint16 k = kStart ; k < kEnd ; k++) {
                            quint8 valeur = (unsigned char)bufferHex.at(k);
                            sum += valeur;
                            xorr ^= valeur;
                        }
                        sum = sum % 0x100;

                        if(commandArgs.first() == "xor")
                            bytes = QString("%1").arg(QString::number(xorr & 0xFF, 16).toUpper(), 2, QChar('0'));
                        if(commandArgs.first() == "mod")
                            bytes = QString("%1").arg(QString::number(sum & 0xFF, 16).toUpper(), 2, QChar('0'));
                        if(commandArgs.first() == "2s")
                            bytes = QString("%1").arg(QString::number((~sum + 1) & 0xFF, 16).toUpper(), 2, QChar('0'));
                    }
                    i++;
                }
                for(quint16 j = 0 ; j < bytes.length() ; j+=2) {
                    QString byte = bytes.mid(j, 2).toUpper();
                    quint8 val = byte.toInt(&ok, 16);
                    bufferItem += byte;
                    bufferHex += val;
                }
            }
            bufferItem = bufferItem.trimmed();

            //Vérification en hexa
            for(quint16 i = 0 ; i < bufferHex.count() ; i++)
                bufferHexCheck += QString("0x%1 ").arg(QString::number((unsigned char)bufferHex.at(i), 16).toUpper(), 2, QChar('0'));
            bufferHexCheck = bufferHexCheck.trimmed();

            //Envoi
            return qMakePair(bufferHex, bufferHexCheck);
        }
    }
    else {
        QString bufferPostMacro = buffer;

        //Valeurs en hexa
        for(qint16 k = 255 ; k >= 0; k--)
            bufferPostMacro = bufferPostMacro.replace(qPrintable(QString("\\%1").arg(QString::number(k), 3, QChar('0'))), QByteArray(1, k));

        //Macros
        QStringList bufferItemVars = bufferPostMacro.split(QRegExp("(?=(?:[^\']*\'[^\']*\')*[^\']*$)"), QString::SkipEmptyParts);
        bufferPostMacro = "";
        for(quint16 i = 0 ; i < bufferItemVars.count() ; i++) {
            QString bytes = "";
            if(bufferItemVars.at(i).length() == 1)
                bytes = bufferItemVars.at(i);
            else {
                //Macros
                QStringList commandArgs = bufferItemVars[i].toLower().remove("'").split("|");
                if((commandArgs.first() == "mod") || (commandArgs.first() == "xor") || (commandArgs.first() == "2s")) {
                    quint16 kStart = 0, kEnd = bufferPostMacro.length();
                    if(commandArgs.length() > 1) kStart = commandArgs.at(1).toInt();
                    if(commandArgs.length() > 2) kEnd   = commandArgs.at(2).toInt();

                    qint32 sum = 0, xorr = 0;
                    for(quint16 k = kStart ; k < kEnd ; k++) {
                        quint8 valeur = (unsigned char)bufferPostMacro.at(k).toLatin1();
                        sum += valeur;
                        xorr ^= valeur;
                    }
                    sum = sum % 0x100;

                    if(commandArgs.first() == "xor")
                        bytes = QChar(xorr & 0xFF);
                    if(commandArgs.first() == "mod")
                        bytes = QChar(sum & 0xFF);
                    if(commandArgs.first() == "2s")
                        bytes = QChar((~sum + 1) & 0xFF);
                }
            }
            bufferPostMacro += bytes;
        }

        //Vérification en hexa
        QString bufferHexCheck;
        for(quint16 i = 0 ; i < bufferPostMacro.count() ; i++)
            bufferHexCheck += QString("0x%1 ").arg(QString::number((unsigned char)bufferPostMacro.at(i).toLatin1(), 16).toUpper(), 2, QChar('0'));
        bufferHexCheck = bufferHexCheck.trimmed();

        //Envoi
        return qMakePair(QByteArray(qPrintable(bufferPostMacro)), bufferHexCheck);
    }
}

