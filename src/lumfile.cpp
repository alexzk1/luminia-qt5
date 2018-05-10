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


#include "lumfile.h"
#include "item.h"
#include "loaderpaths.h"

#include "item_image.h"
#include "item_virtual.h"
#include "item_component.h"
#include "item_mesh.h"
#include "item_index.h"
#include "item_texture.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "texture2lum.h"

LumHandler::LumHandler(Item *root, const QString & _path): QXmlDefaultHandler ()
{
    item = root;
    luminaTag = false;
    path = _path;
}

LumHandler::~LumHandler()
{
}

bool LumHandler::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes &attributes)
{

    if (!luminaTag && qName != "lumina")
    {
        errorStr = QObject::tr("The file is not an Lumina file.");
        return false;
    }

    if (qName == "lumina")
    {
        QString version = attributes.value("version");
        if (!version.isEmpty() && (version.toDouble() > 0.301))
        {
            errorStr = QObject::tr("This file is written by a newer lumina version");
            return false;
        }

        luminaTag = true;
    }
    else
        if (qName == "node")
            item = new Item_node(item, attributes.value("name"));
        else
            if (qName == "text")
                item = new Item_edit(item, attributes.value("name"));
            else
                if (qName == "script")
                {
                    item = new Item_script(item, attributes.value("name"));
                    if (attributes.value("run") == "true")
                        runlater.append(static_cast<Item_script*>(item));
                    qDebug() << runlater;
                }
                else
                    if (qName == "shader")
                    {
                        int shadertype = 0;
                        if (attributes.value("type") == "vertex")shadertype = Item_shader::Vertexshader;
                        else
                            if (attributes.value("type") == "geometry")shadertype = Item_shader::Geometryshader;
                            else
                                if (attributes.value("type") == "fragment")shadertype = Item_shader::Fragmentshader;
                        item = new Item_shader(item, attributes.value("name"), shadertype);
                    }
                    else
                        if (qName == "texture")
                        {
                            Item_texture* t = new Item_texture(item, attributes.value("name"));
                            QString fn = LoaderPaths::findObject(attributes.value("filename"), {path});
                            if (!fn.isEmpty())
                            {
                                qDebug() << "loading" <<  fn;
                                t->load(fn);
                            }
                            else
                            {
                                int format = t->formatFromString(attributes.value("format"));
                                if (format != 0)
                                {
                                    bool mipmap = attributes.value("mipmap") == "true";
                                    qDebug () << mipmap;

                                    int w = attributes.value("width").toInt();
                                    if (attributes.value("cubemap") == "true")
                                        t->ImageCube(w, format, mipmap);
                                    else
                                    {
                                        int h = attributes.value("height").toInt();
                                        int d = attributes.value("depth").toInt();
                                        if (d < 2 || attributes.value("depth").isEmpty())t->Image2d(w, h, format, mipmap);
                                        else t->Image3d(w, h, d, format, mipmap);
                                    }
                                }
                            }
                            item = t;
                        }
                        else
                            if (qName == "buffer")
                            {
                                auto dim = attributes.value("dim").toUInt();
                                auto size =  attributes.value("size").toUInt();
                                auto keyframes =  attributes.value("keyframes").toUInt();
                                QString format = attributes.value("format");
                                int type;
                                if  (format == "byte")  type = GL_BYTE;
                                else
                                    if (format == "ubyte") type = GL_UNSIGNED_BYTE;
                                    else
                                        if (format == "short") type = GL_SHORT;
                                        else
                                            if (format == "ushort")    type = GL_UNSIGNED_SHORT;
                                            else
                                                if (format == "int")   type = GL_INT;
                                                else
                                                    if (format == "uint")  type = GL_UNSIGNED_INT;
                                                    else
                                                        if (format == "half")  type = GL_HALF_FLOAT_ARB;
                                                        else
                                                            if (format == "float") type = GL_FLOAT;
                                                            else
                                                            {
                                                                type = GL_FLOAT;
                                                                qDebug() << "illegal format:" << format;
                                                            }

                                Item_buffer *b = new Item_buffer(item, attributes.value("name"), dim, size, keyframes, type);

                                item = b;
                            }
                            else
                                if (qName == "uniform")
                                {
                                    auto dim = attributes.value("dim").toUInt();
                                    auto size =  attributes.value("size").toUInt();
                                    auto keyframes =  attributes.value("keyframes").toUInt();
                                    QString format = attributes.value("format");
                                    int type;

                                    if  (format == "int")   type = GL_INT;
                                    else
                                        if (format == "float") type = GL_FLOAT;
                                        else
                                        {
                                            type = GL_FLOAT;
                                            qDebug() << "illegal format:" << format;
                                        }

                                    Item_uniform *b = new Item_uniform(item, attributes.value("name"), dim, size, keyframes, type);

                                    item = b;
                                }
                                else
                                    if (qName == "mesh" || qName == "stream")  //stream is deprecated and replaced by mesh
                                    {
                                        Item_mesh *mesh = new Item_mesh(item, attributes.value("name"), attributes.value("vertices").toInt());

                                        item = mesh;
                                    }
                                    else
                                        if (qName == "component")
                                        {
                                            int type = 0;
                                            QString typen = attributes.value("type");
                                            if (typen == "vertex") type = Item_mesh::VERTEX;
                                            else
                                                if (typen == "vector")type = Item_mesh::VECTOR;
                                                else
                                                    if (typen == "color")type = Item_mesh::COLOR;
                                                    else
                                                        if (typen == "uvcoords")type = Item_mesh::UVCOORDS;
                                                        else
                                                            if (typen == "bonedep")type = Item_mesh::BONEDEP;
                                                            else
                                                                if (typen == "quaternion")type = Item_mesh::QUATERNION;
                                                                else type = Item_mesh::GENERIC;

                                            int dim = attributes.value("dim").toInt();

                                            int keyframes = 1;
                                            if (attributes.index("keyframes") != -1)
                                                keyframes = attributes.value("keyframes").toInt();

                                            QString format = attributes.value("format");
                                            int formatn;
                                            if  (format == "byte")  formatn = GL_BYTE;
                                            else
                                                if (format == "ubyte") formatn = GL_UNSIGNED_BYTE;
                                                else
                                                    if (format == "short") formatn = GL_SHORT;
                                                    else
                                                        if (format == "ushort")    formatn = GL_UNSIGNED_SHORT;
                                                        else
                                                            if (format == "int")   formatn = GL_INT;
                                                            else
                                                                if (format == "uint")  formatn = GL_UNSIGNED_INT;
                                                                else
                                                                    if (format == "float") formatn = GL_FLOAT;
                                                                    else
                                                                        if (format == "half")  formatn = GL_HALF_FLOAT_ARB;
                                                                        else
                                                                        {
                                                                            type = GL_FLOAT;
                                                                            qDebug() << "illegal format:" << format;
                                                                            formatn = GL_FLOAT;
                                                                        }

                                            item = new Item_component(static_cast<Item_mesh*>(item), attributes.value("name"), type, dim, keyframes, formatn);
                                        }

                                        else
                                            if (qName == "index" )

                                                item = static_cast<Item*>(static_cast<Item_mesh*>(item)->addIndex(attributes.value("name"), attributes.value("primitive").toInt()));
                                            else
                                                if (qName == "armature")
                                                {
                                                    Item_armature *s = new Item_armature(item, attributes.value("name"));
                                                    item = s;
                                                }

                                                else
                                                    if (qName == "bone")
                                                    {
                                                        Item_bone *s  = new Item_bone(item, attributes.value("name"), static_cast<Item_bone*>(item)->getArmature(), attributes.value("id").toInt());

                                                        s->setJoint(attributes.value("jointx").toFloat(), attributes.value("jointy").toFloat(),  attributes.value("jointz").toFloat());
                                                        item = s;

                                                    }

    content.clear();
    return true;
}

bool LumHandler::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString &qName)
{
    if (qName == "lumina")
    {
        for (int i = 0; i < runlater.size(); ++i)
            runlater.at(i)->run();
    }
    else
    {
        if (content.isEmpty() || qName == "node" || qName == "mesh" || qName == "stream" || qName == "bone" || qName == "armature" )
            item = item->parent();
        else
            if (qName == "text" || qName == "shader" || qName == "script")
            {
                static_cast<Item_edit*>(item)->setText(content);
                item = item->parent();
            }
            else
                if (qName == "texture")
                {
                    auto d(utility::tex_decode(content.toStdString()));
                    if (d.size())
                        static_cast<Item_texture*>(item)->setData(d.data(), d.size());
                    item = item->parent();
                }
                else
                    if (qName == "component")
                    {
                        static_cast<Item_component*>(item)->setData(content.trimmed());
                        item = item->parent();
                    }
                    else
                        if (qName == "buffer")
                        {
                            static_cast<Item_buffer*>(item)->setData(content.trimmed());
                            item = item->parent();
                        }
                        else
                            if (qName == "uniform")
                            {
                                static_cast<Item_uniform*>(item)->setData(content.trimmed());
                                item = item->parent();
                            }
                            else
                                if (qName == "index")
                                {
                                    static_cast<Item_index*>(item)->setData(content.trimmed());
                                    item = item->parent();
                                }
    }
    item->world->setTime(0);

    return true;
}

bool LumHandler::characters(const QString &str)
{
    content += str;
    return true;
}


bool LumHandler::fatalError (const QXmlParseException & exception)
{
    qDebug() << "Fatal error on line" << exception.lineNumber()
             << ", column" << exception.columnNumber() << ":"
             << exception.message();

    return false;
}

QString LumHandler::errorString() const
{
    return errorStr;
}

/********************************************************************************************/

bool LumGenerator::write(QIODevice *device)
{
    QFile *f;

    if ((f = dynamic_cast<QFile*>(device)))
    {
        relto = new QDir(QFileInfo(f->fileName()).absolutePath());
        qDebug() << "Saving relative to: " << relto;
    }
    else
        relto = nullptr;


    out.setDevice(device);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE lumina>\n"
        << "<lumina version=\"0.3\">\n";

    processItem(item, 1);

    out << "</lumina>\n";

    if (relto)delete relto;

    return true;
}




QString LumGenerator::indent(int depth)
{
    return QString(4 * depth, ' ');
}

void LumGenerator::processItem(Item *item, int depth)
{
    qDebug() << item->objectName();

    QString type = item->getType();
    if (type == "World")
    {
        for (int i = 0; i < item->childCount(); ++i)
            processItem(static_cast<Item*>(item->child(i)), depth + 0);
    }
    else
        if (type == "Node")
        {
            out << indent(depth) << "<node name=\"" << item->objectName() << "\">\n";
            for (int i = 0; i < item->childCount(); ++i)
                processItem(static_cast<Item*>(item->child(i)), depth + 1);

            out << indent(depth) << "</node>\n";
        }
        else
            if (type == "Text")
            {
                out << indent(depth) << "<text name=\"" << item->objectName() << "\">";
                out << escapedText(static_cast<Item_edit*>(item)->raw_text());
                out << "</text>\n";
            }
            else
                if (type == "Script")
                {
                    if (!dynamic_cast<Item_script*>(item)->isRunning())
                        out << indent(depth) << "<script name=\"" << item->objectName() << "\">";
                    else
                        out << indent(depth) << "<script name=\"" << item->objectName() << "\" run=\"true\">";


                    out << escapedText(static_cast<Item_edit*>(item)->raw_text());
                    out << "</script>\n";
                }
                else
                    if (type == "Shader")
                    {
                        QString stype;
                        switch (static_cast<Item_shader*>(item)->getShaderType())
                        {
                            case Item_shader::Vertexshader:
                                stype = "vertex" ;
                                break;
                            case Item_shader::Geometryshader:
                                stype = "geometrie";
                                break;
                            case Item_shader::Fragmentshader:
                                stype = "fragment";
                                break;
                        }

                        out << indent(depth) << "<shader name=\"" << item->objectName() << "\" type=\"" << stype << "\">";
                        out << escapedText(static_cast<Item_edit*>(item)->raw_text());
                        out << "</shader>\n";
                    }
                    else
                        if (type == "Texture")
                        {
                            Item_texture *T = static_cast<Item_texture*>(item);
                            out << indent(depth) << "<texture name=\"" << item->objectName() << "\" ";

                            if (T->getFilename() != "")
                            {
                                QString filename;
                                if (relto)
                                    out << "filename=\"" << relto->relativeFilePath(T->getFilename()) << "\" >\n";
                                else
                                    out << "filename=\"" << T->getFilename() << "\" >\n";
                            }

                            else
                            {


                                out << "format=\"" << T->getFormatString() << "\" ";
                                //qDebug() << "format=\"" << T->getFormatString() << "\" " ;

                                if (T->hasMipMap()) out << "mipmap=\"true\" ";
                                if (T->isCube()) out << "cubemap=\"true\" ";

                                out << "width=\"" << T->Width() << "\" ";

                                if (T->Height() > 1)out << "height=\"" << T->Height() << "\" ";
                                if (T->Depth() > 1)out << "depth=\"" << T->Depth() << "\" ";
                                out << ">\n";


                                qDebug() << "Saving texture WxHxD: " << T->Width() << T->Height() << T->Depth();
                                const auto tmp(utility::tex_lz4_encode_base64(T->getData()));
                                const static size_t step = 8192;
                                for (size_t pos = 0, sz = tmp.size(); pos < sz; pos += step)
                                {
                                    std::string str(tmp.data() + pos, std::min<size_t>(step, sz - pos));
                                    out << indent(depth + 1) << str.c_str() << "\n";
                                }
                                // */
                            }

                            out << indent(depth) << "</texture>\n";
                        }
                        else
                            if (type == "Mesh")  //Stream is deprecated and replaces by Mesh
                            {
                                int num = static_cast<Item_mesh*>(item)->getNumOfVertices();
                                out << indent(depth) << "<mesh name=\"" << item->objectName() << "\" vertices=\"" << num << "\">\n";

                                for (int i = 0; i < item->childCount(); ++i)
                                    processItem(static_cast<Item*>(item->child(i)), depth + 1);

                                out << indent(depth) << "</mesh>\n";
                            }
                            else
                                if (type == "Index")
                                {
                                    Item_index* in = static_cast<Item_index*>(item);
                                    out << indent(depth) << "<index name=\"" << item->objectName() << "\" primitive=\"" << in->getIPP() << "\">\n";
                                    out << in->getData();
                                    out << indent(depth) << "</index>\n";
                                }
                                else
                                    if (type == "Component")
                                    {
                                        Item_component* co = static_cast<Item_component*>(item);
                                        QString comptype;
                                        switch (co->getCompType())
                                        {
                                            case Item_component::VERTEX:
                                                comptype = "vertex";
                                                break;
                                            case Item_component::GENERIC:
                                                comptype = "generic";
                                                break;
                                            case Item_component::VECTOR:
                                                comptype = "vector";
                                                break;
                                            case Item_component::COLOR:
                                                comptype = "color";
                                                break;
                                            case Item_component::UVCOORDS:
                                                comptype = "uvcoords";
                                                break;
                                            case Item_component::BONEDEP:
                                                comptype = "bonedep";
                                                break;
                                            case Item_component::QUATERNION:
                                                comptype = "quaternion";
                                                break;
                                        }
                                        out << indent(depth) << "<component name=\"" << item->objectName() << "\"";
                                        out << " type=\"" << comptype << "\"";
                                        out << " dim=\"" << co->getDim() << "\"";
                                        out << " keyframes=\"" << co->getKeyFrames() << "\"";

                                        switch (co->getFormat())
                                        {
                                            case GL_BYTE:
                                                out << " format=\"byte\" >\n";
                                                break;
                                            case GL_UNSIGNED_BYTE:
                                                out << " format=\"ubyte\" >\n";
                                                break;
                                            case GL_SHORT:
                                                out << " format=\"short\" >\n";
                                                break;
                                            case GL_UNSIGNED_SHORT:
                                                out << " format=\"ushort\" >\n";
                                                break;
                                            case GL_INT:
                                                out << " format=\"int\" >\n";
                                                break;
                                            case GL_UNSIGNED_INT:
                                                out << " format=\"uint\" >\n";
                                                break;
                                            case GL_HALF_FLOAT_ARB:
                                                out << " format=\"half\" >\n";
                                                break;
                                            case GL_FLOAT:
                                            default:
                                                out << " format=\"float\" >\n";
                                        }

                                        out << co->getData();
                                        out << indent(depth) << "</component>\n";
                                    }
                                    else
                                        if (type == "Buffer")
                                        {
                                            Item_buffer* buf = static_cast<Item_buffer*>(item);
                                            out << indent(depth) << "<buffer name=\"" << item->objectName() << "\" size=\"" << buf->getSize();
                                            out << "\" dim=\"" << buf->getDim() << "\" keyframes=\"" << buf->getKeyFrames() << "\"";

                                            switch (buf->getFormat())
                                            {
                                                case GL_BYTE:
                                                    out << " format=\"byte\" >\n";
                                                    break;
                                                case GL_UNSIGNED_BYTE:
                                                    out << " format=\"ubyte\" >\n";
                                                    break;
                                                case GL_SHORT:
                                                    out << " format=\"short\" >\n";
                                                    break;
                                                case GL_UNSIGNED_SHORT:
                                                    out << " format=\"ushort\" >\n";
                                                    break;
                                                case GL_INT:
                                                    out << " format=\"int\" >\n";
                                                    break;
                                                case GL_UNSIGNED_INT:
                                                    out << " format=\"uint\" >\n";
                                                    break;
                                                case GL_HALF_FLOAT_ARB:
                                                    out << " format=\"half\" >\n";
                                                    break;
                                                case GL_FLOAT:
                                                default:
                                                    out << " format=\"float\" >\n";
                                            }

                                            out << buf->getData();
                                            out << indent(depth) << "</buffer>\n";
                                        }
                                        else
                                            if (type == "Uniform")
                                            {
                                                Item_uniform* buf = static_cast<Item_uniform*>(item);
                                                out << indent(depth) << "<uniform name=\"" << item->objectName() << "\" size=\"" << buf->getSize();
                                                out << "\" dim=\"" << buf->getDim() << "\" keyframes=\"" << buf->getKeyFrames() << "\"";

                                                switch (buf->getFormat())
                                                {
                                                    case GL_INT:
                                                        out << " format=\"int\" >\n";
                                                        break;
                                                    case GL_FLOAT:
                                                    default:
                                                        out << " format=\"float\" >\n";
                                                }

                                                out << buf->getData();
                                                out << indent(depth) << "</uniform>\n";
                                            }
                                            else
                                                if (type == "Armature")
                                                {
                                                    out << indent(depth) << "<armature name=\"" << item->objectName() << "\">\n";
                                                    for (int i = 0; i < item->childCount(); ++i)
                                                        processItem(static_cast<Item*>(item->child(i)), depth + 1);
                                                    out << indent(depth) << "</armature>\n";
                                                }
                                                else
                                                    if (type == "Bone")
                                                    {
                                                        float *joint = static_cast<Item_bone*>(item)->getJoint();
                                                        auto id = static_cast<Item_bone*>(item)->getId();
                                                        out << indent(depth) << "<bone name=\"" << item->objectName() << "\" id=\"" << id << "\" jointx=\"" << joint[0] << "\" jointy=\"" << joint[1] << "\" jointz=\"" << joint[2] << "\">\n";
                                                        for (int i = 0; i < item->childCount(); ++i)
                                                            processItem(static_cast<Item*>(item->child(i)), depth + 1);
                                                        out << indent(depth) << "</bone>\n";
                                                    }
}

QString LumGenerator::escapedText(const QString &str)
{
    QString result = str;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    return result;
}
