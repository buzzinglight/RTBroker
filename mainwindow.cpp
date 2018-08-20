/*
    This file is part of RTBroker
    Made by Buzzing Light 2014-2015 — Buzzing Light

    Development:     Guillaume Jacquemin (https://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    RTBroker is a free software: you can redistribute it and/or modify
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    guiIsChanging = false;
    MainWindowInterface::main = this;

    //Tray menu
    tray = new QSystemTrayIcon(this);
    QMenu* menu = new QMenu(this);
    quit = new QAction("Quit", this);
    connect(quit, SIGNAL(triggered(bool)), SLOT(openQuit()));

    //Application patch
    QDir pathApplicationDir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
#ifdef Q_OS_MAC
    if((pathApplicationDir.absolutePath().contains("/RTBroker-build")) || (pathApplicationDir.absolutePath().contains("/RTBroker-debug"))) {
        pathApplicationDir.cdUp();
        pathApplicationDir.cdUp();
        pathApplicationDir.cdUp();
    }
    else {
        pathApplicationDir.cdUp();
        pathApplicationDir.cd("Resources");
    }
#endif
    MainWindowInterface::pathApplication = QFileInfo(pathApplicationDir.absolutePath());
    if(MainWindowInterface::pathApplication.absoluteFilePath().contains("/RTBroker-build/release"))
        MainWindowInterface::pathApplication = QFileInfo(MainWindowInterface::pathApplication.absoluteFilePath().remove("/release"));
    if(MainWindowInterface::pathApplication.absoluteFilePath().contains("/RTBroker-build"))
        MainWindowInterface::pathApplication = QFileInfo(MainWindowInterface::pathApplication.absoluteFilePath().remove("-build"));
    if(MainWindowInterface::pathApplication.absoluteFilePath().contains("/RTBroker-debug"))
        MainWindowInterface::pathApplication = QFileInfo(MainWindowInterface::pathApplication.absoluteFilePath().remove("-debug"));

    //Network identification
    QString networkMac, networkIp, networkName;
    QList<QNetworkInterface> networkInterfaces = QNetworkInterface::allInterfaces();
    foreach(const QNetworkInterface &networkInterface, networkInterfaces) {
        bool otherCondition = true;
#ifdef Q_OS_MAC
        otherCondition = networkInterface.humanReadableName().startsWith("en");
#endif
        if((otherCondition) && ((networkInterface.flags() & QNetworkInterface::IsLoopBack) == 0) && (networkInterface.flags() & QNetworkInterface::IsUp) && (networkInterface.flags() & QNetworkInterface::IsRunning) && (networkInterface.flags() & QNetworkInterface::CanMulticast)) {
            QList<QNetworkAddressEntry> addresses = networkInterface.addressEntries();
            foreach(const QNetworkAddressEntry &address, addresses) {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    //networkInterfaceMulticast = networkInterface;
                    networkMac  = networkInterface.hardwareAddress();
                    networkIp   = address.ip().toString();
                    networkName = networkInterface.humanReadableName();
                }
            }
        }
    }

    //Current IP & settings
    ipAction = new QAction(QString("Current IP is %1").arg(networkIp), this);
    connect(ipAction, SIGNAL(triggered(bool)), SLOT(openNetwork()));
    menu->addAction(ipAction);
    showConfig = new QAction("Settings", this);
    connect(showConfig, SIGNAL(triggered(bool)), SLOT(openSettings()));
    menu->addAction(showConfig);
    menu->addSeparator();

    if(QFileInfo(MainWindowInterface::pathApplication.absoluteFilePath() + "/webapp.ini").exists()) {
        showWebPage = new QAction("Open hosted webpage", this);
        connect(showWebPage, SIGNAL(triggered(bool)), SLOT(openWebpage()));
        menu->addAction(showWebPage);
        menu->addSeparator();
    }
    menu->addAction(quit);

    tray->setContextMenu(menu);
#ifdef Q_OS_MAC
    QIcon icon(":/icons/icon_black");
#else
    QIcon icon(":/icons/icon");
#endif
    tray->setIcon(icon);
    tray->show();
    tray->showMessage("Realtime Message Broker", QString("Services are ready!").arg(networkIp));

    //Ouverture des interfaces réseau
    osc  = new Udp(ui->oscInPort, this);
    connect((Udp*)osc,               SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    udp  = new UdpRaw(ui->udpInPort, this);
    connect((UdpRaw*)udp,            SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    tcp  = new Tcp(ui->tcpInPort, this);
    connect((Tcp*)tcp,               SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    http = new InterfaceHttp(ui->httpInPort, this);
    connect((InterfaceHttp*)http,    SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    webSockets = new WebSockets(ui->httpWebSockets, this);
    connect((WebSockets*)webSockets, SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    serial1 = new Serial(ui->serial1PortName, this);
    connect((Serial*)serial1,        SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    serial2 = new Serial(ui->serial2PortName, this);
    connect((Serial*)serial2,        SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    serial3 = new Serial(ui->serial3PortName, this);
    connect((Serial*)serial3,        SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));
    serial4 = new Serial(ui->serial4PortName, this);
    connect((Serial*)serial4,        SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));

    //Interfaces complexes
    httpWeb = new Http(this);
    connect(httpWeb->requestMapper,  SIGNAL(outgoingMessage(QString)), SLOT(incomingMessage(QString)));

    QSettings settings;
    qDebug("Settings\t\t%s", qPrintable(settings.fileName()));

    readSettings();
    saveSettings();

    if((MainWindowInterface::defaultHttpPort) && (QFile(MainWindowInterface::defaultHttpPath.absoluteFilePath() + "/index.html").exists()))
        QDesktopServices::openUrl(QUrl(QString("http://127.0.0.1:%1/").arg(MainWindowInterface::defaultHttpPort)));
}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *) {
    //hide();
    //event->ignore();
}

void MainWindow::readSettings() {
    guiIsChanging = true;

    QStringList serialPorts = ((Serial*)serial1)->getPorts();
    ui->serial1PortName->clear();
    ui->serial2PortName->clear();
    ui->serial3PortName->clear();
    ui->serial4PortName->clear();
    ui->serial1PortName->addItems(serialPorts);
    ui->serial2PortName->addItems(serialPorts);
    ui->serial3PortName->addItems(serialPorts);
    ui->serial4PortName->addItems(serialPorts);

    QSettings settings;
    quint16 websocketsPort = settings.value("port_websocketsPort", 5553).toInt();
    quint16 httpPort       = settings.value("port_httpPort",       5554).toInt();
    QString serial1Port    = settings.value("port_serial1Port",    "").toString();
    QString serial2Port    = settings.value("port_serial2Port",    "").toString();
    QString serial3Port    = settings.value("port_serial3Port",    "").toString();
    QString serial4Port    = settings.value("port_serial4Port",    "").toString();
    quint16 oscPort        = settings.value("port_oscPort",        4001).toInt();
    quint16 udpPort        = settings.value("port_udpPort",        4002).toInt();
    quint16 tcpPort        = settings.value("port_tcpPort",        4002).toInt();
    defaultUdpPort         = settings.value("port_defaultUdpPort", 57130).toInt();
    //quint16 httpWebPort  = ui->httpWebInPort->value();//settings.value("port_httpWebPort", MainWindowInterface::defaultHttpPort).toInt();
    ui->oscInPort     ->setValue(oscPort);
    ui->udpInPort     ->setValue(udpPort);
    ui->tcpInPort     ->setValue(tcpPort);
    ui->defaultUdpPort->setValue(defaultUdpPort);
    ui->httpInPort    ->setValue(httpPort);
    ui->httpWebSockets->setValue(websocketsPort);
    ui->serial1PortName->setCurrentText(serial1Port);
    ui->serial2PortName->setCurrentText(serial2Port);
    ui->serial3PortName->setCurrentText(serial3Port);
    ui->serial4PortName->setCurrentText(serial4Port);
    //ui->httpWebInPort ->setValue(httpWebPort);

    qDebug("Port local OSC par défaut : %d", defaultUdpPort);

    osc       ->setPort(oscPort);
    udp       ->setPort(udpPort);
    tcp       ->setPort(tcpPort);
    http      ->setPort(httpPort);
    webSockets->setPort(websocketsPort);
    serial1   ->setPort(serial1Port);
    serial2   ->setPort(serial2Port);
    serial3   ->setPort(serial3Port);
    serial4   ->setPort(serial4Port);
    //httpWebI->setPort(httpWebPort);

    guiIsChanging = false;
}
void MainWindow::saveSettings() {
    if(!guiIsChanging) {
        QSettings settings;
        settings.setValue("port_oscPort",        ui->oscInPort->value());
        settings.setValue("port_tcpPort",        ui->tcpInPort->value());
        settings.setValue("port_udpPort",        ui->udpInPort->value());
        settings.setValue("port_httpPort",       ui->httpInPort->value());
        settings.setValue("port_websocketsPort", ui->httpWebSockets->value());
        settings.setValue("port_defaultUdpPort", ui->defaultUdpPort->value());
        settings.setValue("port_serial1Port",    ui->serial1PortName->currentText());
        settings.setValue("port_serial2Port",    ui->serial2PortName->currentText());
        settings.setValue("port_serial3Port",    ui->serial3PortName->currentText());
        settings.setValue("port_serial4Port",    ui->serial4PortName->currentText());
        //settings.setValue("port_httpWebPort",    ui->httpWebInPort->value());
        readSettings();
    }
}

void MainWindow::action() {
    if((sender() == ui->oscInPort) || (sender() == ui->udpInPort) || (sender() == ui->tcpInPort) || (sender() == ui->httpInPort) || (sender() == ui->httpWebSockets) || (sender() == ui->serial1PortName) || (sender() == ui->serial2PortName) || (sender() == ui->serial3PortName) || (sender() == ui->serial4PortName))
        saveSettings();
    else if(sender() == ui->httpSample)
        QDesktopServices::openUrl(QUrl(QString("http://127.0.0.1:%1/?protocol=/osc&ip=127.0.0.1&port=%2&destination=/http&valeur1=123&valeur2=Text&valeur3=456").arg(ui->httpInPort->value()).arg(defaultUdpPort)));
}

void MainWindow::openSettings() {
    readSettings();
    show();
    raise();
}
void MainWindow::openWebpage() {
    QDesktopServices::openUrl(QUrl(QString("http://127.0.0.1:%1/").arg(MainWindowInterface::defaultHttpPort)));
}
void MainWindow::openNetwork() {
#ifdef Q_OS_WIN
        launchCommand("C:\\Windows\\System32\\control.exe", QStringList() << "ncpa.cpl");
#endif
#ifdef Q_OS_MAC
        launchCommand("open", QStringList() << "/System/Library/PreferencePanes/Network.prefPane/");
#endif
#ifdef Q_OS_LINUX
        launchCommand("nm-connection-editor");
#endif
}
void MainWindow::openQuit() {
    QApplication::exit();
}

void MainWindow::incomingMessage(const QString &messageToLog) {
    log(messageToLog);
}
void MainWindow::log(const QString &messageToLog) {
    ui->log->setText(messageToLog);
}
