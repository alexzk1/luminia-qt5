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

#include <QtScript>
#include <QFile>
#include <QPointer>

#include "no_copy.h"

class Item;
class glwrapper;

class SEngine: public QObject, public utility::NoCopyAssignMove
{
    Q_OBJECT
public:
    SEngine(QObject *o);
    SEngine(QObject *o, QString  fileName);
    ~SEngine() override;
    bool equals(const SEngine *c) const;

    QScriptValue run();
    QScriptValue run(const QString& src);
    QScriptValue run(QFile &file);
    QScriptValue execJsFunc(const QString& function, const QVariantList& args);

    void bindItem(const QPointer<Item>& itm, bool localy = false);
    QScriptEngine& getEngine();
    QPointer<glwrapper> getGl() const;
signals:
    void scriptError(const QString& err, const QStringList& trace) const;
private:
    QScriptEngine eng;
    QPointer<Item> obj; // assigned object
    QPointer<glwrapper> ogl;
    QString filename; //script filename

    bool testErrors() const;
    void setupEngine(QObject *o);
};
#endif
