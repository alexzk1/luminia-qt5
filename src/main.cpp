/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/
#include "incgl.h"

#include <QApplication>


#include "mainwindow.h"

#ifdef Q_WS_X11
#include "x11/spaceball.h"
#define QApplication QXiApplication
#endif

#include "setup.h"
#include <QFileInfo>

int main(int argc, char **argv){

    qDebug() << "main()";

    QApplication app(argc, argv);
    Q_INIT_RESOURCE(lumina);
    setup();
    app.setWindowIcon(QIcon(":/images/lumina.png"));

    MainWindow mainWin;
#ifdef Q_WS_X11
    QObject::connect(&app,SIGNAL(spaceballEvent(int *)),&mainWin,SLOT(spaceballEvent(int *)));
#endif
    mainWin.show();
    qDebug() << "xxxxxxx " << QFileInfo( QCoreApplication::arguments().at(0) ).absolutePath ();
    return app.exec();
    }
