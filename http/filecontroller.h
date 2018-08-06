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

#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QFileInfo>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QDateTime>
#include <QCache>
#include <QMutex>
#include <QProcess>
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

class FileController : public HttpRequestHandler  {
    Q_OBJECT
    Q_DISABLE_COPY(FileController)

public:
    FileController(QSettings* settings, const QByteArray &_docroot, QObject* parent = 0);
    void service(HttpRequest& request, HttpResponse& response);

private:
    QString encoding;
    QByteArray docroot;
    int maxAge;

    struct CacheEntry {
        QByteArray document;
        qint64 created;
        QByteArray filename;
    };
    int cacheTimeout;
    int maxCachedFileSize;
    QCache<QString, CacheEntry> cache;
    QMutex mutex;
    void setContentType(QString file, HttpResponse& response);
};

#endif // FILECONTROLLER_H
