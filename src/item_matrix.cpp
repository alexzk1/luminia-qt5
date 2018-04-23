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
#include <math.h>

Item_matrix::Item_matrix(Item *parent, const QString& name) : Item(parent, name){
	this->LoadIdenty();
	}
/*!
void LoadIdenty();\n
Replace objects matrix by identy matrix
*/
void Item_matrix::LoadIdenty(){
	matrix[ 0] = 1.0; matrix[ 1] = 0.0; matrix[ 2] = 0.0; matrix[ 3] = 0.0;
	matrix[ 4] = 0.0; matrix[ 5] = 1.0; matrix[ 6] = 0.0; matrix[ 7] = 0.0;
	matrix[ 8] = 0.0; matrix[ 9] = 0.0; matrix[10] = 1.0; matrix[11] = 0.0;
	matrix[12] = 0.0; matrix[13] = 0.0; matrix[14] = 0.0; matrix[15] = 1.0;
	}

void Item_matrix::MultMatrix(float *mat){
	float tmat[16];
	for (int i = 0; i < 4;i++){
		tmat[i+ 0]= matrix[i+0]*mat[ 0] + matrix[i+4]*mat[ 1] + matrix[i+8]*mat[ 2] + matrix[i+12]*mat[ 3];
		tmat[i+ 4]= matrix[i+0]*mat[ 4] + matrix[i+4]*mat[ 5] + matrix[i+8]*mat[ 6] + matrix[i+12]*mat[ 7];
		tmat[i+ 8]= matrix[i+0]*mat[ 8] + matrix[i+4]*mat[ 9] + matrix[i+8]*mat[10] + matrix[i+12]*mat[11];
		tmat[i+12]= matrix[i+0]*mat[12] + matrix[i+4]*mat[13] + matrix[i+8]*mat[14] + matrix[i+12]*mat[15];
		}
	for (int i = 0; i < 16;i++){
		matrix[i+0] = tmat[i+0];		
		}
	}

/*!
void Translate(number alpha, number x, number y, number z);\n
rotate object around axis vec(x,y,z) with alpha in in degrees
*/
void Item_matrix::Rotate(float _a, float _x, float _y, float _z){
	//qDebug() << "Todo: Item_matrix::Rotate()";

	float l = sqrt(_x*_x + _y*_y + _z*_z);
	if (l == 0.0)return;

	float x = _x/l;
	float y = _y/l;
	float z = _z/l;

	float a = _a /180.0 * 3.1415926535;
	float ca = cos(a);
	float sa = sin(a);

	float rmat[16];
	rmat[ 0] = ca + x*x*(1.0-ca);	rmat[ 1] = x*y*(1.0-ca)+z*sa;	rmat[ 2] = z*x*(1.0-ca)-y*sa;	rmat[ 3] = 0.0;
	rmat[ 4] = x*y*(1.0-ca)-z*sa;	rmat[ 5] = ca + y*y*(1.0-ca);	rmat[ 6] = z*y*(1.0-ca)+x*sa;	rmat[ 7] = 0.0;
	rmat[ 8] = x*z*(1.0-ca)+y*sa;	rmat[ 9] = y*z*(1.0-ca)-x*sa;	rmat[10] = ca + z*z*(1.0-ca);	rmat[11] = 0.0;
	rmat[12] = 0.0;			rmat[13] = 0.0;			rmat[14] = 0.0;			rmat[15] = 1.0;

	MultMatrix(rmat);

	}

/*!
void Translate(number x, number y, number z);\n
translate object by vec(x,y,z)
*/
void Item_matrix::Translate(float x, float y, float z){
	float tmat[16];
	tmat[ 0]=1.0;	tmat[ 1]=0.0;	tmat[ 2]=0.0;	tmat[ 3]=0.0;
	tmat[ 4]=0.0;	tmat[ 5]=1.0;	tmat[ 6]=0.0;	tmat[ 7]=0.0;
	tmat[ 8]=0.0;	tmat[ 9]=0.0;	tmat[10]=1.0;	tmat[11]=0.0;
	tmat[12] = x;	tmat[13] = y;	tmat[14] = z;	tmat[15]=1.0;
	MultMatrix(tmat);
	}
/*!
void Scale(number x, number y, number z);\n
scale object with vec(x,y,z)
*/
void Item_matrix::Scale(float x, float y, float z){
	float mat[16];
	mat[ 0] = x  ; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = 0.0;
	mat[ 4] = 0.0; mat[ 5] = y  ; mat[ 6] = 0.0; mat[ 7] = 0.0;
	mat[ 8] = 0.0; mat[ 9] = 0.0; mat[10] = z  ; mat[11] = 0.0;
	mat[12] = 0.0; mat[13] = 0.0; mat[14] = 0.0; mat[15] = 1.0;
	MultMatrix(mat);
	}

float* Item_matrix::getMatrix(){
	return matrix;
	}


void Item_matrix::getInverseMatrix(float *o){
	float *m = matrix;

	float det =	m[ 3] * m[ 6] * m[ 9] * m[12]-m[ 2] * m[ 7] * m[ 9] * m[12]-m[ 3] * m[ 5] * m[10] * m[12]+m[ 1] * m[ 7] * m[10] * m[12]+
			m[ 2] * m[ 5] * m[11] * m[12]-m[ 1] * m[ 6] * m[11] * m[12]-m[ 3] * m[ 6] * m[ 8] * m[13]+m[ 2] * m[ 7] * m[ 8] * m[13]+
			m[ 3] * m[ 4] * m[10] * m[13]-m[ 0] * m[ 7] * m[10] * m[13]-m[ 2] * m[ 4] * m[11] * m[13]+m[ 0] * m[ 6] * m[11] * m[13]+
			m[ 3] * m[ 5] * m[ 8] * m[14]-m[ 1] * m[ 7] * m[ 8] * m[14]-m[ 3] * m[ 4] * m[ 9] * m[14]+m[ 0] * m[ 7] * m[ 9] * m[14]+
			m[ 1] * m[ 4] * m[11] * m[14]-m[ 0] * m[ 5] * m[11] * m[14]-m[ 2] * m[ 5] * m[ 8] * m[15]+m[ 1] * m[ 6] * m[ 8] * m[15]+
			m[ 2] * m[ 4] * m[ 9] * m[15]-m[ 0] * m[ 6] * m[ 9] * m[15]-m[ 1] * m[ 4] * m[10] * m[15]+m[ 0] * m[ 5] * m[10] * m[15];

	det = 1.0 / det;
	
	o[ 0] = det * (m[ 6]*m[11]*m[13] - m[ 7]*m[10]*m[13] + m[ 7]*m[ 9]*m[14] - m[ 5]*m[11]*m[14] - m[ 6]*m[ 9]*m[15] + m[ 5]*m[10]*m[15]);
	o[ 1] = det * (m[ 3]*m[10]*m[13] - m[ 2]*m[11]*m[13] - m[ 3]*m[ 9]*m[14] + m[ 1]*m[11]*m[14] + m[ 2]*m[ 9]*m[15] - m[ 1]*m[10]*m[15]);
	o[ 2] = det * (m[ 2]*m[ 7]*m[13] - m[ 3]*m[ 6]*m[13] + m[ 3]*m[ 5]*m[14] - m[ 1]*m[ 7]*m[14] - m[ 2]*m[ 5]*m[15] + m[ 1]*m[ 6]*m[15]);
	o[ 3] = det * (m[ 3]*m[ 6]*m[ 9] - m[ 2]*m[ 7]*m[ 9] - m[ 3]*m[ 5]*m[10] + m[ 1]*m[ 7]*m[10] + m[ 2]*m[ 5]*m[11] - m[ 1]*m[ 6]*m[11]);
	o[ 4] = det * (m[ 7]*m[10]*m[12] - m[ 6]*m[11]*m[12] - m[ 7]*m[ 8]*m[14] + m[ 4]*m[11]*m[14] + m[ 6]*m[ 8]*m[15] - m[ 4]*m[10]*m[15]);
	o[ 5] = det * (m[ 2]*m[11]*m[12] - m[ 3]*m[10]*m[12] + m[ 3]*m[ 8]*m[14] - m[ 0]*m[11]*m[14] - m[ 2]*m[ 8]*m[15] + m[ 0]*m[10]*m[15]);
	o[ 6] = det * (m[ 3]*m[ 6]*m[12] - m[ 2]*m[ 7]*m[12] - m[ 3]*m[ 4]*m[14] + m[ 0]*m[ 7]*m[14] + m[ 2]*m[ 4]*m[15] - m[ 0]*m[ 6]*m[15]);
	o[ 7] = det * (m[ 2]*m[ 7]*m[ 8] - m[ 3]*m[ 6]*m[ 8] + m[ 3]*m[ 4]*m[10] - m[ 0]*m[ 7]*m[10] - m[ 2]*m[ 4]*m[11] + m[ 0]*m[ 6]*m[11]);
	o[ 8] = det * (m[ 5]*m[11]*m[12] - m[ 7]*m[ 9]*m[12] + m[ 7]*m[ 8]*m[13] - m[ 4]*m[11]*m[13] - m[ 5]*m[ 8]*m[15] + m[ 4]*m[ 9]*m[15]);
	o[ 9] = det * (m[ 3]*m[ 9]*m[12] - m[ 1]*m[11]*m[12] - m[ 3]*m[ 8]*m[13] + m[ 0]*m[11]*m[13] + m[ 1]*m[ 8]*m[15] - m[ 0]*m[ 9]*m[15]);
	o[10] = det * (m[ 1]*m[ 7]*m[12] - m[ 3]*m[ 5]*m[12] + m[ 3]*m[ 4]*m[13] - m[ 0]*m[ 7]*m[13] - m[ 1]*m[ 4]*m[15] + m[ 0]*m[ 5]*m[15]);
	o[11] = det * (m[ 3]*m[ 5]*m[ 8] - m[ 1]*m[ 7]*m[ 8] - m[ 3]*m[ 4]*m[ 9] + m[ 0]*m[ 7]*m[ 9] + m[ 1]*m[ 4]*m[11] - m[ 0]*m[ 5]*m[11]);
	o[12] = det * (m[ 6]*m[ 9]*m[12] - m[ 5]*m[10]*m[12] - m[ 6]*m[ 8]*m[13] + m[ 4]*m[10]*m[13] + m[ 5]*m[ 8]*m[14] - m[ 4]*m[ 9]*m[14]);
	o[13] = det * (m[ 1]*m[10]*m[12] - m[ 2]*m[ 9]*m[12] + m[ 2]*m[ 8]*m[13] - m[ 0]*m[10]*m[13] - m[ 1]*m[ 8]*m[14] + m[ 0]*m[ 9]*m[14]);
	o[14] = det * (m[ 2]*m[ 5]*m[12] - m[ 1]*m[ 6]*m[12] - m[ 2]*m[ 4]*m[13] + m[ 0]*m[ 6]*m[13] + m[ 1]*m[ 4]*m[14] - m[ 0]*m[ 5]*m[14]);
	o[15] = det * (m[ 1]*m[ 6]*m[ 8] - m[ 2]*m[ 5]*m[ 8] + m[ 2]*m[ 4]*m[ 9] - m[ 0]*m[ 6]*m[ 9] - m[ 1]*m[ 4]*m[10] + m[ 0]*m[ 5]*m[10]);
	}

