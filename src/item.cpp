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


#include <QPointer>
#include<QMainWindow>
#include <QDockWidget>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFontInfo>
#include <QFileInfoList>
#include <QDebug>


Item * Item::context = nullptr;
Item_world * Item::world = nullptr;
QMainWindow * Item::ws = nullptr;
Profiler * Item::profiler = nullptr;


QList<ScriptLauncher *> Item::launcher;


Item::Item(Item *parent, const QString& name ):
    QObject(parent),
    QTreeWidgetItem(parent, 0)
{
    dock = nullptr;
    setup();
    setIcon(0, QIcon(":/images/xmp/world.xpm"));
    setName(name);
    setExpanded(true);
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDropEnabled);
    setName ( name );
    menu = new QMenu();
    qDebug() << "created:" <<  this << childCount ()  ;
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
void Item::appendToWs(QWidget *w){
    dock = new QDockWidget(objectName(), ws);
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    dock->setWidget(w);
    ws->addDockWidget(Qt::RightDockWidgetArea, dock);
}



/*!
Function for setting the objects name. Automatic renaming double names is currently defect.
*/
void Item::setName(const QString& _name){
    QString name = _name;
    setObjectName ("___renaming");
    Item *res = (Item*)parent()->findChild(name);
    if (res ){
        if(!name.contains(QRegExp("\\d"))) name.append("_0");

        QStringList parts = name.split("_");
        bool ok;
        qDebug() << parts;
        name = parts[0].append("_%1").arg(parts[1].toUInt(&ok, 10) + 1);
        qDebug() << name;
        setName(name);
        return;
    }
    setObjectName (name);
    setText(0,name);
    if (dock)dock->setWindowTitle (name);
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
Item* Item::parent()
{
    Item* i = (Item*) QObject::parent();
    return i;
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
function to init static XPM icons
*/
void Item::setup()
{
    qDebug() << "Item::setup()";
}

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

/*!
scan the plugin script directory for tool scripts
*/
void Item::scanScripts()
{
    QStringList paths;
    paths << QString(QFileInfo( QCoreApplication::arguments().at(0) ).absolutePath ()) + "/plugins/";
    paths << QString(QDir::homePath ())+ "/.lumina/plugins/";

    for(int p = 0; p < paths.size(); p++){


        QDir dir(paths.at(p));

        dir.setFilter( QDir::Files | QDir::NoSymLinks );
        dir.setSorting( QDir::Size | QDir::Reversed );

        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.suffix()=="js")
                launcher.append(new ScriptLauncher(QString(paths.at(p)).append(fileInfo.fileName()),Item::ws));
        }

    }


}

QObject* Item::findChild(const QString& name)
{
    return QObject::findChild<QObject *>(name);
}


/*!
returns a list of child objects

*/
QObjectList Item::findChildrenByType ( const QString & typen) const
{

    QObjectList l = findChildren<QObject*>();

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

