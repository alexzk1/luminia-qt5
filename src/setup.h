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


#include "script_extender.h"
#include "item.h"
#include "item_image.h"
#include "item_virtual.h"
#include "item_component.h"
#include "item_mesh.h"
#include "item_index.h"
#include "item_texture.h"

void setup(){
	
	ScriptExtender::setup();
	

	//Item::setup();
	Item_world::setup();
	Item_cam::setup();
	Item_node::setup();
	Item_edit::setup();
	Item_script::setup();
	Item_shader::setup();
	Item_texture::setup();
	Item_mesh::setup();
	Item_component::setup();
	Item_index::setup();
	Item_uniform::setup();
	Item_buffer::setup();
	

	//Item_image::setup();
	//Item_virtual::setup();

	}