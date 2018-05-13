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

//todo: replace all that by qscintila

#include "sourceedit.h"
#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QPainter>

TextEdit::TextEdit(QWidget *_parent):
    QTextEdit(_parent)
{
    parent = static_cast<SourceEdit*>(_parent);
    completationOpen = false;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize TextEdit::sizeHint() const
{
    return {800, 600};
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{

    QTextEdit::keyPressEvent(e);
    if (!completationOpen)
    {
        QTextCursor cursor = textCursor();
        QChar rchar = cursor.block().text()[cursor.position() - cursor.block().position()];
        if (!rchar.isLetterOrNumber())
        {

            //request a new completation list
            QString lstring = cursor.block().text().left(cursor.position() - cursor.block().position());
            parent->requestCompletationList(lstring);
            QString last = lstring.split(QRegExp("\\W")).last();
            //open the completationbox if the list has more than one entry
            if (parent->completationList.count() != 0 && !rchar.isLetterOrNumber() )
            {
                QWidget *box = new CompletionBox(this, parent->completationList, last.left(last.length() - parent->completationOffset - 1));
                box->move(mapToGlobal(cursorRect().bottomLeft()));
                box->show();
                connect(parent, SIGNAL(HelpStringSignal(const QString&)), box, SLOT(setHelpString(const QString&)));
                connect(box, SIGNAL(requestHelpString(const QString&)), parent, SLOT(emitRequestHelpString(const QString&)));
            }
        }
    }
}

/******************** compleationbox *******************************************/

AbstractCompletionBox::AbstractCompletionBox( QWidget *_parent, const QStringList& _completions, const QString& _searchString):
    QFrame(nullptr, Qt::Popup)
{
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

void AbstractCompletionBox::keyPressEvent(QKeyEvent *e)
{
    static bool r = true; //protection against recrusion
    switch (e->key())
    {
        case Qt::Key_Down:
        case Qt::Key_Up:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Tab:
            if (r)
            {
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
            if (e->key() == Qt::Key_Backspace)
            {
                if (searchString.isEmpty())
                    window()->close();
                searchString.chop(1);
            }
            else
                searchString.append(e->text());


            QApplication::sendEvent(parent, e);

            populate();

            break;
    }
}

void AbstractCompletionBox::setHelpString(const QString& txt)
{
    label->setText(txt);
    if (txt.length() == 0)
        label->hide();
    else
    {
        label->show();
        //setHeight (180 + label->height());
    }
}


void AbstractCompletionBox::populate()
{
    //qDebug() << "populate" << searchString;

    listwidget->clear();
    for (const auto &str : completions)
    {
        if (!searchString.isEmpty() && !str.startsWith(searchString))
            continue;
        auto item = new QListWidgetItem(str);
        item->setData(Qt::UserRole, str);
        listwidget->addItem(item);
    }

    if (listwidget->count() == 0 || (listwidget->count() == 1 && listwidget->item(0)->data(Qt::UserRole).toString() == searchString))
        window()->close();
}


/******************** compleationbox *******************************************/

CompletionBox::CompletionBox(TextEdit *_editor, const QStringList& _completions, const QString& _searchString):
    AbstractCompletionBox( _editor, _completions, _searchString)
{
    editor = _editor;
    editor->completationOpen = true;
}


CompletionBox::~CompletionBox()
{
    editor->completationOpen = false;
}

void CompletionBox::finishCompletion()
{
    QListWidgetItem *item = listwidget->currentItem();
    if (!item)
        return;
    QString s = item->data(Qt::UserRole).toString().mid(searchString.length());
    editor->insertPlainText(s);
}

/**************************** Line Number Widget ***************************************/

LineNumberWidget::LineNumberWidget(QTextEdit *_editor, QWidget *parent) : QWidget(parent)
{
    editor = _editor;
    setFixedWidth(fontMetrics().width(QLatin1String("000")));
    connect(editor->document()->documentLayout(), &QAbstractTextDocumentLayout::update, this, [this]()
    {
        update();
    });
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int)
    {
        update();
    });
}

void LineNumberWidget::paintEvent(QPaintEvent *)
{
    int contentsY = editor->verticalScrollBar()->value();
    qreal pageBottom = contentsY + editor->viewport()->height();
    int lineNumber = 1;
    const QFontMetrics fm = fontMetrics();
    const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1

    QPainter p(this);

    for (QTextBlock block = editor->document()->begin(); block.isValid(); block = block.next(), ++lineNumber)
    {
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

Highlighter::Highlighter(QTextEdit *parent):
    QSyntaxHighlighter(parent)
{
}

void Highlighter::highlightBlock( const QString &text)
{
    QTextCharFormat myClassFormat;

    const auto testAndApply = [&myClassFormat, &text, this](const HighlightRule & rule)
    {
        myClassFormat.setForeground(rule.foreground);
        myClassFormat.setFontWeight(rule.fontWeight);

        int pos = 0;
        while ((pos = rule.exp.indexIn(text, pos)) != -1)
        {
            int l = rule.exp.matchedLength();
            setFormat(pos, l, myClassFormat);
            pos += l;
        }
    };


    const static std::vector<HighlightRule> defaultRules =
    {
        {"([0-9]+\\.[0-9]+)", Qt::darkMagenta},

        {"([A-Za-z0-9]+)(?=\\s*\\()", Qt::blue},

        {
            "\\b(float|int|void|bool|true|false|mat2|mat3|mat4|mat2x2|mat3x2|mat4x2|mat2x3|mat3x3|mat4x3|"
            "mat2x4|mat3x4|mat4x4|vec2|vec3|vec4|bvec2|bvec3|bvec4|ivec2|ivec3|ivec4|sampler1D|sampler2D|"
            "sampler3D|samplerCube|sampler1DShadow|sampler2DShadow|attribute|const|uniform|varying|in|out|"
            "inout|input|output|hvec2|hvec3|vec4|dvec2|dvec3|dvec4|fvec2|fvec3|fvec4|sampler2DRect|sampler3DRect|"
            "sampler2dRectShadow|long|short|double|half|fixed|unsigned|lowp|mediump|highp|precision)\\b", Qt::darkRed
        },

        {"\"[.\n]*\"", QColor("#008000"),},

        {
            "(radians|degrees|sin|cos|tan|asin|acos|atan|pow|exp|log|exp2|log2|sqrt|inversesqrt|abs|sign|floor|"
            "ceil|fract|mod|min|max|clamp|mix|step|smoothstep|length|distance|dot|cross|normalize|ftransform|faceforward|"
            "reflect|refract|matrixCompMult|outerProduct|transpose|lessThan|lessThanEqual|greaterThan|greaterThanEqual|"
            "equal|notEqual|any|all|not|texture1D|texture1DProj|texture1DLod|texture1DProjLod|texture2D|texture2DProj|"
            "texture2DLod|texture2DProjLod|texture3D|texture3DProj|texture3DLod|texture3DProjLod|textureCube|"
            "textureCubeLod|shadow1D|shadow2D|shadow1DProj|shadow2DProj|shadow1DLod|shadow2DLod|shadow1DProjLod|"
            "shadow2DProjLod|dFdx|dFdy|fwidth|noise1|noise2|noise3|noise4)(?=\\s*\\()", Qt::blue, QFont::Bold
        },

        {"\\b(break|continue|do|for|while|if|else|discard|return|goto|switch|default|case|struct|asm|class|union|enum|var|typedef|template|this|packed|centroid)\\b", Qt::black, QFont::Bold},

        {"//.*", Qt::gray, 0},

        {"#.*", Qt::darkBlue},
    };
    for (const auto& r : defaultRules)
        testAndApply(r);
}

/*********************************************************************/
#include <QFontDatabase>
SourceEdit::SourceEdit(QWidget *parent):
    QWidget(parent)
{
    const static QStringList prefFontsFamilies =
    {
        "Source Code Pro",
        "Ubuntu Mono",
        "Courier",
    };
    static QFontDatabase db;
    static QStringList current = db.families();

    auto layout = new QHBoxLayout(this);
    setLayout(layout);
    buttonsBar = new QToolBar(this);
    buttonsBar->setOrientation(Qt::Vertical);

    edit =  new TextEdit(this);
    linenumbers = new LineNumberWidget(edit);
    highlighter = new Highlighter(edit);


    layout->setSpacing (0);
    layout->setMargin ( 0);
    layout->addWidget(linenumbers);
    layout->addWidget(edit);
    layout->addWidget(buttonsBar);


    for (const auto& fs : prefFontsFamilies)
    {
        if (current.contains(fs, Qt::CaseInsensitive))
        {
            QFont font;
            font.setFamily(fs);
            font.setFixedPitch(true);
            font.setPointSize(11);
            edit->setFont(font);
            break;
        }
    }

}

void SourceEdit::setText(const QString& t)
{
    edit->setText(t);
}

QString SourceEdit::getText()const
{
    return edit->toPlainText();
}

void SourceEdit::setCompleatationList( const QStringList &c, int _offset)
{
    completationList = c;
    completationOffset = _offset;
}

void SourceEdit::setHelpString(const QString &hs)
{
    //HelpString = hs;
    emit HelpStringSignal(hs);

}

void SourceEdit::appendActionToBar(QAction *act, QAction *before)
{
    if (before == nullptr)
    {
        if (act)
            buttonsBar->addAction(act);
        else
            buttonsBar->addSeparator();
    }
    else
    {
        if (act)
            buttonsBar->insertAction(before, act);
        else
            buttonsBar->insertSeparator(before);
    }
}

void SourceEdit::emitRequestCompletationList(const QString &s)
{
    emit requestCompletationList(s);
}

void SourceEdit::emitRequestHelpString(const QString &s)
{
    emit requestHelpString(s);
}
