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


#include <QDir>
#include <QFile>
#include <QObject>
#include <QDateTime>

namespace QS{

class File : public QObject{
	Q_OBJECT

	Q_PROPERTY(QString name READ fileName)
	Q_PROPERTY(QString path READ path)
	Q_PROPERTY(QString fullName READ absFileName)
	Q_PROPERTY(QString baseName READ baseName)
	Q_PROPERTY(QString extension READ extension)
	Q_PROPERTY(QString symLink READ symLink)
	
	Q_PROPERTY(bool exists READ exists)
	Q_PROPERTY(bool readable READ isReadable)
	Q_PROPERTY(bool writable READ isWritable)
	Q_PROPERTY(bool executable READ isExecutable)
	Q_PROPERTY(bool hidden READ isHidden)
	Q_PROPERTY(bool eof READ eof)
	
	Q_PROPERTY(QDateTime created READ created)
	Q_PROPERTY(QDateTime lastModified READ lastModified)
	Q_PROPERTY(QDateTime lastRead READ lastRead)
	
	Q_PROPERTY(int size READ size)
	
	Q_ENUMS(AccessMode)
public:
	enum AccessMode {
		ReadOnly  = QIODevice::ReadOnly,
		WriteOnly = QIODevice::WriteOnly,
		ReadWrite = QIODevice::ReadWrite,
		Append    = QIODevice::Append,
		Truncate  = QIODevice::Truncate,
		Translate = QIODevice::Text
	};

public slots:
	void open(int accessMode);
	void close() { file->close(); }
	void remove();
	
	int readByte();
	int readShort();
	int readInt();
	double readFloat();
	
	void writeShort(int in);
	void writeInt(int in);
	void writeFloat(double in);
	
	QString read();
	QString readLine();
	QStringList readLines();
	
	void writeByte(int byte);
	void write(const QString &data, int length = -1);
	void writeLine(const QString &data) { write(data + QString::fromLatin1("\n")); }

public:
	File(const QString &file);
	~File();
	
	QString fileName() const { return QFileInfo(*file).fileName(); }
	QString path() const { return QFileInfo(*file).absolutePath(); }
	QString absFileName() const { return QFileInfo(*file).absoluteFilePath(); }
	QString baseName() const { return QFileInfo(*file).baseName(); }
	QString extension() const { return QFileInfo(*file).completeSuffix(); }
	QString symLink() const { QFileInfo f(*file); return f.isSymLink() ? f.readLink() : QString(); }
	
	bool exists() const { return file->exists(); }
	bool isReadable() const { return QFileInfo(*file).isReadable(); }
	bool isWritable() const { return QFileInfo(*file).isWritable(); }
	bool isExecutable() const { return QFileInfo(*file).isExecutable(); }
	bool isHidden() const { return QFileInfo(*file).isHidden(); }
	bool eof() const { return file->atEnd(); }
	
	QDateTime created() const { return QFileInfo(*file).created(); }
	QDateTime lastModified() const { return QFileInfo(*file).lastModified(); }
	QDateTime lastRead() const { return QFileInfo(*file).lastRead(); }
	
	int size() const { return file->size(); }

private:
	QFile *file;
	};

} // namespace QS
