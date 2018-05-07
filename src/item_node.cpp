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
#include "item_mesh.h"
#include "item_image.h"
#include "item_texture.h"

//********************************************************************

QString Item_node::getType()const
{
    return QString("Node");
}

Item_node::Item_node( Item *parent, const QString& name) : Item_matrix( parent, name)
{
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    //Icon = xpm_node;
    setIcon(0, QIcon(":/images/xpm/node.xpm"));
    menuinit = false;
}

/*!
slot for opening the contextmenu
*/
void Item_node::contextmenu(const QPoint& point)
{

    context = this;

    if(!menuinit)
    {
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
bool Item_node::dragAccept(Item* i)
{
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

QObject *Item_node::addBuffer(const QString &label1, unsigned dim, unsigned size, unsigned keyframes, int type, bool normalized_int)
{
    return new Item_buffer(this, label1, dim, size,keyframes, type, normalized_int);
}

QObject *Item_node::addMesh(const QString &name, int numOfVertices)
{
    return new Item_mesh(this, name, numOfVertices);
}

QObject *Item_node::addText(const QString &name)
{
    return new Item_edit(this, name);
}

QObject *Item_node::addImage(const QString &name)
{
    return new Item_image(this, name);
}

/*!
void addArmature([String name])\n
*/
QObject* Item_node::addArmature(const QString& name)
{
    return new Item_armature(this, name);
}

QObject *Item_node::addNode(const QString &name)
{
    return new Item_node(this, name);
}

QObject *Item_node::addScript(const QString &name)
{
    return new Item_script(this, name);
}

QObject *Item_node::addVertexshader(const QString &name)
{
    return new Item_shader(this, name, Item_shader::Vertexshader);
}

QObject *Item_node::addGeometryshader(const QString &name)
{
    return new Item_shader(this, name, Item_shader::Geometryshader);
}

QObject *Item_node::addFragmentshader(const QString &name)
{
    return new Item_shader(this, name, Item_shader::Fragmentshader);
}

QObject *Item_node::addTexture(const QString &name)
{
    return new Item_texture(this, name);
}

QObject *Item_node::addUniform(const QString &name, unsigned dim, unsigned size, unsigned keyframes, int type)
{
    return new Item_uniform(this, name, dim, size, keyframes, type);
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
    while (*it)
    {
        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it)){
            glPushMatrix();
            //simple code to apply the nodes matices, a stack based could be better
            QList<float*> matrixlist;
            Item_matrix* m = reinterpret_cast<Item_matrix*>(scriptitem); //Only used to get the parent
            while((m = dynamic_cast<Item_matrix*>(m->parent()))!=nullptr)
            {
                float *mat =  m->getMatrix();
                matrixlist.append(mat);
            }

            for (int k = matrixlist.size()-1; k >= 0; k--)
            {
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
