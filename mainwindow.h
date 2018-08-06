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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QSettings>
#include <QDesktopWidget>
#include <QTouchEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCryptographicHash>
#include <QApplication>
#include "http/http.h"
#include "interfacehttp.h"
#include "udp.h"
#include "serial.h"
#include "websockets.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget, public MainWindowInterface {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    QSystemTrayIcon *tray;
    QAction *showConfig, *showWebPage, *ipAction, *quit;
private:
    Http *httpWeb;
protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void incomingMessage(const QString &messageToLog);
    void log(const QString &_log);
    void action();
public slots:
    void openSettings();
    void openWebpage();
    void openNetwork();
    void openQuit();

private:
    bool guiIsChanging;
    void readSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
