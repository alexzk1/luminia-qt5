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

#include "globals.h"
#include <QPointer>
#include "mainwindow.h"
#include <QDockWidget>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFontInfo>
#include <QFileInfoList>
#include "loaderpaths.h"
#include "dock_prep.h"

QPointer<Item_world> Item::world = nullptr;
QPointer<MainWindow> Item::ws = nullptr;
QPointer<Profiler>   Item::profiler = nullptr;

Item::Item(Item *parent, const QString& name ):
    QObject(parent),
    QTreeWidgetItem(parent, 0),
    dock(nullptr),
    menu(nullptr)
{
    setIcon(0, QIcon(":/images/xmp/world.xpm"));
    setName(name);
    setExpanded(true);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled);
}

Item::~Item()
{
    if (dock)
        dock->deleteLater(); // owned by MainWindow
    resetMenu();
}


/*!
function to add a QWidget or similar to the Workspace area
*/

extern const Qt::DockWidgetAreas DOCK_AREAS;
void Item::appendToWs(QWidget *w)
{
    dock = nsDocks::createDockFromWidget(w, objectName());
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
    while (parent() && (p = (qobject_cast<QObject*>(parent()))->findChild<Item*>(name)) && p != this)
        name = QString("%1_%2").arg(_name).arg(counter++);
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
        QMenu *submenu = nullptr;

        //listing meta-methods "add*", despite what documentation says, it does not list parent's methods. So have to do long way...
        std::set<QString> once; //ensuring that if they fix qt, program will be working yet
        const auto processObject = [&submenu, this, &once](const QMetaObject * metaObject)
        {
            for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
            {
                const auto method = metaObject->method(i);
                auto n = QString::fromLatin1(method.name());

                if (!once.count(n) && method.parameterCount() == 0 && n.startsWith("add", Qt::CaseInsensitive))
                {
                    once.insert(n);
                    if (!submenu)
                        submenu = menu->addMenu(tr("Add"));
                    n.remove("add", Qt::CaseInsensitive);
                    QString icon_name = QString(":/images/xpm/%1.xpm").arg(n.toLower());
                    auto a = submenu->addAction(QIcon(icon_name), n);
                    connect(a, &QAction::triggered, this, [method, this]()
                    {
                        method.invoke(this, Qt::QueuedConnection);
                    });
                }
            };
        };

        auto meta = this->metaObject();
        while (meta)
        {
            processObject(meta);
            meta = meta->superClass();
        }

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
    std::for_each(allItems.begin(), allItems.end(), [](auto & p)
    {
        if (p)
            p->deleteLater();
    });
}

void Item::hideDock()
{
    if (dock)
        dock->hide();
}

void Item::deleteLater()
{
    QList<Item*> allItems = findChildren<Item*>(QString(), Qt::FindDirectChildrenOnly);
    if (parent())
    {
        std::for_each(allItems.begin(), allItems.end(), [this](auto & p)
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
        recursive = [&eng, &recursive](QScriptValue val, Item * itm)
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
            recursive((localNames) ? eng->globalObject() : getEngineParentObject(*eng), this);
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
                                   "var r = %2.apply(null, arguments);"
                                   "obj = tmp;"
                                   "return r;"
                                   "};")
                           .arg(getFullScriptName()).arg(method.funcNameOnly);
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

QString Item::saveDock() const
{
    QString r;
    if (dock)
    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QDataStream os(&buffer);
        os << dock->isFloating();
        os << dock->saveGeometry();
        r = buffer.buffer().toBase64();
    }
    return r;
}

void Item::restoreSavedDock(const QString &src)
{
    if (dock && !src.isEmpty())
    {
        auto tmp = QByteArray::fromBase64(src.toUtf8());
        QBuffer buffer(&tmp);
        buffer.open(QIODevice::ReadOnly);
        QDataStream is(&buffer);
        bool a;
        is >> a;
        QByteArray g;
        is >> g;
        dock->setFloating(a);
        dock->restoreGeometry(g);
    }
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
            auto a = menu->addAction(QIcon(":/images/xpm/edit.xpm"), QString(tr("Show...")), this, [this]()
            {
                if (dock)
                    dock->show();
            });
            menu->setDefaultAction(a);
        }
        if (isDeletable())
        {
            if (menu->actions().size() != b1)
                menu->addSeparator();
            menu->addAction( QIcon(":/images/xpm/del.xpm"), QString(tr("Delete")), this, SLOT( deleteLater()));
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
        QString name = (localName) ? obj->getName() : obj->getFullScriptName();
        return eng.globalObject().property(name);
    }
    return eng.globalObject();
}

bool Item::isDeletable() const
{
    return true;
}


//those 2 are called from scripts...
QObject* Item::findChild(const QString& name) const
{
    return QObject::findChild<QObject *>(name);
}


/*!
returns a list of child objects

*/
QList<Item *> Item::findChildrenByType( const QString & typen) const
{
    auto l = findChildren<Item*>();
    for (int i = l.size() - 1; i >= 0; i--)
    {
        if (Item* it = dynamic_cast<Item*>( l[i] ) )
        {
            if (it->getType() != typen) l.removeAt(i);
        }
        else l.removeAt(i);
    }
    return l;
}

bool Item::dragAccept(Item*)
{
    return false;
}

QString Item::statusText() const
{
    return tr("Type: ") + getType();
}

