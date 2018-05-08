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

#ifndef SCRIPT_LAUNCHER_H
#define SCRIPT_LAUNCHER_H

#include <QAction>
#include <QPointer>
#include <QString>
#include "script_extender_engine.h"
#include "script_header_parser.h"
#include "no_copy.h"
#include "filterableitem.h"

//this is "plugin launcher"

class ScriptLauncher : public QObject, public FilterableItem, public utility::NoCopyAssignMove
{
    Q_OBJECT
public:
    ScriptLauncher(const QString &fn, QObject *parent = nullptr);
    virtual ~ScriptLauncher()override;
    operator QPointer<QAction>() const;
    QPointer<QAction> getAction() const;
public slots:
    void terminate();
private slots:
    void toggled(bool);
    void launch();
private:
    QPointer<QAction> action;
    bool toggle;
    QString fileName;
    QPointer<SEngine> ip;
    ScriptFilePtr scriptFile;
protected:
    void deleteEngine();
};
#endif
