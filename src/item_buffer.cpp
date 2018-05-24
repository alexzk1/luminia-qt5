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
#include "item.h"

#include "half.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include "globals.h"

QList<int> Item_buffer::boundedAttributeLocations = QList<int>();
QList<Item_buffer*> Item_buffer::boundedTransformFeedback = QList<Item_buffer*>();


Item_buffer::Item_buffer(Item *parent, const QString& name, unsigned _dim, unsigned _size, unsigned _keyframes, int _format, bool _normalized_int) : Item( parent, name)
{
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
    setIcon( 0, QIcon(":/images/xpm/buffer.xpm") );

    glGenBuffers(1, (GLuint*)&glbuf);

    size = 0;
    dim = 0;
    keyframes = 0;
    setDim( _dim, _size, _keyframes, _format, _normalized_int);
    ref_pos = -1;

    transformFeedbackName = "";
    transformFeedbackKeyFrame = 0;
}

Item_buffer::~Item_buffer()
{
    glDeleteBuffers(1, &glbuf);
}

void Item_buffer::addMenu(QMenu *menu)
{
    menu->addAction( tr( "Properties"), this, SLOT(PropertiesDialog()) );
}

/*!
void setDim(number dimension, number size, number keyframes, enum format, bool normalized)\n
Set the uniform buffer to dimension with size keyframes and format;
*/
void Item_buffer::setDim(unsigned _dim, unsigned _size, unsigned _keyframes, unsigned _format, bool _normalized_int)
{
    GL_CHECK_ERROR();
    unsigned tmp_size = _size < 1 ? 1 : _size ;
    unsigned tmp_keyframes = _keyframes < 1 ? 1 : _keyframes;
    unsigned tmp_dim =  _dim  < 1 ? 1 : _dim;
    if (tmp_dim > 4)
    {
        tmp_dim = 4;
        qDebug() << "Item_buffer::setDim() dim is limited to 4 !";
    }

    //auto max_elements = tmp_dim * tmp_keyframes * tmp_size;

    glBindBuffer( GL_ARRAY_BUFFER, glbuf);

    unsigned bytes;
    switch (format)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            bytes = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT_ARB:
            bytes = 2;
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        default:
            bytes = 4;
    }
    unsigned  elements = tmp_dim * tmp_keyframes * tmp_size;

#define COPY_DATA(dest, scale)                                      \
    if (buf.b){                                         \
    for (unsigned k = 0; k < (tmp_keyframes < keyframes ? tmp_keyframes : keyframes);k++){  \
    for (unsigned j = 0 ; j < (tmp_size < size ? tmp_size : size); j++){        \
    for (unsigned i = 0; i < (tmp_dim < dim ? tmp_dim : dim); i++){     \
    double x = operator()(i,j,k);                   \
    dest [i + tmp_dim * j + tmp_dim * tmp_size * k] = x * scale;    \
}                               \
}                                   \
}                                       \
}

    tmp_buf.b = nullptr;

    double tmp_normalized_scalefactor = 1.0;
    if ((size * dim * keyframes) != 0)
    {

        switch (_format)
        {
            case GL_BYTE:
                tmp_buf.b = new char[elements];
                memset(tmp_buf.b, 0, elements);
                tmp_normalized_scalefactor = _normalized_int ?  127.0 : 1.0;
                COPY_DATA(tmp_buf.b, tmp_normalized_scalefactor)
                format = GL_BYTE;
                break;
            case GL_UNSIGNED_BYTE:
                tmp_buf.ub = new unsigned char[elements];
                memset(tmp_buf.ub, 0, elements);
                tmp_normalized_scalefactor = _normalized_int ? 255.0 : 1.0;
                COPY_DATA(tmp_buf.ub, tmp_normalized_scalefactor)
                format = GL_UNSIGNED_BYTE;
                break;
            case GL_SHORT:
                tmp_buf.s = new short[elements];
                memset(tmp_buf.s, 0, elements * 2);
                tmp_normalized_scalefactor = _normalized_int ? 32767.0 : 1.0;
                COPY_DATA(tmp_buf.s, tmp_normalized_scalefactor)
                format = GL_SHORT;
                break;
            case GL_UNSIGNED_SHORT:
                tmp_buf.us = new unsigned short[elements];
                memset(tmp_buf.us, 0, elements * 2);
                tmp_normalized_scalefactor = _normalized_int ? 65535.0 : 1.0;
                COPY_DATA(tmp_buf.us, tmp_normalized_scalefactor)
                format = GL_UNSIGNED_SHORT;
                break;
            case GL_INT:
                tmp_buf.i = new int[elements];
                memset(tmp_buf.i, 0, elements * 4);
                tmp_normalized_scalefactor = _normalized_int ? 2147483648.0 : 1.0;
                COPY_DATA(tmp_buf.i, tmp_normalized_scalefactor);
                format = GL_INT;
                break;
            case GL_UNSIGNED_INT:
                tmp_buf.ui = new unsigned int[elements];
                memset(tmp_buf.ui, 0, elements * 4);
                tmp_normalized_scalefactor = _normalized_int ? 4294967296.0 : 1.0;
                COPY_DATA(tmp_buf.ui, tmp_normalized_scalefactor)
                format = GL_UNSIGNED_INT;
                break;
            case GL_HALF_FLOAT_ARB:
                tmp_buf.h = new half[elements];
                memset(tmp_buf.h, 0, elements * 2);
                tmp_normalized_scalefactor = 1.0;
                COPY_DATA(tmp_buf.h, tmp_normalized_scalefactor);
                format = GL_HALF_FLOAT_ARB;
                break;
            default:
                tmp_buf.f = new float[elements];
                memset(tmp_buf.f, 0, elements * 4);
                tmp_normalized_scalefactor = 1.0;
                COPY_DATA(tmp_buf.f, 1.0);
                format =  GL_FLOAT;
        }


        glBufferData(GL_ARRAY_BUFFER, elements * bytes, tmp_buf.b, GL_DYNAMIC_COPY );
        if (tmp_buf.b)
            delete[]tmp_buf.b;

    }
    else
    {
        //zero buffer for initialization
        glBufferData(GL_ARRAY_BUFFER, elements * bytes, nullptr, GL_DYNAMIC_COPY );
    }
    GL_CHECK_ERROR();
    bound_by_GPU = false;
    is_mapped = false;

    size = tmp_size;
    dim = tmp_dim;
    keyframes = tmp_keyframes;
    format = _format;
    normalized_scalefactor = 1.0 / tmp_normalized_scalefactor;
    normalized_int = _normalized_int;

    ref_pos = -1;
    ref_buf_old = 0.0;
    ref_buf = 0.0;
#undef COPY_DATA
}

/*!
returns some statur informations about the object
*/
QString Item_buffer::statusText() const
{
    QString k("");
    if (keyframes > 1)
        k = QString (tr("%3 Keyframes with")).arg(keyframes);

    QString n("");
    if (normalized_int)
        n = QString(tr("normalized "));

    switch (format)
    {
        case GL_BYTE:
            return QString(tr("%5; %1 %2 %3Byte%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_UNSIGNED_BYTE:
            return QString(tr("%5; %1 %2 %3Unsigned Byte%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_SHORT:
            return QString(tr("%5; %1 %2 %3Short%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_UNSIGNED_SHORT:
            return QString(tr("%5; %1 %2 %3Unsigned Short%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_INT:
            return QString(tr("%5; %1 %2 %3Int%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_UNSIGNED_INT:
            return QString(tr("%5; %1 %2 %3Unsigned Int%4")).arg(k).arg(size).arg(n).arg(dim).arg(Item::statusText());
        case GL_HALF_FLOAT_ARB:
            return QString(tr("%5; %1 %2 Half Float%3")).arg(k).arg(size).arg(dim).arg(Item::statusText());
        default:
            return QString(tr("%5; %1 %2 Float%3")).arg(k).arg(size).arg(dim).arg(Item::statusText());
    }
}
#include <QGLContext>
/*!
Operator to acces the content with a up to 3 dimensional index.
*/
double& Item_buffer::operator()( unsigned _dim, unsigned _index, unsigned _keyframe)
{
    /*if(bound_by_GPU){
        qDebug() << "Item_buffer::operator(): VBO is bound to GPU, can't read/write!";
        //return ref_buf;
        }*/
    GL_CHECK_ERROR();
    if (!is_mapped)
    {
        buf.v = nullptr;
        glBindBuffer( GL_ARRAY_BUFFER, glbuf);
        GL_CHECK_ERROR();
        buf.v = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        //qDebug() << "Item_buffer::operator()" << buf.v;
        is_mapped = true;
    }
    GL_CHECK_ERROR();
    if (!buf.v)
    {
        qDebug() << "Item_buffer::operator()" << buf.v << is_mapped << "buf.v == NULL !";
        qDebug() << glIsBuffer(glbuf);

        qDebug() << QGLContext::currentContext ();
        is_mapped = false;

        return ref_buf;
    }

    unsigned element = _dim + _index * dim + _keyframe * dim * size;
    //qDebug() << "Item_buffer::operator()" <<  _dim  << _index << _keyframe << element ;

    if (element > dim * size * keyframes)
    {
        qDebug() << objectName() << " Item_buffer::operator(): Index out of range";
        return ref_buf;
    }

    //write value from ref_buf back, ad replace it
    switch (format)
    {
        case GL_BYTE:
            if (ref_pos >= 0)
                buf.b[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.b[element] * normalized_scalefactor;
            break;
        case GL_UNSIGNED_BYTE:
            if (ref_pos >= 0)
                buf.ub[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.ub[element] * normalized_scalefactor;
            break;
        case GL_SHORT:
            if (ref_pos >= 0)
                buf.s[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.s[element] * normalized_scalefactor;
            break;
        case GL_UNSIGNED_SHORT:
            if (ref_pos >= 0)
                buf.us[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.us[element] * normalized_scalefactor;
            break;
        case GL_INT:
            if (ref_pos >= 0)
                buf.i[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.i[element] * normalized_scalefactor;
            break;
        case GL_UNSIGNED_INT:
            if (ref_pos >= 0)
                buf.ui[ref_pos] = ref_buf / normalized_scalefactor;
            ref_buf = buf.ui[element] * normalized_scalefactor;
            break;
        case GL_HALF_FLOAT_ARB:
            if (ref_pos >= 0)
                buf.h[ref_pos] = ref_buf;
            ref_buf = buf.h[element] * normalized_scalefactor;
            break;
        default:
            if (ref_pos >= 0)buf.f[ref_pos] = ref_buf;
            ref_buf = buf.f[element] * normalized_scalefactor;
    }
    ref_buf_old = ref_buf;
    ref_pos = element;
    need_refresh = true;
    return ref_buf;
}

/*!
internal function.
*/
void Item_buffer::refresh()
{
    if (ref_buf_old == ref_buf) return; //skip if not written...

    GL_CHECK_ERROR();
    if (!is_mapped)
    {
        buf.v = nullptr;
        glBindBuffer( GL_ARRAY_BUFFER, glbuf);
        buf.v = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        qDebug() << "Item_buffer::refresh()" << buf.v;
        is_mapped = true;
    }
    GL_CHECK_ERROR();
    if (!buf.v)
    {


        qDebug() << glIsBuffer(glbuf) << glbuf;

        qDebug() << QGLContext::currentContext ();
        is_mapped = false;


        qDebug() << "Item_buffer::refresh()" << buf.v << is_mapped << "buf.v == NULL !";
        return;
    }

    if (need_refresh && (ref_pos >= 0))
    {
        switch (format)
        {
            case GL_BYTE:
                buf.b[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_UNSIGNED_BYTE:
                buf.ub[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_SHORT:
                buf.s[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_UNSIGNED_SHORT:
                buf.s[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_INT:
                buf.i[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_UNSIGNED_INT:
                buf.ui[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            case GL_HALF_FLOAT_ARB:
                buf.h[ref_pos] = ref_buf / normalized_scalefactor;
                break;
            default:
                buf.f[ref_pos] = ref_buf / normalized_scalefactor;
        }
    }
    need_refresh = false;
    ref_buf_old = ref_buf;
}



/*!
void set(int index, number x[, number y=0[, number z=0[, number w=0]]])\n
Set the element index.
*/
void Item_buffer::set(int index, double x, double y, double z, double w)
{
    setInKeyFrame(0, index, x, y, z, w);
}



void Item_buffer::setInKeyFrame(int key, int index, double x, double y, double z, double w)
{
    switch (dim)
    {
        case 4:
            (*this)(3, index, key) = w;
            [[clang::fallthrough]];
        case 3:
            (*this)(2, index, key) = z;
            [[clang::fallthrough]];
        case 2:
            (*this)(1, index, key) = y;
            [[clang::fallthrough]];
        default:
            (*this)(0, index, key) = x;
    }
}

/*!
void set(int index, Array od numbers)\n
Set the element at index.
*/

void Item_buffer::set(int index, const QList<double>& l)
{
    setInKeyFrame(0, index, l);
}




void Item_buffer::setInKeyFrame(int key, int index, const QList<double>& l)
{
    if ( (unsigned)index > size) return;
    if ( l.size() != (int)dim )
        qDebug() << "Item_buffer::set() list size don't match  dim";
    for (unsigned i = 0 ; i < dim; i++)
        (*this)(i, index, key) = l.at(i);
}


/*!
internal function
*/
void Item_buffer::setData(const QString& content)
{
    qDebug() << "Item_buffer::setData()";
    QStringList list = content.split(QRegExp("\\s+"));

    unsigned n = dim * size * keyframes;
    if (n > (unsigned)list.count())
    {
        qDebug() <<  "Item_buffer::setData(): not enough data";
        return;
    };

    double tmp = normalized_scalefactor;
    normalized_scalefactor = 1.0; //don't normalize data in files, to save space for integer formats
    n = 0;
    for (unsigned k = 0; k < keyframes ; k++)
    {
        for (unsigned j = 0 ; j < size ; j++)
        {
            for (unsigned i = 0; i < dim; i++)
            {
                (*this)(i, j, k) = list.at(n).toFloat();
                n++;
            }
        }
    }
    normalized_scalefactor = tmp;
}

/*!
internal function
*/
QString Item_buffer::getData()
{
    refresh();
    QString data;
    QString num;

    double tmp = normalized_scalefactor;
    normalized_scalefactor = 1.0; //don't normalize data in files, to save space for integer formats

    for (unsigned k = 0; k < keyframes ; k++)
    {
        for (unsigned j = 0 ; j < size ; j++)
        {
            for (unsigned i = 0; i < dim; i++)
            {
                double x = (*this)(i, j, k);
                num.setNum(x);
                data.append(num).append(" ");
            }
            data.append("\n");
        }
    }
    normalized_scalefactor = tmp;

    return data;
}

/*!
internal function \n
returns the dimension i.E. 1,2,3,4,9,16
*/
unsigned Item_buffer::getDim()
{
    return dim;
}

/*!
internal function \n
returns the size (number of vertices)
*/
unsigned Item_buffer::getSize()
{
    return size;
}

/*!
internal function \n
returns the number of keyframes
*/
unsigned Item_buffer::getKeyFrames()
{
    return keyframes;
}

/*!
internal function \n
returns the Opengl format. i.E. GL_FLOAT, GL_BYTE
*/
unsigned Item_buffer::getFormat()
{
    return format;
}

/*!
internal function \n
returns if  a integer format is normalized
*/
bool Item_buffer::isNormalizedInt()
{
    return normalized_int;
}

QString Item_buffer::getType() const
{
    return "Buffer";
}

#include "glwrapper.h"




/*!
TextureBind(int tmu) \n
experimentel non tested function to bind the buffer content as "Buffer Texture"
*/
void Item_buffer::TextureBind(unsigned tmu)
{
    glActiveTexture(tmu);
    glBindBuffer(GL_TEXTURE_BUFFER_EXT, glbuf);
    bool tmp_refresh = need_refresh;
    refresh(); //
    unsigned bytes;
    if (tmp_refresh)
    {
        switch (format)
        {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
                bytes = 1;
                break;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_HALF_FLOAT_ARB:
                bytes = 2;
                break;
            case GL_INT:
            case GL_UNSIGNED_INT:
            default:
                bytes = 4;
        }
        glBufferDataARB( GL_ARRAY_BUFFER, size * dim * keyframes * bytes, buf.f, GL_DYNAMIC_DRAW );
    }

    glBindTexture(GL_TEXTURE_BUFFER_EXT, gltexture);
    switch (dim)
    {
        case 4:
            glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGBA32F_ARB, glbuf);
            break;
        case 3:
            glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_RGB32F_ARB, glbuf);
            break;
        case 2:
            glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_LUMINANCE_ALPHA32F_ARB, glbuf);
            break;
        case 1:
            glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, GL_LUMINANCE32F_ARB, glbuf);
            break;
    }
}


void Item_buffer::Bind(int location)
{
    BindKeyFrame(0, location);
}

#define NULL_CHAR static_cast<char*>(nullptr)
void Item_buffer::BindKeyFrame(unsigned key, int location)
{
    if (location == -1 ) return;
    refresh();

    glBindBuffer( GL_ARRAY_BUFFER, glbuf);

    //check if mapped !!
    if (is_mapped)
    {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        is_mapped = false;
    }

    unsigned bytes;
    switch (format)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            bytes = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT_ARB:
            bytes = 2;
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        default:
            bytes = 4;
    }

    size_t ofs = bytes * size * dim * (key > keyframes ? (keyframes - 1) : key);

    switch (location)
    {
        case -2:
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(dim, format, 0, NULL_CHAR + ofs);
            break;
        case -3:
            if (dim != 3) break;
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer( format, 0, NULL_CHAR + ofs);
            break;
        case -4:
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(dim, format, 0, NULL_CHAR + ofs);
            break;
        case -5:
            glEnableClientState(GL_TEXTURE_COORD_ARRAY );
            glTexCoordPointer(dim, format, 0, NULL_CHAR + ofs);
            break;
        default:
            glEnableVertexAttribArray(location);
            unsigned norm = GL_FALSE;
            if (normalized_int) norm = GL_TRUE;

            glVertexAttribPointer(location, dim, format, norm, 0, NULL_CHAR + ofs);
    }
    boundedAttributeLocations << location;
    bound_by_GPU = true;
}
#undef NULL_CHAR

/*!
void UnbindAll()\n
This functions disables all ClientState and VertexAttribArrays. Is called after excution of a render() script function.
*/
void Item_buffer::UnbindAll()
{
    for (int boundedAttributeLocation : boundedAttributeLocations)
        Unbind(boundedAttributeLocation);
    boundedAttributeLocations.clear();
    boundedTransformFeedback.clear();
}

/*!
void Unbind(int location)\n
unbind a ClientState or VertexAttribArray location.
*/
void Item_buffer::Unbind(int location)
{
    switch (location)
    {
        case -2:
            glDisableClientState(GL_VERTEX_ARRAY);
            break;
        case -3:
            glDisableClientState(GL_NORMAL_ARRAY);
            break;
        case -4:
            glDisableClientState(GL_COLOR_ARRAY);
            break;
        case -5:
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            break;
        default:
            glDisableVertexAttribArray(location);
    }
    //bound_by_GPU = false;
}


/*!
Opens the properties dialog
*/
void Item_buffer::PropertiesDialog()
{

    QDialog d;
    QVBoxLayout vboxlayout;
    d.setLayout(&vboxlayout);

    QWidget gridwidget;
    vboxlayout.addWidget(&gridwidget);
    QGridLayout grid( &gridwidget);

    int row = 0;

    QLabel l_dim("Dim:", nullptr);
    grid.addWidget(&l_dim, row, 0);
    QSpinBox s_dim(nullptr);
    grid.addWidget(&s_dim, row, 1);
    s_dim.setValue(getDim());
    s_dim.setRange(1, 4);
    row++;

    QLabel l_size("Size:", nullptr);
    QSpinBox s_size(nullptr);
    s_size.setRange(1, 1 << 24);
    s_size.setValue(getSize());
    if (getType() != "Component")
    {
        grid.addWidget(&l_size, row, 0);
        grid.addWidget(&s_size, row, 1);
        row++;
    }

    QLabel l_key("Keyframes:", nullptr);
    grid.addWidget(&l_key, row, 0);
    QSpinBox s_key(nullptr);
    grid.addWidget(&s_key, row, 1);
    s_key.setRange(1, 10000);
    s_key.setValue(getKeyFrames());
    row++;


    QLabel l_format("Format:");
    grid.addWidget(&l_format, row, 0);
    QComboBox c_format(nullptr);
    grid.addWidget(&c_format, row, 1);
    c_format.addItems(QStringList() << "Float" << "Byte" << "Unsigned Byte" << "Short" << "Unsigned Short" << "Int" << "Unsigned Int" << "Half Float");
    switch (format)
    {
        case GL_FLOAT:
            c_format.setCurrentIndex(0);
            break;
        case GL_BYTE:
            c_format.setCurrentIndex(1);
            break;
        case GL_UNSIGNED_BYTE:
            c_format.setCurrentIndex(2);
            break;
        case GL_SHORT:
            c_format.setCurrentIndex(3);
            break;
        case GL_UNSIGNED_SHORT:
            c_format.setCurrentIndex(4);
            break;
        case GL_INT:
            c_format.setCurrentIndex(5);
            break;
        case GL_UNSIGNED_INT:
            c_format.setCurrentIndex(6);
            break;
        case GL_HALF_FLOAT_ARB:
            c_format.setCurrentIndex(7);
            break;
        default:
            c_format.setCurrentIndex(0);
            break;
    }
    row++;

    QLabel l_normalized("Normalized int:");
    grid.addWidget(&l_normalized, row, 0);
    QCheckBox c_normalized("");
    c_normalized.setChecked(normalized_int);
    grid.addWidget(&c_normalized, row, 1);
    row++;

    QWidget buttonwidget;
    vboxlayout.addWidget(&buttonwidget);
    QHBoxLayout buttons;
    buttonwidget.setLayout(&buttons);

    QPushButton ok("OK", nullptr);
    connect (&ok, SIGNAL(clicked()), &d, SLOT (accept()));
    buttons.addWidget(&ok);

    QPushButton cancel("Cancel", nullptr);
    connect (&cancel, SIGNAL(clicked()), &d, SLOT (reject()));
    buttons.addWidget(&cancel);

    if (d.exec() == QDialog::Accepted)
    {
        int f;
        switch (c_format.currentIndex())
        {
            case 1:
                f = GL_BYTE;
                break;
            case 2:
                f = GL_UNSIGNED_BYTE;
                break;
            case 3:
                f = GL_SHORT;
                break;
            case 4:
                f = GL_UNSIGNED_SHORT;
                break;
            case 5:
                f = GL_INT;
                break;
            case 6:
                f = GL_UNSIGNED_INT;
                break;
            case 7:
                f = GL_HALF_FLOAT_ARB;
                break;
            default:
                f = GL_FLOAT;
        }

        setDim(s_dim.value(), s_size.value(), s_key.value(), f, c_normalized.isChecked());
    }
}

/*!
void TransformFeedbackBind(String name [, number keyframe])\n
Bind the Buffer to a active varying to stream out the results. Second argument keyframe is optional
*/
void Item_buffer::TransformFeedbackBind(const QString& _name, int _keyframe)
{
    transformFeedbackName = _name;
    transformFeedbackKeyFrame = _keyframe;
    boundedTransformFeedback.append(this);
    if (_name == "") boundedTransformFeedback.removeAll(this);
}

/*!
Internal used function used by glwrapper to bind the buffer to a active varying
*/
void Item_buffer::transformFeedbackBindPrivate(unsigned n)
{
    bound_by_GPU = true;
    int bytes;
    switch (format)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            bytes = 1;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT_ARB:
            bytes = 2;
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        default:
            bytes = 4;
    }


    unsigned s = bytes * size * dim ;
    unsigned ofs = s * (transformFeedbackKeyFrame > keyframes ? (keyframes - 1) : transformFeedbackKeyFrame);

    glBindBufferRangeNV(GL_TRANSFORM_FEEDBACK_BUFFER_NV, n, glbuf, ofs,  s);

}

/*!
internal function for bindable uniforms. Called only by void glwrapper_shader::Uniform (QString var, QObject *obj)
*/
void Item_buffer::bindableUniformBindPrivate(GLhandleARB shader, int location)
{
    bound_by_GPU = true;
    GL_CHECK_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER_EXT, glbuf);

    refresh();

    glUniformBufferEXT(shader, location, glbuf);
    GL_CHECK_ERROR();

}

