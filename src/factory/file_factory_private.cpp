/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for 
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** This code based on Trolltechs QSA input dialog factory.
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

#include "file_factory_private.h"

#include <QtCore>

namespace QS{

File::File(const QString &fileName): file(new QFile(fileName)){
	qDebug() << "Filename:" << fileName;
	}

File::~File(){
	delete file;
	}

void File::open(int accessMode){
	if (accessMode == Append)
		accessMode |= WriteOnly;
	if (!file->open(QIODevice::OpenMode(accessMode))) {
		//interpreter->throwError(QString::fromLatin1("Could not open file '%1':  %2").arg(file->fileName()).arg(ERRORSTRING(file)));
		}
	}

QString File::read(){
	if (!file->isOpen()) {
		//interpreter->throwError(QString::fromLatin1("File '") + file->fileName() + QString::fromLatin1("' is not open"));
		return QString::null;
	}
	return QString::fromLocal8Bit(file->readAll());
	}

QString File::readLine(){
	return QString::fromLocal8Bit(file->readLine(16384));
	}


QStringList File::readLines(){
	return read().split(QString::fromLatin1("\n"));
	}

void File::write(const QString &data, int length){
	if (length<0)
		length = data.length();
	int written = file->write(data.toLocal8Bit());
	if (written != length) {
		//interpreter->throwError(QString::fromLatin1("Failed to write file '%1': %2").arg(file->fileName()).arg(ERRORSTRING(file)));
		}
	}

void File::remove(){
	if (!file->remove()) {
		//interpreter->throwError(QString::fromLatin1("Failed to remove file '%1': %2").arg(file->fileName()).arg(ERRORSTRING(file)));
		}
	}

int File::readByte(){
	char c;
	bool ok = file->getChar(&c);
	if (!ok) {
		//interpreter->throwError(QString::fromLatin1("Failed to read byte from file '%1': %2").arg(file->fileName()).arg(ERRORSTRING(file)));
		}
	return c;
	}

void File::writeByte(int byte){
	bool ok = file->putChar(char(byte));
	if (!ok) {
		//interpreter->throwError(QString::fromLatin1("Failed to write byte to file '%1': %2").arg(file->fileName()).arg(ERRORSTRING(file)));
		}
	}

int File::readShort(){
	short in;
	file->read((char*)&in,2);
	return in;
	}

int File::readInt(){
	int in;
	file->read((char*)&in,4);
	return in;
	}

double File::readFloat(){
	float in;
	file->read((char*)&in,4);
	return in;
	}

void File::writeShort(int in){
	short out = in;
	file->write((char*)&out,2);
	}

void File::writeInt(int in){
	file->write((char*)&in,4);
	}

void File::writeFloat(double in){
	float out = in;
	file->write((char*)&out,4);
	}

} // namespace QS
