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

#include "http.h"

Http::Http(QObject *parent)
    : QObject(parent) {
    QString pathApplication = MainWindowInterface::pathApplication.absoluteFilePath();

    QString configFileName = QFileInfo(MainWindowInterface::pathApplication.absoluteFilePath() + "/webapp.ini").absoluteFilePath();
    qDebug("Config file\t\t%s", qPrintable(configFileName));

    /*
    // Configure logging
    QSettings* logSettings = new QSettings(configFileName, QSettings::IniFormat, this);
    logSettings->beginGroup("logging");
    Logger* logger = new FileLogger(logSettings,10000);
    logger->installMsgHandler();
    if (logSettings->value("bufferSize",0).toInt()>0 && logSettings->value("minLevel",0).toInt()>0)
        //qDebug(You see these debug messages because the logging buffer is enabled");
    */

    // Configure template loader and cache
    QSettings* templateSettings = new QSettings(configFileName, QSettings::IniFormat);
    templateSettings->beginGroup("templates");
    Static::templateLoader = new TemplateCache(templateSettings, this);

    // Configure session store
    QSettings* sessionSettings = new QSettings(configFileName, QSettings::IniFormat);
    sessionSettings->beginGroup("sessions");
    Static::sessionStore = new HttpSessionStore(sessionSettings, this);

    // Configure static file controller
    QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    QString folder = "";
#ifdef Q_OS_WIN
    folder = "/www";
#endif
    if(pathApplication.contains("Contents/Resources"))
        pathApplication = pathApplication + "/../../..";
    MainWindowInterface::defaultHttpPath = QFileInfo(pathApplication + folder);
    qDebug("WWW Root\t\t%s", qPrintable(MainWindowInterface::defaultHttpPath.absoluteFilePath()));
    Static::fileController = new FileController(fileSettings, QFileInfo(pathApplication + folder).absoluteFilePath().toUtf8(), this);

    /*
    QFile file("/Users/guillaume/Desktop/toto.txt");
    file.open(QFile::WriteOnly);
    file.write(configFileName.toUtf8() + "\n");
    file.write(qPrintable(QFileInfo(pathApplication + folder).absoluteFilePath()));
    file.close();
    */

    // Configure and start the TCP listener
    //qDebug(ServiceHelper: Starting webserver");
    QSettings* listenerSettings = new QSettings(configFileName,  QSettings::IniFormat);
    listenerSettings->beginGroup("listener");
    requestMapper = new RequestMapper(this);
    listener = new HttpListener(listenerSettings, requestMapper);
    MainWindowInterface::defaultHttpPort = listener->serverPort();
    //qDebug(ServiceHelper: Service has started");
}

