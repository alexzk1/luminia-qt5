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

#ifndef _ITEM_INDEX_H
#define _ITEM_INDEX_H

#include "item.h"


/*!
index 
*/
class Item_index: public Item{
Q_OBJECT
friend class Item_mesh;
public:
	Item_index( Item *parent, const QString& label1, int indices_per_primitive=3 , int num =0);
	virtual ~Item_index();

	void setData(QString);
	QString getData();
	int getIPP();
	virtual QString statusText()const;
public slots: 

	void Draw(int mode = -1);
	void DrawInstanced(int instances, int mode = -1);
	void set(int index, int a, int b = 0, int c = 0, int d = 0, int e = 0, int f = 0);
	void add(int a, int b = 0, int c = 0, int d = 0, int e = 0, int f = 0);
	void del(int index, int num);
	
	virtual QString getType()const{return QString("Index");}

protected:
	int* index;
	int num_of_primitives;
	
	unsigned int VBO;
	int needrefresh;
	int indices_per_primitive;

	void resize();
public slots: 
	virtual void contextmenu(const QPoint&);
public:
	static void setup();
	static void create(QObject* obj, int id , void** args);
private: 
	bool menuinit;
	};
#endif
