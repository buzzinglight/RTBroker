/*
    This file is part of RTBroker.
    Made by Buzzing Light 2013-2015

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
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

#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "../global.h"
#include "httprequesthandler.h"
#include "../http/lib/dumpcontroller.h"
#include "../http/lib/templatecontroller.h"
#include "../http/lib/formcontroller.h"
#include "../http/lib/fileuploadcontroller.h"
#include "../http/lib/sessioncontroller.h"
#include "../http/static.h"
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QBuffer>
#include <QCoreApplication>
#include <QUrl>
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif


class RequestMapper : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject *parent = 0);

signals:
    void outgoingMessage(QString);

public:
    void service(HttpRequest& request, HttpResponse& response);
};

#endif // REQUESTMAPPER_H
