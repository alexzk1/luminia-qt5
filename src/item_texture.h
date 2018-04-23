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

#ifndef _ITEM_TEXTURE_H
#define _ITEM_TEXTURE_H

#include "item.h"

class half;

/*!
Texture class for the tree, supports DDS and QT suported image formats
*/

#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE GL_TEXTURE_RECTANGLE_EXT 
#endif
class Item_texture : public Item{
	Q_OBJECT
	Q_PROPERTY(int MagFilter READ getMagFilter WRITE MagFilter)
	Q_PROPERTY(int MinFilter READ getMinFilter WRITE MinFilter)
	Q_PROPERTY(int WrapS READ getWrapS WRITE WrapS)
	Q_PROPERTY(int WrapT READ getWrapT WRITE WrapT)
	Q_PROPERTY(bool CompareMode READ getCompareMode WRITE CompareMode)
	Q_PROPERTY(int Anisotropic READ getAnisotropic WRITE Anisotropic)

public:
	Item_texture( Item *parent , const QString& label1);
	virtual ~Item_texture();
	QString getFilename() const {return fn;}

	int getTextureID();

	bool isDepth();

	bool is3d() const {return (type==GL_TEXTURE_3D);} // really needed ?
	bool isCube() const {return (type==GL_TEXTURE_CUBE_MAP_EXT);}
	bool isRect() const {return (type==GL_TEXTURE_RECTANGLE);}
	bool is2d() const {return (type==GL_TEXTURE_2D);}
	bool is2dArray() const {return (type==GL_TEXTURE_2D_ARRAY_EXT);}


	int getTextureType() const {return type;} //used by framebuffer



	int getFormat();
	QString getFormatString()const;

	bool hasMipMap();
	
	virtual QString statusText() const;

	void setData(char *data, int datalen);
	QByteArray getData() const;

public slots: 
	int formatFromString(const QString&);

	void load();
	void load(const QString&);

	void reload();
	void Bind(int tmu = 0);
	void Unbind();

	void MinFilter(int);
	void MagFilter(int);

	void WrapS(int);
	void WrapT(int);
	void WrapR(int);

	void BindFBO(int slice = -1);
	void UnbindFBO();

//init a cleared texture
	void ImageCube(int,int format = -1, bool mipmap = false);
	void Image2d(int,int,int format = -1, bool mipmap = false);
	void Image2dArray(int,int,int,int format = -1, bool mipmap = false);
	void Image3d(int,int,int,int format = -1, bool mipmap = false);
	void ImageRect(int,int,int format = -1, bool mipmap = false);
	
	void ViewPort(float,float,float,float);

	double Width(){return width;}
	double Height(){return height;}
	double Depth(){return depth;}

	//void EnableCompareMode();
	//void DisableCompareMode();


	void GenerateMipmap();
	void Print(int x, int y, QString text); // Print text into a GL_ALPHA8 texture for displaying text wih a shader

	virtual QString getType() const{return QString("Texture");}
protected:
	QString fn;
	unsigned int texture, fbo;
	int width, height, depth;
	int mipmap, type;

	int boundto;
private:

	//property functions
	void CompareMode(bool);
	void Anisotropic(int);

	int getMagFilter(){return magFilter;}
	int getMinFilter(){return minFilter;}
	int getWrapS(){return wrapS;}
	int getWrapT(){return wrapT;}
	int getWrapR(){return wrapR;}
	
	int getAnisotropic(){return anisotropic;}
	bool getCompareMode(){return compareMode;}
	
	int minFilter,magFilter, anisotropic;
	int wrapS,wrapT,wrapR;

	bool compareMode;

	int formatindex; //index to the correct texture format structure
	int viewport[4];

public slots: 
	virtual void contextmenu(const QPoint&);
public:
	static void setup();
	static void create(QObject* obj, int id , void** args);
private: 
	bool menuinit;
	};

#endif
