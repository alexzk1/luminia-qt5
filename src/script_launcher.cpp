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
#include "script_launcher.h"

//plugin launcher
ScriptLauncher::ScriptLauncher(const QString& fn, QObject *parent):
    QObject(parent),
    FilterableItem (),
    action(nullptr),
    toggle(false),
    fileName(fn),
    ip(nullptr),
    scriptFile(nullptr)
{
    QFile file(fileName);
    scriptFile = std::make_shared<ScriptFile>(file);

    action = new QAction(scriptFile->tagXpm(), scriptFile->tagName(), parent);
    if ((toggle = scriptFile->tagToggle()))
        connect( action, SIGNAL(triggered(bool)), this, SLOT(toggled(bool)));
    else
        connect( action, SIGNAL(triggered()), this, SLOT(launch()));

    action->setCheckable(toggle);
    action->setChecked(false);
    action->setStatusTip (scriptFile->tagDescription());

    setFilter(scriptFile->tagFilter());
}

ScriptLauncher::~ScriptLauncher()
{
    deleteEngine();
    if (action)
        action->deleteLater();
    scriptFile.reset();
}

QPointer<QAction> ScriptLauncher::getAction() const
{
    return action;
}

ScriptLauncher::operator QPointer<QAction>() const
{
    return getAction();
}

#include <QGLContext>

void ScriptLauncher::launch()
{
    qDebug() << "Current context" << QGLContext::currentContext ();
    deleteEngine();
    ip = new SEngine(this);
    ip->useDefaultError();

    QScriptValue globalObject = ip->getEngine().globalObject();

    if (toggle)
        globalObject.setProperty("Launcher" ,ip->getEngine().newQObject(this));

    ip->run(*scriptFile);
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
    if (action)
        action->setChecked(false);
}

void ScriptLauncher::toggled(bool run)
{
    if (run)
        launch();
    else
        deleteEngine();
}
