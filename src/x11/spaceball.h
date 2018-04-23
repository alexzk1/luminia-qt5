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

#ifndef SPACEBALL_H
#define SPACEBALL_H

#include <QApplication>
#include <QtGui>
//class XDeviceInfo;
#include <X11/extensions/XInput.h>


class QXiApplication : public QApplication{
Q_OBJECT
public:
	QXiApplication(int & argc, char **argv);
	~QXiApplication();

protected:
	int motion_type;
	int button_press_type;
	int button_release_type;

	int last[6];

	//void Search_6dof();
	void Register_6dof(XDeviceInfo *info);
	virtual bool x11EventFilter ( XEvent *e );
	QList<XDevice*> devices; //list of open devices
signals:
	void spaceballEvent(int *);

	};

#endif
