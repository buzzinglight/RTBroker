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

#include "requestmapper.h"

RequestMapper::RequestMapper(QObject *parent)
    : HttpRequestHandler(parent) {
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QString path = QString(request.getPath());//.normalized(QString::NormalizationForm_D);
    request.path = qPrintable(path);
    /*
     * TODO
    if(!request.getParameter("redirect").isEmpty()) {
        qint16 playerId = request.getParameter("redirect").toInt();
        response.setHeader("Content-Type", "text/plain");
        QString retourUrl;
        if(playerId < Global::deuxMi->typoringPlayers.count()) {
            retourUrl = Global::deuxMi->typoringPlayers.at(playerId).url;
            if((retourUrl != "typoring_wait") && (retourUrl != "typoring_win") && (retourUrl != "typoring_lost"))
                Global::deuxMi->typoringPlayers[playerId].url.clear();
        }
        response.write(retourUrl.toUtf8(), true);
        return;
    }
    */
    //Chemin normal
    path = request.getPath();
    request.path = request.path.replace("/www", "");
    emit(outgoingMessage(QString("%2: HTTP file request on %1").arg(QString(request.path)).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))));
    response.setHeader("Access-Control-Allow-Origin", "*");
    Static::fileController->service(request, response);
}
