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


#include "../item.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <QDir>

void Item_node::importModel(const QString& filename){
    QString fn;
    if (filename.isEmpty()){
        fn = QFileDialog::getOpenFileName(NULL, tr("Open File"), "" , tr("Any Model (*.md2 *.MD2 *.md3 *.MD3 *.3ds *.3DS *.obj *.OBJ *.cr2 *.CR2 *.x *.X);;Quake2 Model (*.md2 *.MD2);;Quake3 Model (*.md3 *.MD3);;3DS Model (*.3ds *.3DS);;Obj Model (*.obj *.OBJ);;Poser Model (*.cr2 *.CR2);;DirectX Model (*.x *.X);;Cal3d binary Model (*.cmf *.CMF)"));
        //fn = QFileDialog::getOpenFileName(NULL, tr("Open File"), "" , tr("Any Model (*.md2 *.MD2 *.md3 *.MD3 *.3ds *.3DS *.obj *.OBJ *.x *.X *.cmf *.CMF);;Quake2 Model (*.md2 *.MD2);;Quake3 Model (*.md3 *.MD3);;3DS Model (*.3ds *.3DS);;Obj Model (*.obj *.OBJ);;DirectX Model (*.x *.X);;Cal3d binary Model (*.cmf *.CMF)"));
    }
    else{
        fn = filename;
        QString f_app = QFileInfo( QCoreApplication::arguments().at(0)).absolutePath() + "/" + filename;
        QString f_home = QDir::homePath() + "/.lumina/" + filename;
        if(QFileInfo (fn ).exists()){

        }
        else if(QFileInfo (f_app).exists()){
            fn = f_app;
        }
        else if(QFileInfo (f_home).exists()){
            fn = f_home;
        }



    }

    if (fn.endsWith(".md2", Qt::CaseInsensitive)){
        importMD2(fn);
    }

    else if (fn.endsWith(".md3", Qt::CaseInsensitive)){
        importMD3(fn);
    }

    else if (fn.endsWith(".obj", Qt::CaseInsensitive)){
        importOBJ(fn);
    }

    else if (fn.endsWith(".3ds", Qt::CaseInsensitive)){
        import3DS(fn);
    }

    else if (fn.endsWith(".x", Qt::CaseInsensitive)){
        importX(fn);
    }

    else if (fn.endsWith(".cmf", Qt::CaseInsensitive)){
        importCMF(fn);
    }

    else if (fn.endsWith(".cr2", Qt::CaseInsensitive)){
        importCR2(fn);
    }

}


