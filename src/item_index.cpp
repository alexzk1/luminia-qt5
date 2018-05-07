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
#include <QDebug>
#include "item_index.h"
#include "item_mesh.h"


#include "incgl.h"

#define GL_CHECK_ERROR()                         \
    do                                              \
{                                               \
    GLenum error = glGetError();                \
    if (error != GL_NO_ERROR)                   \
    fprintf(stderr, "E: %s(%d): %s 0x%X\n", \
    __FILE__, __LINE__,             \
    __PRETTY_FUNCTION__, error);    \
    } while(0)


Item_index::Item_index( Item *parent, const QString& name, int ipp, int num) : Item( parent, name)
{
    num_of_primitives = num;
    indices_per_primitive = ipp;
    index = (int *) malloc(num * ipp * sizeof(int) +1);

    setIcon( 0, QIcon(":/images/xpm/index.xpm") );
    glGenBuffersARB( 1, (GLuint*)&VBO);
    needrefresh = 1;

    menuinit = false;

}


Item_index::~Item_index()
{
    glDeleteBuffersARB(1, (GLuint*)&VBO);
    free(index);
}


/*!
slot for opening the contextmenu
*/
void Item_index::contextmenu(const QPoint& point)
{

    context = this;

    if(!menuinit){

        menu->addSeparator();
        DQMENU(Item_index, menu);
        menu->addSeparator();

        SCRIPT2MENU();
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));
        menuinit = true;
    }

    menu->popup( point );
}


QString Item_index::statusText()const{
    switch(indices_per_primitive){
        case 6:
            return QString("Index %1 Triangles with adjacency").arg(num_of_primitives);
        case 4:
            return QString("Index %1 Quads").arg(num_of_primitives);
        case 3:
            return QString("Index %1 Triangles").arg(num_of_primitives);
        case 2:
            return QString("Index %1 Lines").arg(num_of_primitives);
        default:
            return QString("Index %1 Points").arg(num_of_primitives);
    }
}

/*!
void Draw([Enum mode])\n
The index draw function. Mode should be empty, but it can be used
to overiding the quad or triangle by points
*/
void Item_index::Draw(int mode){
    GL_CHECK_ERROR();
    if (mode == -1){
        switch (indices_per_primitive){
            case  2: mode = GL_LINES;			break;
            case  3: mode = GL_TRIANGLES;			break;
            case  4: mode = GL_QUADS;			break;
            case  6: mode = GL_TRIANGLES_ADJACENCY_EXT;	break;
            default: mode = GL_POINTS;

        }
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, VBO);

    if(needrefresh){
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, num_of_primitives * indices_per_primitive * sizeof(int) , index, GL_DYNAMIC_DRAW_ARB);
    }
    needrefresh = 0;
    GL_CHECK_ERROR();

    profiler->start();
    if (mode == GL_POINT_SPRITE_ARB){
        glEnable(GL_POINT_SPRITE_ARB);
        glDrawElements(GL_POINTS, num_of_primitives * indices_per_primitive, GL_UNSIGNED_INT, (char *) nullptr);
        glDisable(GL_POINT_SPRITE_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
        profiler->stop();
        return;
    }
    glDrawElements(mode, num_of_primitives * indices_per_primitive, GL_UNSIGNED_INT, (char *) nullptr);
    profiler->stop();
    GL_CHECK_ERROR();

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/*!
void DrawInstanced(number instances, [Enum mode])\n
The index draw function with instancing support. Mode should be empty, but it can be used
to overiding the quad or triangle by points
*/
void Item_index::DrawInstanced(int  num_of_i, int mode)
{
    GL_CHECK_ERROR();
    if (mode == -1){
        switch (indices_per_primitive){
            case  2: mode = GL_LINES;			break;
            case  3: mode = GL_TRIANGLES;			break;
            case  4: mode = GL_QUADS;			break;
            case  6: mode = GL_TRIANGLES_ADJACENCY_EXT;	break;
            default: mode = GL_POINTS;

        }
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, VBO);

    if(needrefresh){
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, num_of_primitives * indices_per_primitive * sizeof(int) , index, GL_DYNAMIC_DRAW_ARB);
    }
    needrefresh = 0;
    GL_CHECK_ERROR();

    profiler->start();
    if (mode == GL_POINT_SPRITE_ARB){
        glEnable(GL_POINT_SPRITE_ARB);
        glDrawElements(GL_POINTS, num_of_primitives * indices_per_primitive, GL_UNSIGNED_INT, (char *) NULL);
        glDisable(GL_POINT_SPRITE_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
        profiler->stop();
        return;
    }

    glDrawElementsInstancedEXT(mode, num_of_primitives * indices_per_primitive, GL_UNSIGNED_INT, (char *)NULL, num_of_i);

    profiler->stop();
    GL_CHECK_ERROR();

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}


/*!
void add(number a, number b, number c, number d)
add a primitive to the index. Only dim arguments are valid
*/
void Item_index::set(int i, int a, int b, int c, int d, int e, int f){

    int ofs = i * indices_per_primitive;

    switch (indices_per_primitive){
        case 6: index[ofs + 5]=f;
            [[clang::fallthrough]]; case 5: index[ofs + 4]=e;
            [[clang::fallthrough]]; case 4: index[ofs + 3]=d;
            [[clang::fallthrough]]; case 3: index[ofs + 2]=c;
            [[clang::fallthrough]]; case 2: index[ofs + 1]=b;
            [[clang::fallthrough]]; case 1: index[ofs + 0]=a;
    }
    needrefresh = 1;

}

/*!
void add(number a, number b, number c, number d, number d, number e)
add a primitive to the index. Only dim arguments are valid
*/
void Item_index::add(int a, int b, int c, int d, int e, int f){
    int ofs = num_of_primitives * indices_per_primitive;

    num_of_primitives++;
    resize();

    switch (indices_per_primitive){
        case 6: index[ofs + 5]=f;
            [[clang::fallthrough]]; case 5: index[ofs + 4]=e;
            [[clang::fallthrough]]; case 4: index[ofs + 3]=d;
            [[clang::fallthrough]]; case 3: index[ofs + 2]=c;
            [[clang::fallthrough]]; case 2: index[ofs + 1]=b;
            [[clang::fallthrough]]; case 1: index[ofs + 0]=a;
    }

}

/*!
internal used reszie function
*/
void Item_index::resize(){
    //static int size = 0;
    index = (int *) realloc(index,(num_of_primitives * indices_per_primitive) * sizeof(int));
    needrefresh = 1;
}
/*!
void del(number index, number n)\n
delet at index n primitives
*/
void Item_index::del(int i, int num){
    int ipp = indices_per_primitive;

    memcpy (&index[i * ipp],&index[(i+num) * ipp] ,(num_of_primitives - i - num) * ipp * sizeof(int));
    num_of_primitives -= num;
    resize();
}

QString Item_index::getType() const
{
    return QString("Index");
}

/*!
set data function for lum files
*/
void Item_index::setData(QString content){
    QStringList list = content.split(QRegExp("\\s+"));

    int num_of_indices = list.count();
    num_of_primitives = num_of_indices / indices_per_primitive;
    resize();
    qDebug() << "set" << num_of_indices << "indices";
    for (int i = 0; i < num_of_indices; i ++){
        index[i] = list.at(i).toInt();
    }
}
/*!
get data for lum files
*/
QString Item_index::getData(){
    QString data;
    QString num;
    for (int i = 0; i < num_of_primitives; i ++){
        for ( int k = 0; k < indices_per_primitive; k++){
            num.setNum (index[i * indices_per_primitive + k ]);
            data.append(num).append (" ");
        }
        data.append("\n");
    }
    return data;
}
/*!
void getIPP()\n
returns the number of indices per primitive
*/
int Item_index::getIPP(){
    return indices_per_primitive;
}
