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
#include <QToolBar>

#include "script_extender.h"
#include "loaderpaths.h"
#include "item.h"

static std::vector<PreloadedScript> scripts;
static std::vector<PreloadedScript> plugins;
using eptr_t = QPointer<SEngine>;

PreloadedScript::PreloadedScript(const QString &fileName):
    filePath(fileName),
    scriptFile(),
    sactions(),
    sexports()
{
    QFile file(fileName);
    scriptFile.reset(new ScriptFile(file)); //make_shared will keep locked by weak ptrs too
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

void ScriptExtender::addActionsForItem(QPointer<QMenu> menu, const QPointer<Item> itm)
{
    if (menu && itm)
    {
        //building "plugins actions" (those are launched as single-file program)
        for (const auto& as : plugins)
        {
            if(as.isForItem(itm))
            {
                QAction *a = menu->addAction(as.scriptFile->tagXpm(), as.scriptFile->tagName());
                a->setToolTip(as.scriptFile->tagDescription() + " (" + as.filePath + ").");
                a->setCheckable(as.scriptFile->tagToggle());
                a->setChecked(false);

                std::shared_ptr<eptr_t> ppEng(new eptr_t(nullptr));
                ScriptFilePtrW ws = as.scriptFile;

                if (!a->isCheckable())
                {
                    QObject::connect(a, &QAction::triggered, itm, [ws, itm, ppEng]()
                    {
                        if (*ppEng)
                        {
                            (*ppEng)->deleteLater();
                        }
                        auto ptr = ws.lock();
                        //somewhen later, when user clicks
                        if (itm && ptr)
                        {
                            (*ppEng) = new SEngine(itm);
                            (*ppEng)->useDefaultError();
                            //bug fix for old samples code, guess currently it had to use "obj"
                            (*ppEng)->run(QString("World.getContext = function() { return %1; };").arg(itm->getFullScriptName()));
                            (*ppEng)->run(ptr->getFileText());
                        }
                    });
                }
                else
                {
                    QObject::connect(a, &QAction::triggered, itm, [ws, itm, ppEng](bool checked)
                    {
                        auto ptr = ws.lock();
                        //somewhen later, when user clicks
                        if (*ppEng)
                            (*ppEng)->deleteLater();
                        if (itm && ptr)
                        {
                            if (checked)
                            {
                                (*ppEng) = new SEngine(itm);
                                (*ppEng)->useDefaultError();
                                //bug fix for old samples code, guess currently it had to use "obj"
                                (*ppEng)->run(QString("World.getContext = function() { return %1; };").arg(itm->getFullScriptName()));
                                (*ppEng)->run(ptr->getFileText());
                            }
                        }
                    });
                }
            }
        }

        //building "script actions" (those should contain functions only which are loaded into namespace later)
        for (const auto& as : scripts)
        {
            if(as.isForItem(itm) && !as.sactions.isEmpty())
            {
                for (const auto& ac : as.sactions)
                {
                    //damn...for each action will have own engine...need that, because ...some actions open dialogs for long
                    QAction *a = menu->addAction(ac.icon, "*" + ac.text);
                    eptr_t engine = new SEngine(itm);
                    engine->useDefaultError();

                    //bug fix for old samples code, guess currently it had to use "obj"
                    engine->run(QString("World.getContext = function() { return %1; };").arg(itm->getFullScriptName()));

                    QObject::connect(a, &QAction::triggered, itm, [ac, itm, engine]()
                    {
                        //somewhen later, when user clicks
                        if (engine)
                        {
                            engine->execJsFunc(ac.slot.funcNameOnly, {});
                        }
                    });
                }
            }
        }
    }
}

script_header_parser::FuncsList ScriptExtender::getImportedMethods(const QPointer<Item> itm)
{
    script_header_parser::FuncsList result;
    result.reserve(100);
    //getting list of methods (slots) which object should export
    for (const auto& as : scripts)
    {
        if(as.isForItem(itm))
            result.append(as.sexports);
    }
    return result;
}

void ScriptExtender::buildToolbar(const QPointer<QToolBar> &toolBar)
{

    if (toolBar)
    {
        for (const auto& as : plugins)
        {
            if(as.isForItem("ScriptToolBar"))
            {
                QAction *a = toolBar->addAction(as.scriptFile->tagXpm(), as.scriptFile->tagName());
                a->setToolTip(as.scriptFile->tagDescription() + " (" + as.filePath + ").");
                a->setCheckable(as.scriptFile->tagToggle());
                a->setChecked(false);

                std::shared_ptr<eptr_t> ppEng(new eptr_t(nullptr));
                ScriptFilePtrW ws = as.scriptFile;

                if (!a->isCheckable())
                {
                    QObject::connect(a, &QAction::triggered, toolBar, [ws, toolBar, ppEng]()
                    {
                        if (*ppEng)
                        {
                            (*ppEng)->deleteLater();
                        }
                        auto ptr = ws.lock();
                        //somewhen later, when user clicks
                        if (ptr)
                        {
                            (*ppEng) = new SEngine(toolBar);
                            (*ppEng)->useDefaultError();
                            (*ppEng)->run(ptr->getFileText());
                        }
                    });
                }
                else
                {
                    QObject::connect(a, &QAction::triggered, toolBar, [ws, toolBar, ppEng](bool checked)
                    {
                        auto ptr = ws.lock();
                        //somewhen later, when user clicks
                        if (*ppEng)
                            (*ppEng)->deleteLater();

                        if (toolBar && ptr)
                        {
                            if (checked)
                            {
                                (*ppEng) = new SEngine(toolBar);
                                (*ppEng)->useDefaultError();
                                (*ppEng)->run(ptr->getFileText());
                            }
                        }
                    });
                }
            }
        }
    }
}

void ScriptExtender::reloadExports()
{
    scripts.clear();
    auto paths = LoaderPaths::listFilesInSubfolder(LoaderPaths::SCRIPTS);
    for(const auto& p : paths)
        scripts.emplace_back(p);
}

void ScriptExtender::reloadPlugins()
{
    plugins.clear();
    auto paths = LoaderPaths::listFilesInSubfolder(LoaderPaths::PLUGINS);
    for(const auto& p : paths)
        plugins.emplace_back(p);
}

void ScriptExtender::reloadAll()
{
    reloadExports();
    reloadPlugins();
}
