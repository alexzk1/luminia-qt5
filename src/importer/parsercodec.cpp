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
**********************************************************************************/

#include "parsercodec.h"

QString ParserCodec::convertToUnicode ( const char * chars, int len, ConverterState* /*state*/) const{
	QString result;
	for (int i = 0; i < len; i++) {
		switch(chars[i]){
			case '{': result +=  QString(" { "); break;
			case '}': result +=  QString(" } "); break;
			case '(': result +=  QString(" ( "); break;
			case ')': result +=  QString(" ) "); break;
			case ';': result +=  QString(" ; "); break;
			case ',': result +=  QString(" , "); break;
			case ':': result +=  QString(" : "); break;
				default: result += QLatin1Char(chars[i]);
			}
		}
	return result;
	}

QByteArray ParserCodec::convertFromUnicode(const QChar* c, int len, ConverterState*) const{
	return QString(len, *c).toAscii(); //dummy
	}

QByteArray ParserCodec::fromUnicode ( const QString & str ) const{
	return str.toAscii(); //dummy
	}

QByteArray ParserCodec::name() const{
	return QByteArray("ParserCodec");
	}

int ParserCodec::mibEnum() const{
	return 999999;
	}

