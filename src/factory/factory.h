/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** This code based on Trolltechs QSA input dialog factory.
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

#ifndef FACTORY_H
#define FACTORY_H



#ifndef QTSCRIPT
#include <qsobjectfactory.h>

class Factory : public QSObjectFactory{
public:
    Factory();
    QObject *create(const QString &className, const QVariantList &arguments, QObject *context);
    static const QMetaObject *metaObjectFrom(QString classname);
};
#endif

#ifdef QTSCRIPT
#include <QtCore>


struct QMetaObject;
class QScriptEngine;
namespace Factory
{
    void Factory(QScriptEngine &eng);
    const QMetaObject *metaObjectFrom(QString classname);
};
#endif

#endif
