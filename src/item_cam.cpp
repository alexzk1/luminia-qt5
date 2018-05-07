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
#include "glcam.h"
#include <QDebug>

//********************************************************************
Item_cam::Item_cam( Item_world *parent, const QString& name) : Item_matrix( parent, name){
    setIcon( 0,  QIcon(":/images/xpm/cam.xpm"));

    deletable = true;
    if (!GLCam::shareWidget)deletable = false;

    else menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));

    cam = new  GLCam(nullptr, this);

    qDebug() << GLCam::shareWidget << cam;

    appendToWs(cam);
    SCRIPT2MENU(menu)
            connect(world, SIGNAL(update()),(QObject*)cam, SLOT(update()) );
    Translate(0.0,0.0,10.0);


    qDebug() << GLCam::shareWidget << cam;
}

/*!
property function for reading the near value
*/
double Item_cam::getNear(){
    return cam->near_plane;
}

/*!
property function for reading the near value
*/
double Item_cam::getFar(){
    return cam->far_plane;
}

/*!
porperty: distance to the near plane
*/
void Item_cam::Near(double val){
    cam->setNear(val);
}

/*!
porperty: distance to the far plane
*/
void Item_cam::Far(double val){
    cam->setFar(val);
}

/*!
modified deleteLater() function to protect undeletable cams
*/
void Item_cam::deleteLater(){
    //if(deletable)QObject::deleteLater();

    qDebug() << GLCam::shareWidget << cam;

    if(GLCam::shareWidget != cam)QObject::deleteLater();

}





