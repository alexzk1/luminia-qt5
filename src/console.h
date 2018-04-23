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

#ifndef CONSOLE_H
#define CONSOLE_H
#include <QtGui>

#include "sourceedit.h" //reuse the completationbox


class QGLWidget;
class QDockWidget;
class QTextEdit;
class MainWindow;
class QScriptEngine;
class QScriptValue;
class QScriptContext;
class ConsoleLine;




class ConsoleLine;
/*!
CompletionBox for the console
*/
class ConsoleCompletionBox : public AbstractCompletionBox{
Q_OBJECT
public:
	ConsoleCompletionBox (ConsoleLine *line, const QStringList& completions, const QString& searchstring);
	virtual ~ConsoleCompletionBox();
protected:
	void finishCompletion();
	ConsoleLine *line;
	};


class Console;
/*!
LineEdit with history and completion box
*/
class ConsoleLine : public QLineEdit{
Q_OBJECT
friend class ConsoleCompletionBox;
public:
	ConsoleLine(Console *parent):QLineEdit(){
		completationOpen=false;
		console = parent;
		meta = NULL;
		}
	~ConsoleLine(){}
	void keyPressEvent(QKeyEvent *e);
public slots:
	void helpHandler(const QString& string);
signals:
	void setHelpString(const QString& string);
protected:
	const QMetaObject *meta;
	Console *console;
	bool completationOpen;
	int count;
	QString temp;
	QStringList history;
	};




class Console : public QObject{
Q_OBJECT
friend class ConsoleLine;
public:
	Console(QObject *root);
	~Console();

public slots:
	void toggle(bool);

private slots:
	void returnPressed();

protected:
	QWidget *widget;
	QVBoxLayout *layout;
	QTextEdit *out;
	ConsoleLine *in;

	QObject *root;
	QDockWidget *dock;

	QScriptEngine *eng;
	//QScriptValue *global;
	QObject *world;

	QString code;
	bool active;
	};

#endif
