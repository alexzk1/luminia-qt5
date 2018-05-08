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
#include <vector>
#include "script_extender.h"
#include "loaderpaths.h"
#include "item.h"

static std::vector<PreloadedScript> scripts;

PreloadedScript::PreloadedScript(const QString &fileName):
    filePath(fileName),
    scriptFile(nullptr),
    sactions(),
    sexports()
{
    QFile file(fileName);
    scriptFile = std::make_shared<ScriptFile>(file);
    setFilter(scriptFile->tagFilter());
    sactions = scriptFile->tagAction();
    sexports = scriptFile->tagFunction();
}


void ScriptExtender::loadImported(SEngine *engine)
{
    for (const auto& as : scripts)
    {
        engine->run(as.scriptFile->getFileText());
    }
}

void ScriptExtender::addActions(QPointer<QMenu> menu, const QPointer<Item> itm)
{
    if (menu && itm)
    {
        for (const auto& as : scripts)
        {
            if(as.isForItem(itm) && !as.sactions.isEmpty())
            {
                for (const auto& ac : as.sactions)
                {
                    QAction *a = menu->addAction(ac.icon, "*" + ac.text);
                    QObject::connect(a, &QAction::triggered, itm, [itm, ac]()
                    {
                        //making special "action engine" and execute function inside it
                        if (itm)
                        {
                            SEngine *e = new SEngine(itm); //constructor will load all imports
                            e->execJsFunc(ac.slot.funcNameOnly, {});
                            e->deleteLater();
                        }
                    });
                }
            }
        }
    }
}

void ScriptExtender::setup()
{
    QStringList paths = LoaderPaths::listFilesInSubfolder(LoaderPaths::SCRIPTS);
    for(const auto& p : paths)
        scripts.emplace_back(p);
}
