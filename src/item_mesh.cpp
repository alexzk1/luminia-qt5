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
#include "item_mesh.h"
#include "incgl.h"
#include <math.h>



void Item_mesh::create( QObject* obj, int id , void** args){
    QObject* r;
    switch (id){
        case 0:
            r = new DQObject<Item_mesh>(dynamic_cast<Item*>(obj), "Mesh");
            break;
        case 1:
            r = new DQObject<Item_mesh>(dynamic_cast<Item*>(obj), (*reinterpret_cast< const QString(*)>(args[1])));
            break;
        case 2:
            r = new DQObject<Item_mesh>(dynamic_cast<Item*>(obj), (*reinterpret_cast< const QString(*)>(args[1])), (*reinterpret_cast< int(*)>(args[2])));
            break;
        default:
            qDebug() << "item_mesh.cpp unhandled: create(" << obj << ", " << id << ", " << args << ")";
            return;
        }
    if (args[0]) *reinterpret_cast< QObject**>(args[0]) = r;
    }

void Item_mesh::setup(){

    qDebug() << "Item_mesh::setup()";

    DQObject<Item_node>::createCallBackSlot( "QObject*", "addMesh()", "", Item_mesh::create, 0); //reuse code
    DQObject<Item_node>::createCallBackSlot( "QObject*", "addMesh(QString)", "name", Item_mesh::create, 1);
    DQObject<Item_node>::createCallBackSlot( "QObject*", "addMesh(QString,int)", "name,numOfVertices", Item_mesh::create, 2);
    DQObject<Item_node>::actionlist << Action(":/images/xpm/stream.xpm", "Add Mesh", SLOT(addMesh()));

    SCRIPTSLOTS(Item_mesh,"Mesh");
    }


Item_mesh::Item_mesh( Item *parent, const QString& label1, int vertices) : Item( parent,label1 ){
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDragEnabled);
    setIcon( 0, QIcon(":/images/xpm/stream.xpm") );

    num_of_vertices = vertices;
    menuinit = false;
    }


/*!
slot for opening the contextmenu
*/
void Item_mesh::contextmenu(const QPoint& point){

    context = this;

    if(!menuinit){
        QMenu *add = menu->addMenu (QString("Add"));

        DQMENU(Item_mesh, add);


        menu->addSeparator();
    menu->addAction( QIcon(":/images/xpm/normal.xpm"), QString("generate Normal"), this, SLOT( genNormal()) );
    menu->addAction( QIcon(":/images/xpm/normal.xpm"), QString("generate Tangent"), this, SLOT( genTangent()) );
    menu->addAction( QIcon(":/images/xpm/quaternion.xpm"), QString("generate Texspace Quaternion"), this, SLOT( genTexSpaceQuaternion()));

        menu->addSeparator();
        SCRIPT2MENU(menu);
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));


        menuinit = true;
        }

    menu->popup( point );
    }



QString Item_mesh::statusText() const{
    return QString("Mesh  %1 Vertices").arg(num_of_vertices);
    }

/*!
Object addIndex(String name, Number vertices_per_primitive)\n
*/
QObject* Item_mesh::addIndex(const QString& label1 , int verts_per_prim){
    return new DQObject<Item_index>(this, label1, verts_per_prim, 0);
    }
/*!
Object addComponent(Enum Type, String name="Component", Number dimension=4)\n
*/
QObject* Item_mesh::addComponent(int type ,const QString& label1, int dimension, int keyframes, int format){
    return new DQObject<Item_component>(this, label1, type, dimension, keyframes, format);
    }

/*!
void Draw(Enum mode)\n
Draw the mesh without index. Recommend for gl.POINTS and gl.POINT_SPRITES.
*/
void Item_mesh::Draw(int mode){
    profiler->start();
    if (mode == GL_POINT_SPRITE_ARB){
        glEnable(GL_POINT_SPRITE_ARB);
        glDrawArrays(GL_POINTS, 0 , num_of_vertices);
        glDisable(GL_POINT_SPRITE_ARB);
        profiler->stop();
        return;
        }

    glDrawArrays(mode, 0 , num_of_vertices);
    profiler->stop();
    }

/*!
void DrawInstanced(number instances, Enum mode)\n
Draw "instances" times the mesh without index. Recommend for gl.POINTS and gl.POINT_SPRITES.
*/
void Item_mesh::DrawInstanced( int  num_of_i, int mode){
    profiler->start();
    if (mode == GL_POINT_SPRITE_ARB){
        glEnable(GL_POINT_SPRITE_ARB);
        glDrawArraysInstancedEXT(GL_POINTS, 0 , num_of_vertices, num_of_i);
        glDisable(GL_POINT_SPRITE_ARB);
        profiler->stop();
        return;
        }

    glDrawArraysInstancedEXT(mode, 0 , num_of_vertices, num_of_i);
    profiler->stop();
    }





/*!
void setNumOfVertices(Number n)\n
set the number of vertices to n
*/
void Item_mesh::setNumOfVertices(int num){

    num_of_vertices = num;

    QTreeWidgetItemIterator it(this);
    while (*it) {
        if (Item_component* ic = dynamic_cast<Item_component*>(*it)){
            ic->setDim( ic->getDim(), ic->getSize(), ic->getKeyFrames(), ic->getFormat(), ic->isNormalizedInt());
            }
        ++it;
        }
    }

/*!
number getNumOfVertices()\n
get the number of vertices
*/
int Item_mesh::getNumOfVertices(){
    return num_of_vertices;
    }
