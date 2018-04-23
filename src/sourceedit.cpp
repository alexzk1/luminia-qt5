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


#include "sourceedit.h"
#include <QtGui>

TextEdit::TextEdit(QWidget *_parent):QTextEdit(_parent){
	parent = static_cast<SourceEdit*>(_parent);
	completationOpen = false;
	}

void TextEdit::keyPressEvent(QKeyEvent *e){

	QTextEdit::keyPressEvent(e);
	if (completationOpen) return; // don't open a second completationbox

	QTextCursor cursor = textCursor();

	QChar rchar = cursor.block().text()[cursor.position() - cursor.block().position()];
	if (!rchar.isLetterOrNumber()){

		//request a new completation list
		QString lstring = cursor.block().text().left(cursor.position() - cursor.block().position());
		parent->requestCompletationList(lstring);
		QString last = lstring.split(QRegExp("\\W")).last();
		//open the completationbox if the list has more than one entry
		if(parent->completationList.count() != 0 && !rchar.isLetterOrNumber() ){;
			QWidget *box = new CompletionBox(this, parent->completationList, last.left(last.length() - parent->completationOffset -1));
			box->move(mapToGlobal(cursorRect().bottomLeft()));
			box->show();
			connect(parent,SIGNAL(HelpStringSignal(const QString&)),box,SLOT(setHelpString(const QString&)));
			connect(box,SIGNAL(requestHelpString(const QString&)), parent, SLOT(emitRequestHelpString(const QString&)));	

			}
		}
	}

/******************** compleationbox *******************************************/


AbstractCompletionBox::AbstractCompletionBox( QWidget *_parent, const QStringList& _completions, const QString& _searchString):QFrame(NULL, Qt::Popup){
	parent = _parent;
	
	completions = _completions;
	searchString = _searchString;

	listwidget = new QListWidget(this);
	listwidget->setFocusPolicy (Qt::NoFocus);
	//listwidget->setFixedHeight (180);
	label = new QLabel(this);
	label->setWordWrap(true);
	label->setFocusPolicy (Qt::NoFocus);
	label->hide();
	layout = new QVBoxLayout(this);
	
	layout->setMargin(0);
	layout->addWidget(listwidget);
	layout->addWidget(label);
	setLayout(layout);



	setAttribute(Qt::WA_DeleteOnClose);

	//qDebug() << searchString << completions;

	populate();
	}


void AbstractCompletionBox::keyPressEvent(QKeyEvent *e){
	static bool r = true; //protection against recrusion
	switch (e->key()) {
		case Qt::Key_Down:
		case Qt::Key_Up:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_Tab:
			if(r){
				r = false;
				QApplication::sendEvent(listwidget, e);
				r = true;
				}
			emit requestHelpString(listwidget->currentItem()->data(Qt::UserRole).toString());
			break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
			e->accept();
			finishCompletion();
			window()->close();
			break;
		case Qt::Key_Left:
		case Qt::Key_Right:
			window()->close();
			break;
		default:
			if (e->key() == Qt::Key_Backspace) {
				if (searchString.isEmpty())
					window()->close();
				searchString.chop(1);
				}
			else {
				searchString.append(e->text());
				}


			QApplication::sendEvent(parent, e);

			populate();

			break;
		}
	}

void AbstractCompletionBox::setHelpString(const QString& txt){
	label->setText(txt);
	if(txt.length() == 0)
		label->hide();
	else{
		label->show();
		//setHeight (180 + label->height());
		}
	}


void AbstractCompletionBox::populate(){
	//qDebug() << "populate" << searchString;

	listwidget->clear();
	foreach (QString str, completions) {
		if (!searchString.isEmpty() && !str.startsWith(searchString))continue;
		QListWidgetItem *item = new QListWidgetItem(str);
		item->setData(Qt::UserRole, str);
		listwidget->addItem(item);
		}

	if (listwidget->count() == 0 || (listwidget->count() == 1 && listwidget->item(0)->data(Qt::UserRole).toString() == searchString)){
		window()->close();
		}
	}


/******************** compleationbox *******************************************/

CompletionBox::CompletionBox(TextEdit *_editor, const QStringList& _completions, const QString& _searchString) :	AbstractCompletionBox( _editor, _completions, _searchString){
	editor = _editor;
	editor->completationOpen = true;
	}


CompletionBox::~CompletionBox(){
	editor->completationOpen = false;
	}

void CompletionBox::finishCompletion(){
	QListWidgetItem *item = listwidget->currentItem();
	if (!item)
		return;
	QString s = item->data(Qt::UserRole).toString().mid(searchString.length());
	editor->insertPlainText(s);
	}

/**************************** Line Number Widget ***************************************/

LineNumberWidget::LineNumberWidget(QTextEdit *_editor, QWidget *parent) : QWidget(parent){
	editor = _editor;
	setFixedWidth(fontMetrics().width(QLatin1String("000")));
	connect(editor->document()->documentLayout(), SIGNAL(update(const QRectF &)), this, SLOT(update()));
	connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
	}

void LineNumberWidget::paintEvent(QPaintEvent *){
	int contentsY = editor->verticalScrollBar()->value();
	qreal pageBottom = contentsY + editor->viewport()->height();
	int lineNumber = 1;
	const QFontMetrics fm = fontMetrics();
	const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1

	QPainter p(this);

	for (QTextBlock block = editor->document()->begin();block.isValid();block = block.next(), ++lineNumber) {
		QTextLayout *layout = block.layout();

		const QRectF boundingRect = layout->boundingRect();
		QPointF position = layout->position();
		if (position.y() + boundingRect.height() < contentsY)
			continue;
		if (position.y() > pageBottom)
			break;

		const QString txt = QString::number(lineNumber);
		p.drawText(width() - fm.width(txt), qRound(position.y()) - contentsY + ascent, txt);
		}
	}

Highlighter::Highlighter(QTextEdit *parent) : QSyntaxHighlighter(parent){
	}

Highlighter::~Highlighter(){
	}

void Highlighter::highlightBlock( const QString &text){

 	QTextCharFormat myClassFormat;
	int index,length;

	static QRegExp floats("([0-9]+\\.[0-9]+)");
	myClassFormat.setForeground(Qt::darkMagenta);
	index = text.indexOf(floats);
	while (index >= 0) {
		length = floats.matchedLength();
		setFormat(index, length , myClassFormat);
		index = text.indexOf(floats, index + length);
		}

	static QRegExp function("([A-Za-z0-9]+)\\s*\\(");
	myClassFormat.setForeground(Qt::blue);
	index = text.indexOf(function);
	while (index >= 0) {
		length = function.matchedLength();
		setFormat(index, length - 1, myClassFormat);
		index = text.indexOf(function, index + length);
		}

	static QRegExp glsltypes("\\b(float|int|void|bool|true|false|mat2|mat3|mat4|mat2x2|mat3x2|mat4x2|mat2x3|mat3x3|mat4x3|mat2x4|mat3x4|mat4x4|vec2|vec3|vec4|bvec2|bvec3|bvec4|ivec2|ivec3|ivec4|sampler1D|sampler2D|sampler3D|samplerCube|sampler1DShadow|sampler2DShadow|attribute|const|uniform|varying|in|out|inout|input|output|hvec2|hvec3|vec4|dvec2|dvec3|dvec4|fvec2|fvec3|fvec4|sampler2DRect|sampler3DRect|sampler2dRectShadow|long|short|double|half|fixed|unsigned|lowp|mediump|highp|precision)\\b");

  //   myClassFormat.setFontWeight(QFont::Bold);
	myClassFormat.setForeground(Qt::darkRed);
	index = text.indexOf(glsltypes);
	while (index >= 0) {
		length = glsltypes.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(glsltypes, index + length);
		}

	static QRegExp string("\"[.\n]*\"");
	index = text.indexOf(string);
	while (index >= 0) {
		length = string.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(string, index + length);
		}

	myClassFormat.setFontWeight(QFont::Bold);

	static QRegExp buildin("(radians|degrees|sin|cos|tan|asin|acos|atan|pow|exp|log|exp2|log2|sqrt|inversesqrt|abs|sign|floor|ceil|fract|mod|min|max|clamp|mix|step|smoothstep|length|distance|dot|cross|normalize|ftransform|faceforward|reflect|refract|matrixCompMult|outerProduct|transpose|lessThan|lessThanEqual|greaterThan|greaterThanEqual|equal|notEqual|any|all|not|texture1D|texture1DProj|texture1DLod|texture1DProjLod|texture2D|texture2DProj|texture2DLod|texture2DProjLod|texture3D|texture3DProj|texture3DLod|texture3DProjLod|textureCube|textureCubeLod|shadow1D|shadow2D|shadow1DProj|shadow2DProj|shadow1DLod|shadow2DLod|shadow1DProjLod|shadow2DProjLod|dFdx|dFdy|fwidth|noise1|noise2|noise3|noise4)\\s*\\(");

	index = text.indexOf(buildin);
	myClassFormat.setForeground(Qt::blue);
	while (index >= 0) {
		length = buildin.matchedLength();
		setFormat(index, length - 1, myClassFormat);
		index = text.indexOf(buildin, index + length);
		}

	static QRegExp glslkeywords("\\b(break|continue|do|for|while|if|else|discard|return|goto|switch|default|case|struct|asm|class|union|enum|typedef|template|this|packed|centroid)\\b");


	myClassFormat.setForeground(Qt::black);
  
	index = text.indexOf(glslkeywords);
	while (index >= 0) {
		length = glslkeywords.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(glslkeywords, index + length);
		}


	static QRegExp comment("//.*");
	myClassFormat.setFontWeight(0);
	myClassFormat.setForeground(Qt::gray);
  
	index = text.indexOf(comment);
	while (index >= 0) {
		length = comment.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(comment, index + length);
		}

	static QRegExp pre("#.*");
	myClassFormat.setFontWeight(0);
	myClassFormat.setForeground(Qt::darkGreen);
  
	index = text.indexOf(comment);
	while (index >= 0) {
		length = pre.matchedLength();
		setFormat(index, length, myClassFormat);
		index = text.indexOf(pre, index + length);
		}
        }

/*********************************************************************/

SourceEdit::SourceEdit(QWidget *parent):QWidget(parent){
	layout = new QHBoxLayout(this);
	layout->setSpacing (0);
	layout->setMargin ( 0);
	edit =  new TextEdit(this);

	linenumbers = new LineNumberWidget(edit);
	highlighter = new Highlighter(edit);

	layout->addWidget(linenumbers);
	layout->addWidget(edit);

	setLayout(layout);

	QFont font;
        font.setFamily("Courier");
        font.setFixedPitch(true);
        font.setPointSize(10);
	edit->setFont(font);
	}


SourceEdit::~SourceEdit(){
	}

void SourceEdit::setText(const QString& t){
	edit->setText(t);
	}

QString SourceEdit::getText()const{
	return edit->toPlainText();
	}

void SourceEdit::setCompleatationList( const QStringList &c, int _offset){
	completationList = c;
	completationOffset = _offset;
	}

void SourceEdit::setHelpString(const QString &hs){
	//HelpString = hs;
	emit HelpStringSignal(hs);

	}

void SourceEdit::emitRequestCompletationList(const QString &s){
	emit requestCompletationList(s);
	}

void SourceEdit::emitRequestHelpString(const QString &s){
	emit requestHelpString(s);
	}
