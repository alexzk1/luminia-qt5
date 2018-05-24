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

#include <QFrame>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>


/*!
The Abstract Completation box implement common code for  Text and line edits
*/
class AbstractCompletionBox : public QFrame
{
    Q_OBJECT
public:
    AbstractCompletionBox(QWidget* parent, const QStringList& completions, const QString& searchstring);
    ~AbstractCompletionBox() override = default;

public slots:
    void setHelpString(const QString&);

signals:
    void requestHelpString(const QString&);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    virtual void finishCompletion() = 0;
    void populate();

    QListWidget *listwidget;
    QLabel *label;
    QVBoxLayout *layout;
    QWidget *parent;

    QStringList completions;
    QString searchString;
};

#endif
