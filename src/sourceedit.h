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

#include <QTextEdit>
#include <QFrame>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSyntaxHighlighter>
#include <QToolBar>

class SourceEdit;

class TextEdit : public QTextEdit
{
    friend class CompletionBox;
    Q_OBJECT
public:
    TextEdit(QWidget *parent);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;
    SourceEdit* parent;
private:
    bool completationOpen;

};

/*!
The Abstract Completation box implement common code for  Text and line edits
*/
class AbstractCompletionBox : public QFrame
{
    Q_OBJECT
public:
    AbstractCompletionBox(QWidget* parent, const QStringList& completions, const QString& searchstring);
    virtual ~AbstractCompletionBox() override;

public slots:
    void setHelpString(const QString&);

signals:
    void requestHelpString(const QString&);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;
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
class CompletionBox : public AbstractCompletionBox
{
    Q_OBJECT
public:
    CompletionBox(TextEdit *editor, const QStringList& completions, const QString& searchstring);
    virtual ~CompletionBox() override;

protected:
    void finishCompletion() override;
    TextEdit *editor;
};



/*!
Higlighter for ECMA script and GLSL
*/
class QTextEdit;
class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextEdit *parent);
    virtual ~Highlighter() override;
    virtual void highlightBlock( const QString &text) override;
};




class LineNumberWidget : public QWidget
{
    Q_OBJECT
public:
    LineNumberWidget(QTextEdit *editor, QWidget *parent = nullptr);
protected:
    virtual void paintEvent(QPaintEvent *) override;

private:
    QTextEdit *editor;
};


class SourceEdit : public QWidget
{
    friend class TextEdit;
    friend class CompletionBox;
    Q_OBJECT
public:
    SourceEdit(QWidget *parent = nullptr);
    ~SourceEdit() override;
    void setText(const QString&);
    QString getText()const;
    void setCompleatationList(const QStringList&, int offset = 0);
    void setHelpString(const QString&);
    void appendActionToBar(QAction *act);
protected:
    void emitRequestCompletationList(const QString&);
    TextEdit *edit;
    QToolBar *buttonsBar;
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
