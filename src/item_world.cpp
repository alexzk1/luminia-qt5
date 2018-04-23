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

void Item_world::setup(){
	qDebug() << "Item_world::setup()";
	SCRIPTSLOTS(Item_world,"World");
	}




Item_world::Item_world(Item *p, const QString& name) : Item( p,name ){
	Item::world = this;

	//setIcon(0, QIcon(*xpm_world));
	//menu->addAction ( QIcon(*xpm_node),QString("Add Node"),this, SLOT( addNode()));
	//menu->addAction ( QIcon(*xpm_cam),QString("Add Cam"),this, SLOT( addCam()));
	//DQMENU(Item_world)

	//SCRIPT2MENU
	setIcon(0, QIcon(":/images/xpm/world.xpm"));
	setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);
	new DQObject<Item_cam>(this, "Cam");
	}



/*!
slot for opening the contextmenu 
*/
void Item_world::contextmenu(const QPoint& point){

	context = this;

	static bool init = false;
	if(!init){	
	//	menu->addAction ( QIcon(*xpm_node),QString("Add Node"),this, SLOT( addNode()));
		DQMENU(Item_world, menu);
		SCRIPT2MENU(menu);
		init = true;
		}

	menu->popup( point );
	}





/*!
accept only Node items for dragging
*/
bool Item_world::dragAccept(Item* i){
	if (i->getType()=="Node")return true;
	return false;
	}


/*!
returns the time from the timewidget in seconds
*/
double Item_world::getTime(){
	return timev;
	}
/*!
set the worlds time. Usefull for rendering videos
*/
void Item_world::setTime(double t){
	timev = t;
	emit update();
	}

/*!
get the selected Item, function is currently not working and retuns "this"
*/
QObject* Item_world::getSelected(){
	//QListViewItem *tmp = parent->selectedItem();
	return (Item*)this;
	}


/*!
retuns the cama width in pixel. Usefull for generating rendertargets for deferred rendering.
*/
int Item_world::getCamWidth(){
	return CamWidth;
	}
/*!
retuns the cam heigh in pixel
*/
int Item_world::getCamHeight(){
	return CamHeight;
	}

void Item_world::setCamSize(int w, int h){
	CamWidth = w;
	CamHeight = h;
	}

/*!
void Call(String functionname, Array arguments);\n
This function trys to call "functionname" in all Script items in the tree.
Can't be called recursive!
*/
// same code as Item_node::Call();
void Item_world::Call(const QString& function, const QVariantList& args){
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
			while( (m = dynamic_cast<Item_matrix*>(m->parent()))!= NULL){
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
	rec--;
	}
