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

#include <QMainWindow>
#include <QTextCodec>
#include "singleapplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
#ifdef QT4
    QTextCodec::setCodecForTr      (QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale  (QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    QCoreApplication::setApplicationName   ("Realtime Message Broker");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName  ("Buzzing Light");
    QCoreApplication::setOrganizationDomain("buzzinglight");

    SingleApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    MainWindow w;
    
    return a.exec();
}
