/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007 - 2008 oc2k1
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

#include "item_virtual.h"


bool Item_virtual::exist = false;

void Item_virtual::create( QObject* obj, int id , void** args)
{
    if(exist) return;

    QObject* r;
    switch (id){
        case 0:
            r = new DQObject<Item_virtual>(dynamic_cast<Item*>(obj), QString("Virtual"));
            break;
        case 1:
            r = new DQObject<Item_virtual>(dynamic_cast<Item*>(obj), CAST_QSTRING(1));
            break;

        default:
            qDebug() << "item_image.cpp unhandled: create(" << obj << ", " << id << ", " << args << ")";
            return;
    }
    if (args[0]) *reinterpret_cast< QObject**>(args[0]) = r;
}

void Item_virtual::setup()
{
    qDebug() << "Item_image::setup()";
    DQObject<Item_world>::createCallBackSlot( "QObject*", "addVirtual()", "", Item_virtual::create, 0);
    DQObject<Item_world>::createCallBackSlot( "QObject*", "addVirtual(QString)", "name", Item_virtual::create, 1);

    DQObject<Item_world>::actionlist << Action(":/images/xpm/virtual.xpm", "Add Virtual", SLOT(addVirtual()));
    SCRIPTSLOTS(Item_virtual,"Virtual");
}

Item_virtual::Item_virtual( Item *parent, const QString& name): Item( parent, name)
{
    menuinit = false;
    exist = true;
}

Item_virtual::~Item_virtual()
{
    exist = false;

}

/*!
slot for opening the contextmenu
*/
void Item_virtual::contextmenu(const QPoint& point)
{

    context = this;

    if(!menuinit)
    {
        DQMENU(Item_virtual, menu);
        menu->addSeparator();
        menu->addSeparator();
        SCRIPT2MENU(menu);
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));
        menuinit = true;
    }
    menu->popup( point );
}






