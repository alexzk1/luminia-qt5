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

#ifndef _ITEM_COMPONENT_H
#define _ITEM_COMPONENT_H

#include "item.h"

class Item_mesh;


/*!
component
*/

class Item_component : public Item_buffer
{
    Q_OBJECT Q_ENUMS(comptype)
public:
    enum comptype {VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
    Item_component(Item_mesh *parent, const QString& label1, int comptype, int dimension, int keyframes = 1, int format = GL_FLOAT, bool normalized_int = true);
    ~Item_component() override = default;
    int getCompType()
    {
        return comptype;
    }
public slots:
    void setDim( unsigned dimension = 1, unsigned dummy = 0, unsigned keyframes = 1, unsigned format = GL_FLOAT, bool normalized_int = true) override; //the parent size will be used
    void Bind();
    void BindKeyFrame(int key);
    void Unbind();
    QString getType()const override;
protected:
    int comptype;
    Item_mesh *parent;
    void addMenu(QMenu *menu) override;
};


#endif
