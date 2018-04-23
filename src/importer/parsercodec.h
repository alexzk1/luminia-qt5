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
**********************************************************************************/
#include <QtCore>

#ifndef __PARSERCODEC_H
#define __PARSERCODEC_H

class ParserCodec : public QTextCodec{
protected:
	virtual QString convertToUnicode ( const char * chars, int len, ConverterState* state) const;
	virtual QByteArray convertFromUnicode(const QChar* c, int len, ConverterState*) const;
	virtual QByteArray fromUnicode ( const QString & str ) const;
	virtual QByteArray name() const;
	virtual int mibEnum() const;
	};

#endif
