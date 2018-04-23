/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for 
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007  oc2k1
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
#include "item_component.h"
#include "item_mesh.h"

void Item_component::create( QObject* obj, int id , void** args){
	QObject* r;
	switch (id){
		case 0: 
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), QString("Vertex"), int(VERTEX), 3);
			break;
		case 1:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "Generic", GENERIC, 3);
			break;
		case 2:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "Vector", VECTOR, 3);
			break;
		case 3:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "Color", COLOR, 3);
			break;
		case 4:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "UvCoords", UVCOORDS, 2);
			break;
		case 5:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "Bonedep", BONEDEP, 3);
			break;
		case 6:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), "Quaternion", QUATERNION, 4);
			break;
		case 7:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), CAST_QSTRING(2), CAST_INT(1), CAST_INT(3));
			break;
		case 8:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), CAST_QSTRING(2), CAST_INT(1), CAST_INT(3), CAST_INT(4));
			break;
		case 9:
			r = new DQObject<Item_component>(dynamic_cast<Item_mesh*>(obj), CAST_QSTRING(2), CAST_INT(1), CAST_INT(3), CAST_INT(4), CAST_BOOL(5));
			break;

		default:
			qDebug() << "item_mesh.cpp unhandled: create(" << obj << ", " << id << ", " << args << ")";
			return;
		}
	if (args[0]) *reinterpret_cast< QObject**>(args[0]) = r;
	}

void Item_component::setup(){

	qDebug() << "Item_node::setup()";

	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addVertex()", "", Item_component::create, 0);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addGeneric()", "", Item_component::create, 1);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addVector()", "", Item_component::create, 2);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addColor()", "", Item_component::create, 3);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addUvCoords()", "", Item_component::create, 4);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addBonedep()", "", Item_component::create, 5);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addQuaternion()", "", Item_component::create, 6);

	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addComponent(int,QString,int)", "componentType,name,dimension", Item_component::create, 7);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addComponent(int,QString,int,int)", "componentType,name,dimension,keyframes", Item_component::create, 8);
	DQObject<Item_mesh>::createCallBackSlot( "QObject*", "addComponent(int,QString,int,int,int)", "componentType,name,dimension,keyframes,format", Item_component::create, 9);

	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/vertex.xpm", "Add Vertex", SLOT(addVertex()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/normal.xpm", "Add Generic", SLOT(addGeneric()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/normal.xpm", "Add Vector", SLOT(addVector()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/color.xpm", "Add Color", SLOT(addColor()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/uvcoord.xpm", "Add UvCoords", SLOT(addUvCoords()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/matrix.xpm", "Add Bonedep", SLOT(addBonedep()));
	DQObject<Item_mesh>::actionlist << Action(":/images/xpm/quaternion.xpm", "Add Quaternion", SLOT(addQuaternion()));

	SCRIPTSLOTS(Item_component,"Component");

	}




Item_component::Item_component( Item_mesh *_parent, const QString& name, int _comptype, int _dimension, int _keyframes, int _format, bool _normalized_int) : 
			Item_buffer( _parent, name, _dimension, _parent->getNumOfVertices(), _keyframes, _format, _normalized_int){
	parent = _parent;
	


	comptype = _comptype;
	switch (comptype){
		case VERTEX:	 setIcon( 0, QIcon(":/images/xpm/vertex.xpm"));	break;
		case GENERIC:	 setIcon( 0, QIcon(":/images/xpm/normal.xpm"));	break;
		case VECTOR:	 setIcon( 0, QIcon(":/images/xpm/normal.xpm"));	break;
		case COLOR:	 setIcon( 0, QIcon(":/images/xpm/color.xpm"));	break;
		case UVCOORDS:	 setIcon( 0, QIcon(":/images/xpm/uvcoord.xpm"));	break;
		case BONEDEP:	 setIcon( 0, QIcon(":/images/xpm/matrix.xpm"));	break;
		case QUATERNION: setIcon( 0, QIcon(":/images/xpm/quaternion.xpm"));	break;
		}

	menuinit = false;
	}

Item_component::~Item_component(){
	}


/*!
slot for opening the contextmenu 
*/
void Item_component::contextmenu(const QPoint& point){

	context = this;

	if(!menuinit){	
		menu->addAction( QString( "Properties"), this, SLOT(PropertiesDialog()) );
		menu->addSeparator();
		DQMENU(Item_edit, menu);
		menu->addSeparator();

		SCRIPT2MENU(menu);
		menu->addSeparator();
		menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));
		menuinit = true;
		}

	menu->popup( point );
	}









/*
QString Item_component::statusText(){
	if (keyframes > 1){
		return QString("%1 Keyframes with %2 %3D Vertices").arg(keyframes).arg(size).arg(dim);
		}
	else{
		return QString("%1 %2D Vertices").arg(size).arg(dim);
		}
	}
*/

/*!
void Bind([number location])\n
Bind a stream to an attribute. Location is optional
*/
void Item_component::Bind(){
	BindKeyFrame(0);
	}

/*!
void BindKeyFrame(number KeyFrame, [number location])\n
Bind the "KeyFrame" of the component
*/

void Item_component::BindKeyFrame(int _key){
	switch (comptype){
		case VERTEX:	Item_buffer::BindKeyFrame(_key, -2); break;
		case VECTOR:	Item_buffer::BindKeyFrame(_key, -3); break;
		case COLOR:	Item_buffer::BindKeyFrame(_key, -4); break;
		case UVCOORDS:	Item_buffer::BindKeyFrame(_key, -5); break;
		}
	}

/*!
void Unbind([number location])\n
Unbind the component. For attributes  use the location as argument.
*/

void Item_component::Unbind(){
	switch (comptype){
		case VERTEX:	Item_buffer::Unbind(-2); break;
		case VECTOR:	Item_buffer::Unbind(-3); break;
		case COLOR:	Item_buffer::Unbind(-4); break;
		case UVCOORDS:	Item_buffer::Unbind(-5); break;
		}
	}




/*!
void setDim(number dimension, number dummy_size, number keyframes, enum format, bool normalized_int)\n
Dimension: value 1 to 4
The second argument will be replaces by the number of vertices in the mesh.
Keyframes: The number of keyframes for this component.
Format: gl.BYTES, gl.FLOAT 
Normalized_int: true = use a range from -1.0 (signed) or 0.0 (unsigned) to 1.0 for byte ,short or int formats.
*/

void Item_component::setDim( int dimension, int, int keyframes, int format, bool normalized){
	Item_buffer::setDim(dimension, parent->getNumOfVertices(), keyframes, format,normalized);
	}




