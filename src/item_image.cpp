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

#include "item_image.h"
#include "item_texture.h"
#include <QDebug>

Item_image::Item_image( Item *parent, const QString& name): Item( parent, name)
{

    menuinit = false;

}

/*!
slot for opening the contextmenu
*/
void Item_image::contextmenu(const QPoint& point){

    context = this;

    if(!menuinit){

        DQMENU(Item_image, menu);
        menu->addSeparator();
        menu->addSeparator();
        SCRIPT2MENU(menu);
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));


        menuinit = true;
    }

    menu->popup( point );
}


void Item_image::load(const QString& )
{




}

void Item_image::subImage(Item_texture *tex, int xTexOfs,int yTexOfs, int, int, int, int )
{
    qDebug() << "SubImage: " << tex << xTexOfs << yTexOfs;
}



