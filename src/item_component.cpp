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


Item_component::Item_component( Item_mesh *_parent, const QString& name, int _comptype, int _dimension, int _keyframes, int _format, bool _normalized_int) :
    Item_buffer( _parent, name, _dimension, _parent->getNumOfVertices(), _keyframes, _format, _normalized_int)
{
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
}

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

QString Item_component::getType() const
{
    return QString("Component");
}

void Item_component::addMenu(QMenu *menu)
{
    menu->addAction( tr( "Properties"), this, SLOT(PropertiesDialog()) );
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




