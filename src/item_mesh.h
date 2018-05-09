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

#ifndef _ITEM_MESH_H
#define _ITEM_MESH_H

#include "item.h"
#include "item_index.h"
#include "item_component.h"

/*!
mesh
*/
class Item_mesh : public Item
{
    Q_OBJECT
    Q_ENUMS(comptype)

public:
    enum comptype{VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
    Item_mesh( Item *parent , const QString& label1, int vertices = 0);
    virtual QString statusText() const;


    Q_INVOKABLE QObject* addIndex(const QString& label = "Index" , int numOfVertices = 3);
    Q_INVOKABLE QObject* addVertex();
    Q_INVOKABLE QObject* addGeneric();
    Q_INVOKABLE QObject* addVector();
    Q_INVOKABLE QObject* addColor();
    Q_INVOKABLE QObject* addUvCoords();
    Q_INVOKABLE QObject* addBonedeep();
    Q_INVOKABLE QObject* addQuaternion();
    Q_INVOKABLE QObject* addComponent(int type ,const QString& label1, int dimension, int keyframes= 1, int format=GL_FLOAT);
public slots:

    unsigned getNumOfVertices();

    void setNumOfVertices(unsigned num);

    void Draw(int mode );
    void DrawInstanced(int  num_of_instances, int mode);


    void genNormal(Item_component* position = nullptr);
    void genTangent(Item_component* position = nullptr,Item_component* normal = nullptr,Item_component* texcoord = nullptr);
    void genTexSpaceQuaternion(Item_component* normal = nullptr,Item_component* tangent = nullptr);

    virtual QString getType() const {return QString("Mesh");}
protected:
    unsigned num_of_vertices;
    virtual void addMenu(QMenu *menu);
};

#endif
