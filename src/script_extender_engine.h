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


#ifndef SCRIPT_EXTENDER_ENGINE
#define SCRIPT_EXTENDER_ENGINE


#include "factory/factory.h"
#include "glwrapper.h"

#include "script_extender.h"
#include "item.h"

#include <QtScript>

Q_SCRIPT_DECLARE_QMETAOBJECT(glwrapper, QObject*);



class SEngine: public QObject{
Q_OBJECT
public:
	
	SEngine(QObject *o, const QString& fn){
		obj = o;
		filename = fn;
		connect(obj,SIGNAL(destroyed()), this, SLOT(deleteLater()));

		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text )) {
			qDebug() << "Error launching script";
			return ;
			}
		QString script(file.readAll());



		QScriptValue obj_sv = eng.newQObject(obj);
		eng.globalObject().setProperty("obj" , obj_sv );


		ogl = new glwrapper(this,"gl");
		QScriptValue ogl_sv = eng.newQObject(ogl);
		ogl_sv.setPrototype(eng.scriptValueFromQMetaObject<glwrapper>());
		eng.globalObject().setProperty("gl" , ogl_sv );

		eng.globalObject().setProperty("World" ,eng.newQObject(Item::world));


		Factory::Factory(eng);

		QScriptValue r = eng.evaluate(script);
		if (eng.hasUncaughtException()) {
			int line = eng.uncaughtExceptionLineNumber();
			QMessageBox::critical ( 0, QString("Script error"), QString("Error processing Script %1 at Line %2 ").arg(filename).arg(line));
			}



		}

	~SEngine(){
		ScriptExtender::engineList.removeAll (this);
		qDebug() << "SEngine destroyed";
		delete ogl;
		}

	bool is(QObject *o, const QString& fn){
		return (o==obj && fn == filename);

		}

	QScriptEngine eng;

	QObject *obj; // assigned object
	QString filename; //script filename
	
	glwrapper *ogl;

	};




#endif