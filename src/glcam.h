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
#ifndef GLCAM_H
#define GLCAM_H

#include <GL/glew.h>
#include <QtOpenGL>
#include <qlistview.h>
//#define VIEW_ORTHO 1
//#define VIEW_FRUSTUM 0

class GLCam : public QGLWidget{
Q_OBJECT
	friend class Item_cam;
public:
	GLCam(QWidget* parent,Item_cam* cam); //, const QGLWidget * shareWidget = NULL);
	~GLCam();
public slots:

protected:
	void resizeEvent(QResizeEvent* /* event */);
	void initializeGL();
	void paintGL();
	void mousePressEvent (QMouseEvent *ev);
	void mouseMoveEvent (QMouseEvent *ev);
	void mouseReleaseEvent (QMouseEvent *ev);
	void wheelEvent (QWheelEvent *ev);
	virtual void keyPressEvent(QKeyEvent *e);
public:
	void resizeGL( int w, int h );

	void setNear(double val);
	void setFar(double val);

public: 
	int view_projection_mode;

	static QGLWidget* shareWidget;

private:

	Item_cam* cam;
	QPoint startpoint;
	float zoom;
	GLuint object;

	GLfloat readpixel[4];

	double near_plane;
	double far_plane;
	};

#endif
