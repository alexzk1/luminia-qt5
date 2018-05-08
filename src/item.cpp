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

QPointer<Item_world> Item::world = nullptr;
QPointer<MainWindow> Item::ws = nullptr;
Profiler * Item::profiler = nullptr;

Item::Item(Item *parent, const QString& name ):
    QObject(parent),
    QTreeWidgetItem(parent, 0),
    menu(nullptr),
    dock(nullptr)
{
    setIcon(0, QIcon(":/images/xmp/world.xpm"));
    setName(name);
    setExpanded(true);
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDropEnabled);
    setName ( name );
}

Item::~Item()
{
    if (dock)
        dock->deleteLater(); // owned by Mainwindow
    resetMenu();
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
    static std::map<QString, int64_t> renameCounter;
    const auto key = getType();
    if (!renameCounter.count(key))
        renameCounter[key] = 0;

    QString name = _name;
    Item* p;
    int64_t counter = std::max(0l, renameCounter.at(key));

    while ((p = parent()->findChild(name)) && p != this)
    {
        name = QString("%1_%2").arg(_name).arg(counter++);
    }
    renameCounter[key] = counter - 1;

    setObjectName(name);
    setText(0, name);

    resetMenu(); // menu holds actions, which holds engines, which are using old name

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
slot for opening the contextmenu
*/
void Item::contextmenu(const QPoint& point)
{
    if (!menu)
    {
        menu = new QMenu(ws);
        addMenu(menu);
        if (menu->actions().size())
            menu->addSeparator();
        buildMenu(menu);
    }

    menu->popup( point );
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

void Item::bindToEngine(QScriptEngine *eng, bool localNames)
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

        QScriptValue val = getEngineObject(*eng, this, localNames);
        if (!val.isValid() || val.isUndefined() || val.isNull())
        {
            recursive((localNames)?eng->globalObject():getEngineParentObject(*eng), this);
            val = getEngineObject(*eng, this, localNames);
        }

        //now exposing methods
        auto mlist = ScriptExtender::getImportedMethods(this);
        for (const auto& method : mlist)
        {
            //qDebug() << "Binding " << method.funcNameOnly << "to " << getType();
            QString expr = QString("%1.%2 = function() {"
                                   "var tmp = obj;"
                                   "obj = %1;"
                                   "var r = %2(arguments);"
                                   "obj = tmp;"
                                   "return r;"
                                   "};")
                           .arg(getFullScriptName()).arg(method.funcNameOnly);
            qDebug() <<expr;
            eng->evaluate(expr);
        }
    }
}

void Item::resetMenu()
{
    if (menu)
        menu->deleteLater();
    menu = nullptr;
}

void Item::buildMenu(QMenu *menu)
{
    if (menu)
    {
        int b1 = menu->actions().size();
        ScriptExtender::addActionsForItem(menu, this);
        if (dock)
        {
            if (menu->actions().size() != b1)
                menu->addSeparator();
            auto a = menu->addAction(QIcon(":/images/xpm/edit.xpm"), QString(tr("Show...")) , this, [this]()
            {
                if (dock)
                {
                    dock->show();
                }
            });
            menu->setDefaultAction(a);
        }
        if (isDeletable())
        {
            if (menu->actions().size() != b1)
                menu->addSeparator();
            menu->addAction( QIcon(":/images/xpm/del.xpm"), QString(tr("Delete")) , this, SLOT( deleteLater()));
        }
    }
}

QScriptValue Item::getEngineParentObject(QScriptEngine &eng) const
{
    return getEngineObject(eng, parent());
}

QScriptValue Item::getEngineObject(QScriptEngine &eng, const Item* obj, bool localName)
{
    if (obj)
    {
        QString name = (localName)? obj->getName() : obj->getFullScriptName();
        return eng.globalObject().property(name);
    }
    return eng.globalObject();
}

bool Item::isDeletable() const
{
    return true;
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

