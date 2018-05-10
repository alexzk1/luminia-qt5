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

#ifndef PROFILER_H
#define PROFILER_H

#include "incgl.h"
#include <QObject>
#include <atomic>

class QGLWidget;
class QDockWidget;
class QTextEdit;

class Profiler : public QObject
{
    Q_OBJECT
public:
    Profiler(QObject *parent);
    virtual ~Profiler()override;
    void newFrame();
    void start();
    void stop();
    void render(QGLWidget *w);

public slots:
    void toggle(bool);

protected:
    std::atomic<bool>     active;
    std::atomic<int64_t>  count;
    std::atomic<int64_t>  registered;

    GLuint* queries;

    QDockWidget *dock;
    QTextEdit *text;
    void freeQueries();
};

#endif
