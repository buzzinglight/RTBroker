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

#ifndef STATIC_H
#define STATIC_H

#include <QString>
#include "templatecache.h"
#include "httpsessionstore.h"
#include "filecontroller.h"

class Static {
public:
    static TemplateLoader* templateLoader;
    static HttpSessionStore* sessionStore;
    static FileController* fileController;
};

#endif // STATIC_H
