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

#ifndef SOURCEEDIT_H
#define SOURCEEDIT_H


#include <QtGui>

class SourceEdit;

class TextEdit : public QTextEdit{
friend class CompletionBox;
Q_OBJECT
public:
	TextEdit(QWidget *parent);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	SourceEdit* parent;
private:
	bool completationOpen;

	};

/*!
The Abstract Completation box implement common code for  Text and line edits
*/
class AbstractCompletionBox : public QFrame{
Q_OBJECT
public:
	AbstractCompletionBox(QWidget* parent, const QStringList& completions, const QString& searchstring);
	virtual ~AbstractCompletionBox(){}

public slots:
	void setHelpString(const QString&);
	
signals:
	void requestHelpString(const QString&);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

	virtual void finishCompletion() = 0;
	void populate();

	QListWidget *listwidget;
	QLabel *label;
	QVBoxLayout *layout;
	QWidget *parent;

	QStringList completions;
	QString searchString;
	};

/*!
completation box for a TextEdit
*/
class CompletionBox : public AbstractCompletionBox{
public:
	CompletionBox(TextEdit *editor, const QStringList& completions, const QString& searchstring);
	virtual ~CompletionBox();

protected:
	void finishCompletion();
	TextEdit *editor;
	};



/*!
Higlighter for ECMA script and GLSL
*/
class QTextEdit;
class Highlighter : public QSyntaxHighlighter{
public:
        Highlighter(QTextEdit *parent);
	~Highlighter();
 	virtual void highlightBlock( const QString &text);
	};




class LineNumberWidget : public QWidget{
	Q_OBJECT
public:
	LineNumberWidget(QTextEdit *editor, QWidget *parent = 0);
protected:
	virtual void paintEvent(QPaintEvent *);

private:
	QTextEdit *editor;
	};


class SourceEdit : public QWidget{
	friend class TextEdit;
	friend class CompletionBox;

	Q_OBJECT
public:
	SourceEdit(QWidget *parent = 0);
	~SourceEdit();
	void setText(const QString&);
	QString getText()const;
	void setCompleatationList(const QStringList&, int offset = 0);
	void setHelpString(const QString&);

	
protected:
	void emitRequestCompletationList(const QString&);


	QHBoxLayout *layout;
	TextEdit *edit;
	LineNumberWidget *linenumbers;
	Highlighter *highlighter;
	QStringList completationList;
	int completationOffset;
	
public slots:
	void emitRequestHelpString(const QString&);

signals:
	void requestCompletationList(const QString&);
	void requestHelpString(const QString&);
	void HelpStringSignal(const QString&);
	};



#endif
