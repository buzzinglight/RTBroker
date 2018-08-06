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

#ifndef INTERFACEHTTP_H
#define INTERFACEHTTP_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QBuffer>
#include <QDateTime>
#include <QApplication>
#include "global.h"
#ifndef QT4
    #include <QUrlQuery>
#endif


class InterfaceHttpServer : public QTcpServer, public SenderInterface {
    Q_OBJECT

public:
    InterfaceHttpServer(QWidget *_uiFeedback, QObject *parent = 0);

private:
    QNetworkAccessManager *http;

public:
    void setPort(quint16 port);
    QString send(const QVariantList &valeurs, const QString &ip = "", quint16 port = 0, const QString &destination = "", void * = 0);

protected:
    void incomingConnection(qintptr socketDescriptor);
private slots:
    void readClient();
    void discardClient();
    void parse(QNetworkReply*);
signals:
    void parseRequest(QNetworkReply*);
    void parseSocket(QTcpSocket*);
};



class InterfaceHttp : public QObject, public SenderInterface {
    Q_OBJECT
    
public:
    explicit InterfaceHttp(QWidget *_uiFeedback, QObject *parent = 0);
    ~InterfaceHttp();

private:
    InterfaceHttpServer *httpServer;
public:
    void setPort(quint16 port);
    QString send(const QVariantList &valeurs, const QString & = "", quint16 = 0, const QString & = "", void * = 0);

private slots:
    void parseRequest(QNetworkReply*);
    void parseSocket(QTcpSocket*);

signals:
    void outgoingMessage(const QString &log);
};

#endif // INTERFACEHTTP_H
