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
#include "item.h"
#include "editors/Cebitor.h"
#include "editors/searchbox.h"

Item_shader::Item_shader( Item *parent, const QString& name, int _shadertype) :
    Item_edit(1, parent, name)
{
    initParent(); //this allows to properly call createTextEditor
    shadertype = _shadertype;
    if (shadertype == Vertexshader)
        setIcon(0, QIcon(":/images/xpm/vertexshader.xpm"));
    else
        setIcon(0, QIcon(":/images/xpm/fragmentshader.xpm"));
}

int Item_shader::getShaderType()
{
    return shadertype;
}

QString Item_shader::statusText() const
{
    const static QStringList names =
    {
        tr("Vertex"), tr("Geometry"), tr("Fragments"), tr("Wrong")
    };
    int id = shadertype;
    if (id < Vertexshader || id > Fragmentshader)
        id = names.size() - 1;
    return QString("%1, %2").arg(Item::statusText()).arg(names.at(id));
}

QWidget *Item_shader::createTextEditor(QWidget *parent) const
{
    return new Cebitor(searchBox, true, parent);
}
