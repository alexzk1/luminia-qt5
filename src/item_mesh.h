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
class Item_mesh : public Item{
Q_OBJECT
Q_ENUMS(comptype)

public:
	enum comptype{VERTEX, GENERIC, VECTOR, COLOR, UVCOORDS, BONEDEP, QUATERNION};
	Item_mesh( Item *parent , const QString& label1, int vertices = 0);
	virtual QString statusText() const;

	QObject* addComponent(int type ,const QString& label1, int dimension, int keyframes= 1, int format=GL_FLOAT); //deprecated
	QObject* addIndex(const QString& label = "Index" , int verts_per_prim = 3);  //deprecated

public slots: 

	int getNumOfVertices();
	
	void setNumOfVertices(int num);

	void Draw(int mode );
	void DrawInstanced(int  num_of_instances, int mode);


	void genNormal(Item_component* position = NULL);
	void genTangent(Item_component* position = NULL,Item_component* normal = NULL,Item_component* texcoord = NULL);
	void genTexSpaceQuaternion(Item_component* normal = NULL,Item_component* tangent = NULL);
	
	virtual QString getType() const {return QString("Mesh");}
protected:
	int num_of_vertices;

public slots: 
	virtual void contextmenu(const QPoint&);
public:
	static void setup();
	static void create(QObject* obj, int id , void** args);
private: 
	bool menuinit;
	};

#endif
