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


#ifndef _ITEM_IMAGE_H
#define _ITEM_IMAGE_H


#include "item.h"


class Item_image: public Item
{
    Q_OBJECT
public:
    Item_image(Item *parent, const QString& label1);

public slots:
    void load(const QString& filename = "");

public slots:
    virtual void contextmenu(const QPoint&) override;

public:
    Q_INVOKABLE void subImage(Item_texture *tex, int xTexOfs,int yTexOfs, int width, int height, int xImgOfs = 0, int yImgofs = 0);

private:
    bool menuinit;
};

#endif





