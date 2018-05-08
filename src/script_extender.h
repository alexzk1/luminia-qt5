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

#include <QMenu>
#include <QImage>
#include "filterableitem.h"
#include "script_header_parser.h"
#include "script_extender_engine.h"

#ifndef _SCRIPT_EXTENDER_H
#define _SCRIPT_EXTENDER_H

class PreloadedScript : public FilterableItem
{
private:
public:
    PreloadedScript(const QString &fileName);
    virtual ~PreloadedScript() override = default;

    QString filePath;
    ScriptFilePtr scriptFile;
    ScriptFile::ActionsList sactions;
    ScriptFile::FuncsList   sexports;
};

//this is global exports/includes to all scripts

class ScriptExtender
{
public:
    static void loadImported(SEngine *engine); //loads all imports into engine (global namespace)
    static void addActions(QPointer<QMenu> menu, const QPointer<Item> itm);
    static ScriptFile::FuncsList getImportedMethods(const QPointer<Item> itm);


    static void reloadExports();
    static void reloadPlugins();
    static void reloadAll();
};

#endif






