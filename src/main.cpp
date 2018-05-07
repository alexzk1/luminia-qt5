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
#include <QFileInfo>
#include <QDebug>
#include "script_extender.h"
#include "item.h"
#include "glwrapper.h"

Q_SCRIPT_DECLARE_QMETAOBJECT(glwrapper, QObject*);

int main(int argc, char **argv)
{
    qDebug() << "main()";
    QApplication app(argc, argv);

    app.setApplicationName("Luminia Modern");
    //app.setApplicationVersion("0.1");
    app.setApplicationDisplayName("Luminia Modern");
    app.setOrganizationDomain("pasteover.net");
    app.setOrganizationName("pasteover.net");


    Q_INIT_RESOURCE(lumina);
    app.setWindowIcon(QIcon(":/images/lumina.png"));

    ScriptExtender::setup();
    Item::scanScripts();

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
