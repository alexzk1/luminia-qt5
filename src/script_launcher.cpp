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




#include <qscriptengine.h>

#include "factory/factory.h"

#include "script_launcher.h"

#include "item.h"
#include "glwrapper.h"
#include <QMessageBox>
#include "mainwindow.h"

ScriptLauncher::ScriptLauncher(QString fn, QObject * parent):
    QObject( parent)
{
    QPixmap *pix;
    QString name;
    ip = nullptr;
    fileName = fn;
    //Load XPM
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text ))
        qDebug() << "Error launching script";

    QRegExp rx( "<XPM>.+\"(.+)\".+</XPM>" );
    QString in = file.readAll();
    rx.indexIn(in,0);
    QString captured = rx.cap( 1 );
    captured.replace( QRegExp("\"[^\"&.]+\""), "\n" );
    QByteArray ascii = captured.toUtf8();
    const char *xpmbytes=ascii.data();

    char * xpmfinal = new char[captured.length()];
    int lines = 1;
    for (int i=0;i<captured.length();i++){
        if (xpmbytes[i]=='\n'){
            xpmfinal[i]=0;
            lines++;
        }
        else xpmfinal[i]=xpmbytes[i];
    }

    const char ** xpmptrs = new const char*[lines+1];
    xpmptrs[0] = xpmfinal;
    int j = 1;
    for (int i=0;i<captured.length();i++){
        if (xpmfinal[i]=='\0'){
            xpmptrs[j]=xpmfinal + i + 1;
            j++;
        }
    }

    pix = new QPixmap(xpmptrs);
    delete[] xpmfinal;
    delete[] xpmptrs;

    QRegExp fx( "<FILTER>(.+)</FILTER>" );

    if (fx.indexIn(in,0))filter = new QRegExp(fx.cap( 1 ));
    else filter = new QRegExp("(.+)");

    QRegExp nx( "<NAME>(.+)</NAME>" );
    if (nx.indexIn(in,0))
        name = nx.cap( 1 );
    else
        name = fn;

    QString tooltip;
    QRegExp tx( "<DESCRIPTION>(.+)</DESCRIPTION>" );
    if (tx.indexIn(in,0))tooltip = tx.cap( 1 );


    a = new QAction(QIcon(*pix), name , parent);
    a->setStatusTip ( tooltip );

    toggle = false;
    QRegExp px( ".*<TOGGLE/>.*" );
    if (px.exactMatch(in))
    {
        toggle=true;
        qDebug() << "toggle";
        a->setCheckable ( true );
        QObject::connect( a, SIGNAL(triggered(bool)), this, SLOT(toggled(bool)));
    }
    else
    {
        QObject::connect( a, SIGNAL(triggered()), this, SLOT(launch()));
    }
    delete pix;

    ogl = new glwrapper(this,"gl");
}

ScriptLauncher::~ScriptLauncher()
{
    deleteEngine();
    delete a;
    delete filter;
    delete ogl;
}

#include <QGLContext>

void ScriptLauncher::launch()
{
    qDebug() << "Current context" << QGLContext::currentContext ();
    deleteEngine();
    ip = new SEngine(this, fileName);
    ip->useDefaultError();

    QScriptValue globalObject = ip->getEngine().globalObject();

    if (toggle)
        globalObject.setProperty("Launcher" ,ip->getEngine().newQObject(this));

    ip->run();
    deleteEngine();
}

void ScriptLauncher::deleteEngine()
{
    if (ip)
        ip->deleteLater();
    ip = nullptr;
}

void ScriptLauncher::terminate()
{
    deleteEngine();
    a->setChecked (false);
}

void ScriptLauncher::toggled(bool run)
{
    if (run)
        launch();
    else
        deleteEngine();
}
