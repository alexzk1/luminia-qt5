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
#include "sourceedit.h"

Item_shader::Item_shader( Item *parent, const QString& name, int _shadertype) : Item_edit( parent, name)
{
    shadertype = _shadertype;
    if (shadertype==Vertexshader){
        setIcon(0, QIcon(":/images/xpm/vertexshader.xpm"));

    }
    else {

        setIcon(0, QIcon(":/images/xpm/fragmentshader.xpm"));
    }

    connect(edit, SIGNAL(requestCompletationList(QString)), this, SLOT(completationHandler(QString)));
    connect(edit, SIGNAL(requestHelpString(QString)), this, SLOT(helpHandler(QString)));
    menuinit = false;
}

/*!
slot for opening the contextmenu
*/
void Item_shader::contextmenu(const QPoint& point){

    context = this;

    if(!menuinit){

        menu->addSeparator();
        DQMENU(Item_edit, menu);
        menu->addAction ( QIcon(":/images/xpm/load.xpm"), "Load file", this, SLOT(load()) );
        menu->addAction ( QIcon(":/images/xpm/save.xpm"), "Save as file", this, SLOT(saveas()) );
        menu->addAction ( QIcon(":/images/xpm/reload.xpm"), "Reload file", this, SLOT(reload()) );
        menu->addSeparator();

        SCRIPT2MENU();
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));
        menuinit = true;

    }

    menu->popup( point );
}


int Item_shader::getShaderType()
{
    return shadertype;
}

void Item_shader::completationHandler(const QString& line)
{
    edit->setCompleatationList(QStringList(),0);
    QString last = line.split(QRegExp("[^\\w\\._\\[\\]]")).last();

    if (last.length() < 2)return;

    if (last.left(3) == "gl_"){
        static QStringList glslvar = QStringList()<< "gl_FrontColor" << "gl_BackColor" << "gl_FrontSecondaryColor" << "gl_BackSecondaryColor" << "gl_TexCoord[" << "gl_FogFragCoord" << "gl_ModelViewMatrix" << "gl_ProjectionMatrix" << "gl_ModelViewProjectionMatrix" << "gl_TextureMatrix[" << "gl_NormalMatrix" << "gl_ModelViewMatrixInverse" << "gl_ProjectionMatrixInverse" << "gl_ModelViewProjectionMatrixInverse" << "gl_TextureMatrixInverse[" << "gl_ModelViewMatrixTranspose" << "gl_ProjectionMatrixTranspose" << "gl_ModelViewProjectionMatrixTranspose" << "gl_TextureMatrixTranspose[" << "gl_ModelViewMatrixInverseTranspose" << "gl_ProjectionMatrixInverseTranspose" << "gl_ModelViewProjectionMatrixInverseTranspose" << "gl_TextureMatrixInverseTranspose[" << "gl_NormalScale" << "gl_DepthRange.near" << "gl_DepthRange.far" << "gl_DepthRange.diff" << "gl_ClipPlane[" << "gl_Point.size" << "gl_Point.sizeMin" << "gl_Point.sizeMax" << "gl_Point.fadeThresholdSize" << "gl_Point.distanceConstantAttenuation" << "gl_Point.distanceLinearAttenuation" << "gl_Point.distanceQuadraticAttenuation" << "gl_FrontMaterial.emission" << "gl_FrontMaterial.ambient" << "gl_FrontMaterial.diffuse" << "gl_FrontMaterial.specular" << "gl_FrontMaterial.shininess" << "gl_BackMaterial.emission" << "gl_BackMaterial.ambient" << "gl_BackMaterial.diffuse" << "gl_BackMaterial.specular" << "gl_BackMaterial.shininess" << "gl_LightSource[].ambient" << "gl_LightSource[].diffuse" << "gl_LightSource[].specular" << "gl_LightSource[].position" << "gl_LightSource[].halfVector" << "gl_LightSource[].spotDirection" << "gl_LightSource[].spotExponent" << "gl_LightSource[].spotCutOff" << "gl_LightSource[].spotCosCutOff" << "gl_LightSource[].constantAttenuation" << "gl_LightSource[].linearAttenuation" << "gl_LightSource[].quadricAttenuation" << "gl_LightModel.ambient" << "gl_FrontLightModelProduct.sceneColor" << "gl_BackLightModelProduct.sceneColor" << "gl_FrontLightProduct[].ambient" << "gl_FrontLightProduct[].diffuse" << "gl_FrontLightProduct[].specular" << "gl_BackLightProduct[].ambient" << "gl_BackLightProduct[].diffuse" << "gl_BackLightProduct[].specular" << "gl_TextEnvColor[" << "gl_EyePlaneS[" << "gl_EyePlaneT[" << "gl_EyePlaneR[" << "gl_EyePlaneQ[" << "gl_ObjectPlaneS[" << "gl_ObjectPlaneT[" << "gl_ObjectPlaneR[" << "gl_ObjectPlaneQ[" << "gl_Fog.color" << "gl_Fog.density" << "gl_Fog.start" << "gl_Fog.end" << "gl_Fog.scale";

        edit->setCompleatationList(glslvar /*.filter(QRegExp(QString("^") + last))*/,last.length());
        return;
    }

    static QStringList glslfunctions = QStringList() << "radians" << "degrees" << "sin" << "cos" << "tan" << "asin" << "acos" << "atan" << "pow" << "exp" << "log" << "exp2" << "log2" << "sqrt" << "inversesqrt" << "abs" << "sign" << "floor" << "ceil" << "fract" << "mod" << "min" << "max" << "clamp" << "mix" << "step" << "smoothstep" << "length" << "distance" << "dot" << "cross" << "normalize" << "ftransform" << "faceforward" << "reflect" << "refract" << "matrixCompMult" << "outerProduct" << "transpose" << "lessThan" << "lessThanEqual" << "greaterThan" << "greaterThanEqual" << "equal" << "notEqual" << "any" << "all" << "not" << "texture1D" << "texture1DProj" << "texture1DLod" << "texture1DProjLod" << "texture2D" << "texture2DProj" << "texture2DLod" << "texture2DProjLod" << "texture3D" << "texture3DProj" << "texture3DLod" << "texture3DProjLod" << "textureCube" << "textureCubeLod" << "shadow1D" << "shadow2D" << "shadow1DProj" << "shadow2DProj" << "shadow1DLod" << "shadow2DLod" << "shadow1DProjLod" << "shadow2DProjLod" << "dFdx" << "dFdy" << "fwidth" << "noise1" << "noise2" << "noise3" << "noise4" ;

    if (glslfunctions.filter(line.right(2)).count() != 0){
        edit->setCompleatationList(glslfunctions.filter(QRegExp(QString("^") + last)),last.length());
        //qDebug() <<  glslfunctions.filter(QRegExp(QString("^") + last)); // << last.length();
        return;
    }
}

#include "glsl_reference.h"


void Item_shader::helpHandler(const QString& string)
{
    for (int i = 0; *(glsl_ref + i); i += 2)
    {
        if (string == glsl_ref[i])
        {
            edit->setHelpString(QString(glsl_ref[i+1]));
            break;
        }
    }
}

