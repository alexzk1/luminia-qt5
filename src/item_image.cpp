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

void Item_image::create( QObject* obj, int id , void** args){
    QObject* r;

    Item_texture* tex;
    Item_image* img;

    switch (id){
        case 0:
            r = new DQObject<Item_image>(dynamic_cast<Item*>(obj), QString("Image"));
            break;
        case 1:
            r = new DQObject<Item_buffer>(dynamic_cast<Item*>(obj), CAST_QSTRING(1));
            break;
        case 2:
            if((tex = dynamic_cast<Item_texture*>(obj)) && (img = dynamic_cast<Item_image*>(*reinterpret_cast<QObject*(*)>(args[1])))){
                img->subImage(tex,CAST_INT(2),CAST_INT(3),CAST_INT(4),CAST_INT(5));
                }
            return;
        case 3:
            if((tex = dynamic_cast<Item_texture*>(obj)) && (img = dynamic_cast<Item_image*>(*reinterpret_cast<QObject*(*)>(args[1])))){
                img->subImage(tex,CAST_INT(2),CAST_INT(3),CAST_INT(4),CAST_INT(5),CAST_INT(6),CAST_INT(7));
                }
            return;

        default:
            qDebug() << "item_image.cpp unhandled: create(" << obj << ", " << id << ", " << args << ")";
            return;
        }
    if (args[0]) *reinterpret_cast< QObject**>(args[0]) = r;
    }

void Item_image::setup(){

    qDebug() << "Item_image::setup()";

    DQObject<Item_node>::createCallBackSlot( "QObject*", "addImage()", "", Item_image::create, 0);
    DQObject<Item_node>::createCallBackSlot( "QObject*", "addImage(QString)", "name", Item_image::create, 1);

    DQObject<Item_node>::actionlist << Action(":/images/xpm/image.xpm", "Add Image", SLOT(addImage()));
    SCRIPTSLOTS(Item_image,"Image");

    DQObject<Item_texture>::createCallBackSlot( "", "subImage2D(QObject*,int,int,int,int)", "xTexOfs,yTexOfs,width,height", Item_image::create, 2);
    DQObject<Item_texture>::createCallBackSlot( "", "subImage2D(QObject*,int,int,int,int,int,int)", "xTexOfs,yTexOfs,width,height,xImgOfs,yImgofs", Item_image::create, 3);


    }


Item_image::Item_image( Item *parent, const QString& name): Item( parent, name){

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




void Item_image::load(const QString& ){




    }

void Item_image::subImage(Item_texture *tex, int xTexOfs,int yTexOfs, int, int, int, int ){
    qDebug() << tex << xTexOfs << yTexOfs;


    }



