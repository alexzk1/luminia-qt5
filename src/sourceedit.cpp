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
