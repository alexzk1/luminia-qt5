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

#ifndef SCRIPT_LAUNCHER_H
#define SCRIPT_LAUNCHER_H

#include <QtGui>

class Item;
class QSInterpreter;
class QScriptEngine;
class glwrapper;

class ScriptLauncher : public QObject {
 Q_OBJECT
public:
	ScriptLauncher(QString fn, QObject * parent = 0);
	~ScriptLauncher();

	static Item *world;
	QAction *a;
	QRegExp *filter;

public slots: 
	void terminate();
private slots:
	void toggled(bool);
	void launch();

protected:
	bool toggle;
	QString fileName;

	glwrapper *ogl;
	QScriptEngine *ip;
	};
#endif
