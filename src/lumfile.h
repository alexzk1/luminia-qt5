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

#ifndef LUMFILE_H
#define LUMFILE_H
#include <QXmlDefaultHandler>

class Item;

class Item_script;

class LumHandler : public QXmlDefaultHandler
{
public:
    LumHandler(Item *root, const QString & path = "");
    ~LumHandler() override = default;
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes) override;
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName) override;
    bool characters(const QString &str) override;
    bool fatalError(const QXmlParseException &exception)override;
    QString errorString() const override;

private:
    Item *createChildItem(const QString &tagName);
    Item *item;
    QString content;
    QString errorStr;
    QString path;

    bool luminaTag;
    QList<Item_script*> runlater;
};

class QDir;

class LumGenerator
{
public:
    LumGenerator(Item *rootitem);
    bool write(QIODevice *device);

private:
    QString escapedText(const QString &str);
    void processItem(Item *item, int depth );
    QString indent(int depth);
    Item *item;
    QTextStream out;
    QDir *relto;
};

#endif

