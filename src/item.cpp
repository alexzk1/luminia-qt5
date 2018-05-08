/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007-2008  oc2k1
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
#include "item.h"

#include "palgorithm.h"

#include <QPointer>
#include "mainwindow.h"
#include <QDockWidget>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFontInfo>
#include <QFileInfoList>
#include <QDebug>

#include "loaderpaths.h"

Item * Item::context = nullptr;
Item_world * Item::world = nullptr;
QPointer<MainWindow> Item::ws = nullptr;
Profiler * Item::profiler = nullptr;


QList<ScriptLauncher *> Item::launcher;


Item::Item(Item *parent, const QString& name ):
    QObject(parent),
    QTreeWidgetItem(parent, 0)
{
    dock = nullptr;
    setIcon(0, QIcon(":/images/xmp/world.xpm"));
    setName(name);
    setExpanded(true);
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDropEnabled);
    setName ( name );
    menu = new QMenu();
}

Item::~Item()
{
    if (dock)
        dock->deleteLater(); // owned by Mainwindow
    delete menu;
    qDebug() << "destroyed:" <<  this << childCount ()  ;
}


/*!
function to add a QWidget or similar to the Workspace area
*/
void Item::appendToWs(QWidget *w)
{
    dock = new QDockWidget(objectName(), ws);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(w);
    ws->addDockWidget(Qt::RightDockWidgetArea, dock);
}



/*!
Function for setting the objects name.
*/
void Item::setName(const QString& _name)
{
    static std::map<QString, size_t> renameCounter;
    const auto key = getType();
    if (!renameCounter.count(key))
        renameCounter[key] = 0;

    QString name = _name;
    Item* p;
    while ((p = parent()->findChild(name)) && p != this)
    {
        name = QString("%1_%2").arg(_name).arg(renameCounter[key]++);
    }

    setObjectName(name);
    setText(0, name);
    if (dock)
        dock->setWindowTitle (name);
}

QString Item::getName() const
{
    return objectName();
}


void Item::setData ( int column, int role, const QVariant &value )
{
    if (role == Qt::EditRole && this != world)
    {
        setName(value.toString());
        return;
    }
    QTreeWidgetItem::setData (  column,  role, value );
}
/*!
returns the parent object
*/
Item* Item::parent() const
{
    return qobject_cast<Item*>(QObject::parent());
}


/*!
Function for destroying an object from a script
*//*
void Item::del(){
    //delete(this);
    deleteLater ();
    }
*/

/*!
slot for opening the contextmenu
*/
void Item::contextmenu(const QPoint& point)
{
    if (menu)
    {
        context = this;
        menu->popup( point );
    }
}

QString Item::getType() const
{
    return QString("Item");
}

void Item::destroyAll()
{
    QList<Item*> allItems = findChildren<Item*>();
    std::for_each(allItems.begin(), allItems.end(), [](auto& p)
    {
        if (p)
        {
            p->deleteLater();
        }
    });
}

void Item::deleteLater()
{
    QList<Item*> allItems = findChildren<Item*>(QString(), Qt::FindDirectChildrenOnly);
    if (parent())
    {
        std::for_each(allItems.begin(), allItems.end(), [this](auto& p)
        {
            if (p)
            {
                setParent(nullptr);
                p->deleteLater();
            }
        });
        auto eng = engine();
        if (eng)
        {
            const static QScriptValue invalid;
            getEngineObject(*eng, nullptr).setProperty(getFullScriptName(), invalid);
        }
    }
    QObject::deleteLater();
}

QString Item::getFullScriptName() const
{
    if (!parent())
        return getName();

    return parent()->getFullScriptName() + "." + getName();
}

void Item::bindToEngine(QScriptEngine *eng)
{
    if (eng)
    {
        std::function<void(QScriptValue val, Item* itm)> recursive;
        recursive = [&eng, &recursive](QScriptValue val, Item* itm)
        {
            auto nobj = eng->newQObject(itm);
            val.setProperty(itm->getName(), nobj);
            QList<Item*> directs = itm->findChildren<Item*>(QString(), Qt::FindDirectChildrenOnly);
            for (const auto& p : directs)
            {
                if (p)
                    recursive(nobj, p);
            }
        };

        QScriptValue val = getEngineObject(*eng, this);
        if (!val.isValid() || val.isUndefined() || val.isNull())
            recursive(getEngineParentObject(*eng), this);
    }
}

void Item::binding(QScriptEngine *ep)
{

}

void Item::SCRIPT2MENU()
{
    if (menu)
    {
        for (int i = 0; i < launcher.size(); ++i)
        {
            if (launcher.at(i)->isForItem(this))
                menu->addAction ( launcher.at(i)->getAction());
        }
        ScriptExtender::addActions(menu, this);
    }
}

QScriptValue Item::getEngineParentObject(QScriptEngine &eng) const
{
    return getEngineObject(eng, parent());
}

QScriptValue Item::getEngineObject(QScriptEngine &eng, const Item* obj)
{
    if (obj)
    {
        QString name = obj->getFullScriptName();
        return eng.globalObject().property(name);
    }
    return eng.globalObject();
}

/*!
scan the plugin script directory for tool scripts
*/
void Item::scanScripts()
{
    QStringList paths = LoaderPaths::listFilesInSubfolder(LoaderPaths::PLUGINS, "js");
    for(const QString& p : paths)
        launcher.append(new ScriptLauncher(p, Item::ws));
}

Item* Item::findChild(const QString& name) const
{
    return QObject::findChild<Item *>(name);
}


/*!
returns a list of child objects

*/
QList<Item *> Item::findChildrenByType( const QString & typen) const
{
    auto l = findChildren<Item*>();
    for (int i = l.size() -1; i >= 0; i--)
    {
        if(Item* it = dynamic_cast<Item*>( l[i] ) )
        {
            if(it->getType()!= typen) l.removeAt(i);
        }
        else l.removeAt(i);
    }
    return l;
}

bool Item::dragAccept(Item*)
{
    return false;
}
