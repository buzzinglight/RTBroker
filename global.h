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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QFileInfo>
#include <QVariant>
#include <QApplication>
#include <QProcess>
#include <QWidget>


//Common class for all network interfaces
class SenderInterface {
public:
    //QWidget feedback will be colored in green (opened) or red (closed or error)
    explicit SenderInterface(QWidget *_uiFeedback) {
        uiFeedback = _uiFeedback;
    }

protected:
    //Color QWidget depending on success or fault
    QWidget *uiFeedback;
    void setUiFeedback(bool status) {
        if(uiFeedback) {
            if(status)
                uiFeedback->setStyleSheet("color: #00BA98;");
            else
                uiFeedback->setStyleSheet("color: #FF2835;");
        }
    }

public:
    //Generic interface to send a message through the interface
    virtual QString send(const QVariantList &valeurs, const QString &ip = "", quint16 port = 0, const QString &destination = "", void *sender = nullptr) { return QString(); }
    virtual QString send(const QByteArray &message, const QString &ip = "", quint16 port = 0, void *sender = nullptr) { return QString(); }

public:
    //Sets the port name or number
    virtual void setPort(quint16 port)            { Q_UNUSED(port);     }
    virtual void setPort(const QString &portName) { Q_UNUSED(portName); }
};


//Main class interface
class MainWindowInterface {
public:
    static MainWindowInterface *main;

public:
    //Network interfaces
    SenderInterface *osc;
    SenderInterface *udp;
    SenderInterface *tcp;
    SenderInterface *http;
    SenderInterface *webSockets;
    SenderInterface *serial1, *serial2, *serial3, *serial4;

public:
    //Dispatch a message on different interfaces
    const QString dispatch(const QString &message, const QVariantList &defautProtocol, void *sender = nullptr);
    const QString dispatch(const QStringList &message, const QVariantList &defautProtocol, void *sender = nullptr);
    const QString dispatch(QVariantList message, const QVariantList &defautProtocol, void *sender = nullptr);

public:
    //Default values to be shared
    static quint16 defaultUdpPort;
    static quint16 defaultHttpPort;
    static QFileInfo defaultHttpPath;
    static QFileInfo pathApplication;

public:
    virtual void openSettings() = 0;
    virtual void openWebpage() = 0;
    virtual void openNetwork() = 0;
    virtual void openQuit() = 0;

public:
    //Log text in settings window
    virtual void log(const QString &_log) = 0;

public:
    //Launch a bash command
    static QPair<QString,QString> launchCommand(const QString &command, const QStringList &arguments = QStringList(), qint32 waitForStarted = -1, qint32 waitForFinished = -1);
    inline static QPair<QByteArray,QString> format(const QString &buffer) { return format(QByteArray(qPrintable(buffer))); }
    static QPair<QByteArray,QString> format(const QByteArray &buffer);

};

#endif // GLOBAL_H
