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
//********************************************************************



void Item_node::create( QObject* obj, int id , void** args){
	QObject* r;
	switch (id){
		case 1:
			r = new DQObject<Item_node>(dynamic_cast<Item*>(obj), (*reinterpret_cast< const QString(*)>(args[1])));
			break;
		case 0: 
			r = new DQObject<Item_node>(dynamic_cast<Item*>(obj), "Node");
			break;
		default:
			qDebug() << "item_node.cpp unhandled: create(" << obj << ", " << id << ", " << args << ")";
			return;
		}
	if (args[0]) *reinterpret_cast< QObject**>(args[0]) = r;
	}

void Item_node::setup(){

	qDebug() << "Item_node::setup()";

	DQObject<Item_world>::createCallBackSlot( "QObject*", "addNode()", "", Item_node::create, 0);
	DQObject<Item_world>::createCallBackSlot( "QObject*", "addNode(QString)", "name", Item_node::create, 1);
	DQObject<Item_world>::actionlist << Action(":/images/xpm/node.xpm", "Add Node", SLOT(addNode()));

	DQObject<Item_node>::createCallBackSlot( "QObject*", "addNode()", "", Item_node::create, 0); //reuse code
	DQObject<Item_node>::createCallBackSlot( "QObject*", "addNode(QString)", "name", Item_node::create, 1);
	DQObject<Item_node>::actionlist << Action(":/images/xpm/node.xpm", "Add Node", SLOT(addNode()));

	SCRIPTSLOTS(Item_node,"Node");



	}


Item_node::Item_node( Item *parent, const QString& name) : Item_matrix( parent, name){
	setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled); 
	//Icon = xpm_node;
	setIcon(0, QIcon(":/images/xpm/node.xpm"));
	menuinit = false;
	}


/*!
slot for opening the contextmenu 
*/
void Item_node::contextmenu(const QPoint& point){

	context = this;

	if(!menuinit){	
		DQMENU(Item_node, menu);

menu->addAction( QIcon(":/images/xpm/armature.xpm"), QString("Add Armature") , this, SLOT( addArmature()));

		menu->addSeparator();
		menu->addAction( QIcon( ":/images/xpm/importer.xpm"), QString("Import Model"), this, SLOT(importModel()) );
		menu->addSeparator();
		SCRIPT2MENU(menu);
		menu->addSeparator();
		menu->addAction( QIcon( ":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));


		menuinit = true;
		}

	menu->popup( point );
	}





/*!
accept the most drag events
*/
bool Item_node::dragAccept(Item* i){
	if (i->getType()=="Node")return true;
	if (i->getType()=="Mesh")return true;
	if (i->getType()=="Text")return true;
	if (i->getType()=="Shader")return true;
	if (i->getType()=="Script")return true;
	if (i->getType()=="Texture")return true;
	//if (i->getType()=="Stream")return true;
	if (i->getType()=="Armature")return true;
	if (i->getType()=="Buffer")return true;
	if (i->getType()=="Uniform")return true;
	return false;
	}


/*!
void addArmature([String name])\n
*/
QObject* Item_node::addArmature(const QString& name){
	return new Item_armature(this,name);
	}


/*!
void Call(String functionname, Array arguments);\n
This function trys to call "functionname" in all Script items in the subtree.
Can't be called recursive!
*/
// same code as Item_world::Call();
void Item_node::Call(const QString& function, const QVariantList& args){
	static int rec = 0;
	if(rec > 8)return;
	rec++;
	
	QTreeWidgetItemIterator it(this);
	while (*it) {

		if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it)){
			glPushMatrix();
			//simple code to apply the nodes matices, a stack based could be better
			QList<float*> matrixlist;
			Item_matrix* m = reinterpret_cast<Item_matrix*>(scriptitem); //Only used to get the parent
			while((m = dynamic_cast<Item_matrix*>(m->parent()))!=NULL){
				float *mat =  m->getMatrix();
				matrixlist.append(mat);
				}
			
			for (int k = matrixlist.size()-1; k >= 0; k--){
				glMultMatrixf(matrixlist.at(k));
				}

			scriptitem->Call(function,args);

			Item_buffer::UnbindAll(); // clear all client and attribute arrays

			glPopMatrix();
			}
		++it;
		}
	rec --;
	}




