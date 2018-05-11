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

#include "item_texture.h"

#include "textureformat.h"
#include "loaderpaths.h"

#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QCoreApplication>
#include <QDebug>
#include "file_loader.h"
#include "mainwindow.h"

constexpr auto static lastTexture = GL_TEXTURE31;

#define  DDS_CAPS       0x00000001
#define  DDS_HEIGHT     0x00000002
#define  DDS_WIDTH      0x00000004
#define  DDS_PITCH      0x00000008
#define  DDS_PIXELFORMAT    0x00001000
#define  DDS_MIPMAPCOUNT    0x00020000
#define  DDS_LINEARSIZE     0x00080000
#define  DDS_DEPTH      0x00800000

#define DDS_FOURCC      0x00000004
#define DDS_RGB         0x00000040
#define DDS_RGBA        0x00000041
#define DDS_ALPHA       0x00000001

#define DDS_VOLUME      0x00200000
#define DDS_CUBEMAP     0x00000200

#define DDS_CUBEMAP_POSITIVEX   0x00000400
#define DDS_CUBEMAP_NEGATIVEX   0x00000800
#define DDS_CUBEMAP_POSITIVEY   0x00001000
#define DDS_CUBEMAP_NEGATIVEY   0x00002000
#define DDS_CUBEMAP_POSITIVEZ   0x00004000
#define DDS_CUBEMAP_NEGATIVEZ   0x00008000

#define DDS_MAGIC ('D'|('D'<<8)|('S'<<16)|(' '<<24))
#define DDS_DXT1 ('D'|('X'<<8)|('T'<<16)|('1'<<24))
#define DDS_DXT3 ('D'|('X'<<8)|('T'<<16)|('3'<<24))
#define DDS_DXT5 ('D'|('X'<<8)|('T'<<16)|('5'<<24))

#include <cmath>

//missing here 8 bytes, magic (int32) and size (int32)
//i did that, so can read size 1st and compare with this structure size before loading all

struct __attribute__ (( packed )) DDS_Header_t
{
    uint32_t Flags;
    uint32_t Height;
    uint32_t Width;
    uint32_t PitchOrLinearSize;
    uint32_t Depth;
    uint32_t MipMapCount;
    uint32_t Reserved1[11];
    uint32_t pfSize;
    uint32_t pfFlags;
    uint32_t pfFourCC;
    uint32_t pfRGBBitCount;
    uint32_t pfRBitMask;
    uint32_t pfGBitMask;
    uint32_t pfBBitMask;
    uint32_t pfABitMask;
    uint32_t Caps1;
    uint32_t Caps2;
    uint32_t Reserved2[3];
};

Item_texture::Item_texture( Item *parent, const QString& name ) : Item( parent, name)
{
    //if (fbo == 0)
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

    glGenFramebuffersEXT (1,  &fbo);
    //fileNormal = new QPixmap( pix_file );
    setIcon( 0, QIcon(":/images/xpm/texture.xpm") );

    texture = 0;

    viewport[0] = -1;
    boundto = -1;

    formatindex = 0;

    type = 0;

    mipmap = false;
    width = 0;
    height = 0;
    depth = 0;
}

Item_texture::~Item_texture()
{
    glDeleteTextures(1, &texture);
    glDeleteFramebuffersEXT (1, &fbo);
}

/*!
returns some information about the texture
*/
QString Item_texture::statusText() const
{
    switch (type)
    {
        case GL_TEXTURE_3D:
            return QString("3D Texture %1 x %2 x %3 %4").arg(width).arg(height).arg(depth).arg(texturetype[formatindex].name);
        case GL_TEXTURE_2D:
            return QString("2D Texture %1 x %2 %3").arg(width).arg(height).arg(texturetype[formatindex].name);
        case GL_TEXTURE_CUBE_MAP:
            return QString("Cube Texture %1  %3").arg(width).arg(texturetype[formatindex].name);
        case GL_TEXTURE_RECTANGLE:
            return QString("Rectangle Texture %1 x %2 %3").arg(width).arg(height).arg(texturetype[formatindex].name);
        case GL_TEXTURE_2D_ARRAY_EXT:
            return QString("2D Array Texture %1 x %2 with  %3 Layer %4").arg(width).arg(height).arg(depth).arg(texturetype[formatindex].name);
        default:
            return QString("Empty Texture");
    }
}

/*!
returns true if it's a depth component
*/
bool Item_texture::isDepth()
{
    return (texturetype[formatindex].format == GL_DEPTH_COMPONENT);
}

/*!
returns the Opengl texture handle
*/
GLuint Item_texture::getTextureID()
{
    return texture;
}

/*!
void Bind(number Tmu)\n
Tmu is the number of the "Texture mapping unit" on current graphics cards 0-15
*/
void Item_texture::Bind(int tmu)
{
    GL_CHECK_ERROR();
    if (tmu > -1)
    {
        glActiveTexture(GL_TEXTURE0 + tmu);
        if (GL_CHECK_ERROR() == GL_NO_ERROR)
        {
            glBindTexture(type, texture);
            if (GL_CHECK_ERROR() == GL_NO_ERROR)
                boundto = tmu;
        }
    }

}

/*!
void Unbind()\n
Unbind a texture. Unbinding textures isn't needed.
*/
void Item_texture::Unbind()
{
    if (boundto > -1)
    {
        glActiveTexture(GL_TEXTURE0 + boundto);
        glBindTexture(type, 0);
    }
    boundto = -1;
}

/*!
void load([String filename])\n
Load a texture from a file. JPG, BMP, PNG and DDS(flipped in Y direction) are supported. If no file name is set, a filedialog will open.
*/
void Item_texture::load(const QString& filename)
{
    if (filename.isEmpty())
        fn = QFileDialog::getOpenFileName(Item::ws, tr("Open File"), "", tr("All Files (*.jpg *.png *.dds)"));
    else
        fn = LoaderPaths::findObject(filename);
    reload();
}
/*!
reload function will reload the texture from the file
*/



#include <QGLWidget>
void Item_texture::reload()
{
    const static auto ends = [](const QString & fn, const QStringList & list)->bool
    {
        for (const auto& e : list)
            if (fn.endsWith(e, Qt::CaseInsensitive))
                return true;
        return false;
    };

    const static QStringList compressed =
    {
        ".jpg", ".png", ".jpeg",

    };

    const static QStringList dds =
    {
        ".dds",
    };

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);


    if (texture)
        glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);


    if (ends(fn, compressed))
    {
        //qWarning( "Load jpg/png" );
        QImage buf;
        if ( !buf.load(fn))
        {
            // Load first image from file
            //qWarning( "Could not read image file, using single-color instead." );
            QImage dummy( 128, 128, QImage::Format_ARGB32 );
            dummy.fill( Qt::green );
            buf = dummy;
        }

        buf = QGLWidget::convertToGLFormat( buf );
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        type = GL_TEXTURE_2D;

        for (int i = 0; texturetype[i].intformat != 0; i++)
        {
            if (QString("RGBA8") == texturetype[i].name)
            {
                formatindex = i;
                break;
            }
        }

        width = buf.width();
        height = buf.height();
        depth = 1;

        glTexImage2D( GL_TEXTURE_2D, 0, texturetype[formatindex].intformat, width, height, 0, texturetype[formatindex].format, texturetype[formatindex].datatype, buf.bits() );
    }

    //process the dds file header
    if (ends(fn, dds))
    {
        QFile file(fn);
        if ( file.open( QIODevice::ReadOnly ) )
        {
            using namespace utility;
            const auto dds_magic = le2cpu(readIntegralFromFile<uint32_t>(file));
            const auto dds_size  = le2cpu(readIntegralFromFile<uint32_t>(file));
            const auto trueSize  = std::min(static_cast<size_t>(dds_size) - sizeof (dds_size), sizeof(DDS_Header_t));
            // qDebug() << "Loading DDS, magic: " << dds_magic <<", size: " << dds_size <<", effectiveSize = " << trueSize;
            DDS_Header_t DDS_Header;
            memset(&DDS_Header, 0, sizeof (DDS_Header));
            readFromFile(file, DDS_Header, trueSize);

            const qint64 delta = static_cast<qint64>(dds_size) - static_cast<qint64>(trueSize) - static_cast<qint64>(sizeof (dds_size));
            if (delta > 0)
            {
                qDebug() << "Have extra bytes in DDS file in header, skipping it: " << delta;
                file.skip(delta);
            }
            int format = 0;
            DDS_Header.pfFlags = le2cpu(DDS_Header.pfFlags);
            DDS_Header.pfFourCC = le2cpu(DDS_Header.pfFourCC);
            DDS_Header.pfRGBBitCount = le2cpu(DDS_Header.pfRGBBitCount);
            DDS_Header.Caps2 = le2cpu(DDS_Header.Caps2);
            DDS_Header.Width = le2cpu(DDS_Header.Width);
            DDS_Header.Height = le2cpu(DDS_Header.Height);
            DDS_Header.Depth = le2cpu(DDS_Header.Depth);
            DDS_Header.MipMapCount = le2cpu(DDS_Header.MipMapCount);

            if (DDS_Header.pfFlags & DDS_FOURCC)
            {

                switch (DDS_Header.pfFourCC)
                {
                    case DDS_DXT1:
                        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                        break;
                    case DDS_DXT3:
                        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                        break;

                    case DDS_DXT5:
                        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                        break;
                }
            }

            else
                if (DDS_Header.pfRGBBitCount == 32 && (DDS_Header.pfFlags & DDS_RGB))
                    format = GL_RGBA8;
                else
                    if (DDS_Header.pfRGBBitCount == 24 && (DDS_Header.pfFlags & DDS_RGB))
                        format = GL_RGB8;
                    else
                        if (DDS_Header.pfRGBBitCount == 8 && (DDS_Header.pfFlags & DDS_ALPHA))
                            format = GL_ALPHA8;
                        else
                            if (DDS_Header.pfRGBBitCount == 8)
                                format = GL_LUMINANCE8;
                            else
                            {
                                qDebug() << "Not a valid Texture format in DDS file";
                                return;
                            }

            bool m = false;
            if (DDS_Header.Flags & DDS_MIPMAPCOUNT)
            {
                qDebug() << "DDS File: Mipmap found" << DDS_Header.MipMapCount;
                m = true;
            }

            if (DDS_Header.Caps2 & DDS_VOLUME)
            {
                qDebug() << "DDS File: 3D texture";
                Image3d (DDS_Header.Width, DDS_Header.Height, DDS_Header.Depth, format, m);
                type = GL_TEXTURE_3D;
            }
            else
                if ((DDS_Header.Caps2 & 0x0000FE00) == 0x0000FE00) //Cubemap with all faces
                {

                    qDebug() << "DDS File: Cubemap\n" ;
                    GL_CHECK_ERROR();
                    ImageCube (DDS_Header.Width, format, m);
                    GL_CHECK_ERROR();
                    type = GL_TEXTURE_CUBE_MAP;
                }
                else
                {
                    qDebug() << "DDS File: 2D texture\n";
                    Image2d (DDS_Header.Width, DDS_Header.Height, format, m);
                    type = GL_TEXTURE_2D;
                }

            qint64 datalen =  file.size() - (delta + static_cast<qint64>((sizeof (dds_magic) + sizeof (dds_size) + trueSize)));
            qDebug() << "DDS data len: " << datalen;

            auto *buffer = new char[datalen];

            file.read (buffer, datalen);
            qDebug() << "setData for texture named: " << getName();
            setData(buffer, datalen);
            qDebug() << "setData complete";
            delete [] buffer;
        }
    }
    GL_CHECK_ERROR();
    //   qDebug() << "Texture type: " <<type;
}

/*!
function to fill a texture with rawdata. The format is the same as a DDS file without header.
*/
void Item_texture::setData(char *data, qint64 len)
{
    char *buffer;
    buffer = data;


    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        qDebug() << "Texture was not bound" ;
        glBindTexture(type, texture);
    }


    GL_CHECK_ERROR();


    int miplevels = 1;
    if (mipmap)
        miplevels += int(log2 (width | height | depth));

    for (GLenum cub = 0, cubeloop = ((type == GL_TEXTURE_CUBE_MAP ) ? 6 : 1); cub < cubeloop; ++cub)
    {
        int64_t w = width;
        int64_t h = height;
        int64_t d = depth;

        for (int mip = 0; mip < miplevels; mip++)
        {
            auto  num_of_bytes = d * h * w;
            if (texturetype[formatindex].bits >= 8)
                num_of_bytes *= texturetype[formatindex].bits / 8;
            else
                num_of_bytes /= 8 / texturetype[formatindex].bits;

            if (num_of_bytes < texturetype[formatindex].blocksize / 8)
                num_of_bytes = texturetype[formatindex].blocksize / 8;
            qDebug() << "Texture Miplevel " << mip << " bytes: " << num_of_bytes << w << h << d ;

            if ((buffer - data  + num_of_bytes) > static_cast<decltype(num_of_bytes)>(len))
            {
                qDebug() << "Texture: Not enough data, canceled loading of \"" << getName() << "\"";
                return;
            }

            if (texturetype[formatindex].compressed)
            {
                switch (type)
                {
                    case GL_TEXTURE_2D:
                        //glCompressedTexImage2D(GL_TEXTURE_2D, mip, texturetype[formatindex].intformat,w, h, 0, num_of_bytes, buffer);
                        glCompressedTexSubImage2D(GL_TEXTURE_2D, mip, 0, 0, w, h, texturetype[formatindex].intformat, num_of_bytes, buffer);
                        break;
                    case GL_TEXTURE_3D:
                        //glCompressedTexImage3D(GL_TEXTURE_3D, mip, texturetype[formatindex].intformat,w, h, d, 0, num_of_bytes, buffer);
                        glCompressedTexSubImage3D(GL_TEXTURE_3D, mip, 0, 0, 0, w, h, d, texturetype[formatindex].intformat, num_of_bytes, buffer);
                        break;
                    case GL_TEXTURE_CUBE_MAP:
                        //glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cub, mip, texturetype[formatindex].intformat,w, h, 0 ,num_of_bytes , buffer);
                        glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cub, mip, 0, 0, w, h, texturetype[formatindex].intformat, num_of_bytes, buffer);
                        break;
                }
            }
            else
            {
                switch (type)
                {
                    case GL_TEXTURE_2D:
                        glTexSubImage2D( GL_TEXTURE_2D, mip, 0, 0, w, h, texturetype[formatindex].format,  texturetype[formatindex].datatype, buffer);
                        break;
                    case GL_TEXTURE_3D:
                        GL_CHECK_ERROR();
                        glTexSubImage3D( GL_TEXTURE_3D, mip, 0, 0, 0, w, h, d, texturetype[formatindex].format, texturetype[formatindex].datatype, buffer);
                        break;
                    case GL_TEXTURE_CUBE_MAP:
                        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cub, mip, 0, 0, w, h, texturetype[formatindex].format,  texturetype[formatindex].datatype, buffer);
                        break;
                }
            }

            w /= 2;
            if (w == 0) w = 1;
            h /= 2;
            if (h == 0) h = 1;
            d /= 2;
            if (d == 0) d = 1;

            buffer += num_of_bytes;
        }
    }
}


Item_texture::data_t Item_texture::getData() const
{
    GLsizei miplevels = 1;
    if (mipmap)
        miplevels += static_cast<decltype (miplevels)>(log2 (width | height | depth));

    const unsigned int cublimit = (type == GL_TEXTURE_CUBE_MAP ? 6 : 1 );

    int64_t len = 0;
    for (unsigned int cub = 0; cub < cublimit; ++cub)
    {
        int64_t w = width;
        int64_t h = height;
        int64_t d = depth;
        for (int mip = 0; mip < miplevels; mip++)
        {
            int64_t num_of_bytes = texturetype[formatindex].bits * d * h * w / 8;
            if (num_of_bytes < texturetype[formatindex].blocksize / 8)
                num_of_bytes = texturetype[formatindex].blocksize / 8;
            w /= 2;
            if (w == 0) w = 1;
            h /= 2;
            if (h == 0) h = 1;
            d /= 2;
            if (d == 0) d = 1;
            len += num_of_bytes;
        }
    }

    //qDebug() << "QByteArray Item_texture::getData()" << len << "bytes";

    data_t data(static_cast<size_t>(std::max(0l, len)), 0);

    char *buffer = data.data();

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }

    for (unsigned int cub = 0; cub < cublimit; ++cub)
    {
        int64_t w = width;
        int64_t h = height;
        int64_t d = depth;

        for (int mip = 0; mip < miplevels; mip++)
        {

            int64_t num_of_bytes = texturetype[formatindex].bits * d * h * w / 8;

            if (num_of_bytes < texturetype[formatindex].blocksize / 8)
                num_of_bytes = texturetype[formatindex].blocksize / 8;

            //qDebug() << "Texture Miplevel " << mip << " bytes: " << num_of_bytes;

            if (texturetype[formatindex].compressed)
                glGetCompressedTexImage(type + cub, mip, buffer);
            else
                glGetTexImage(type + cub, mip, texturetype[formatindex].format, texturetype[formatindex].datatype, buffer);

            w /= 2;
            if (w == 0) w = 1;
            h /= 2;
            if (h == 0) h = 1;
            d /= 2;
            if (d == 0) d = 1;

            buffer += num_of_bytes;
        }
    }
    /*
    if (mipmap){
        data.resize(buffer - data.data()); //remove the reseve bytes
        }
    else{
        data.resize(width * height * depth * texturetype[formatindex].bits / 8 * (type == GL_TEXTURE_CUBE_MAP ? 6:1 ));
        }
    */
    return data;
}



/*!
void MinFilter(Enum filter) deprecated \n
MinFilter = filter \n
Set the minification filter to gl.(NEAREST|LINEAR)[_MIPMAP_(NEAREST|LINEAR)]
*/
void Item_texture::MinFilter(int f)
{
    minFilter = f;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_MIN_FILTER, f );
}
/*!
void MagFilter(Enum filter) deprecated \n
MagFilter = filter \n
Set the textures filter to gl.NEAREST or gl.LINEAR
*/
void Item_texture::MagFilter(int f)
{
    magFilter = f;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_MAG_FILTER, f );
}

/*!
Anisotropic = number\
property for setting the anisotropic level. 1 = off
*/
void Item_texture::Anisotropic(int a)
{
    anisotropic = a;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_MAX_ANISOTROPY_EXT, a );

}


/*!
void BindFBO([number slice])
Bind a texture as FrameBufferObject to use it as rendertarget. Slice is optional for 3D textures. The viewport will be backed up.
*/
void Item_texture::BindFBO(int slice)
{
    GL_CHECK_ERROR();
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo);
    switch (type)
    {
        case GL_TEXTURE_3D:
            if (slice == -1)
            {
                qDebug() << "Item_texture::BindFBO() a slice as argument is required";
                return;
            }
            glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_3D, texture, 0, slice);
            break;
        case GL_TEXTURE_2D_ARRAY_EXT: //not tested code....
        case GL_TEXTURE_CUBE_MAP:
            if (slice != -1)
            {
                qDebug() << "Item_texture::BindFBO() Binding a slice is currently not supported";
                return;
            }
            else
            {
                if (texturetype[formatindex].format != GL_DEPTH_COMPONENT)
                    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture, 0);
                else
                {
                    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, texture, 0);
                    glDrawBuffer (GL_FALSE);
                    glReadBuffer (GL_FALSE);
                    //qDebug() << "Depth Cube/Array";
                }
            }
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE:
            if (texturetype[formatindex].format != GL_DEPTH_COMPONENT)
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, type, texture, 0);
            else
            {
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, type, texture, 0);
                glDrawBuffer (GL_FALSE);
                glReadBuffer (GL_FALSE);
            }
            break;
    }
    GLenum status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            //  qDebug() << "FBO OK";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            //QMessageBox::warning ( 0,QString("FBO configuration unsupported") ,QString("FBO configuration unsupported") );
            qDebug() << "FBO configuration unsupported";
            break;
        default:
            //QMessageBox::warning ( 0,QString("FBO error") , QString("FBO error %1").arg(status));
            qDebug() << "FBO error" << status;

    }
    //save viewport and replace it;
    if (viewport[0] == -1)
    {
        glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);
        glViewport(0, 0, width, height);
    }
    GL_CHECK_ERROR();
}
/*!
void UnbindFBO()\n
Unbind the as FrameBufferObject used texture and restore viewport.
*/
void Item_texture::UnbindFBO()
{
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
    //restore vieport
    glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);
    viewport[0] = -1;
}

/*!
void ViewPort(number leftX, number upperY, number width, number height)\n
Set the viewport in FBO mode. all values between 0.0 and 1.0
*/
void Item_texture::ViewPort(float a, float b, float c, float d)
{
    if (viewport[0] == -1) return;
    glViewport ( int(width * a), int(height * b), int(width * c), int(height * d));
}

/*!
void Image2d(number width, number height, enum textureformat, bool mipmap)\n
Format the texture...
*/
void Item_texture::Image2d(GLsizei w, GLsizei h, GLint _format, bool _mipmap)
{
    qDebug() << _format;
    if (_format == -1)_format = GL_RGBA8;
    for (int i = 0; texturetype[i].intformat != 0; i++)
    {
        if ( texturetype[i].intformat == _format)
        {
            formatindex = i;
            break;
        }
    }
    mipmap = _mipmap;
    width = w;
    height = h;
    depth = 1;
    test_w_h_d_limit();

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);

    if (!texture)glGenTextures(1, (GLuint*)&texture);

    type = GL_TEXTURE_2D;

    glBindTexture(GL_TEXTURE_2D, texture);

    int mipw = w;
    int miph = h;

    int miplevels = 1;
    if (mipmap) miplevels += int(log2 (width | height | depth));

    qDebug() << "Miplevels" << miplevels;

    for (int mip = 0; mip < miplevels; mip ++)
    {
        glTexImage2D (GL_TEXTURE_2D, mip, texturetype[formatindex].intformat, mipw, miph, 0, texturetype[formatindex].format, GL_FLOAT, nullptr);
        mipw = mipw >> 1;
        if (mipw == 0) mipw = 1;
        miph = miph >> 1;
        if (miph == 0) miph = 1;
    }

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/*!
void ImageRect(number width, number height, enum textureformat, bool mipmap)\n
Format the texture as Rectangle texture. Mipmap should be false
*/
void Item_texture::ImageRect(GLsizei w, GLsizei h, GLint _format, bool _mipmap)
{
    qDebug() << _format;
    if (_format == -1)_format = GL_RGBA8;
    for (int i = 0; texturetype[i].intformat != 0; i++)
    {
        if ( texturetype[i].intformat == _format)
        {
            formatindex = i;
            break;
        }
    }
    mipmap = _mipmap;
    width = w;
    height = h;
    depth = 1;
    test_w_h_d_limit();

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);

    if (!texture)glGenTextures(1, (GLuint*)&texture);

    type = GL_TEXTURE_RECTANGLE;

    glBindTexture(GL_TEXTURE_RECTANGLE, texture);

    int mipw = w;
    int miph = h;

    int miplevels = 1;
    if (mipmap) miplevels += int(log2 (width | height | depth));

    qDebug() << "Miplevels" << miplevels;

    for (int mip = 0; mip < miplevels; mip ++)
    {
        glTexImage2D (GL_TEXTURE_RECTANGLE, mip, texturetype[formatindex].intformat, mipw, miph, 0, texturetype[formatindex].format, GL_FLOAT, NULL);
        mipw = mipw >> 1;
        if (mipw == 0) mipw = 1;
        miph = miph >> 1;
        if (miph == 0) miph = 1;
    }

    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/*!
void Image2dArray(number width, number height, number depth, enum textureformat, bool mipmap)\n
Format as 2D Array texture...
*/

void Item_texture::Image2dArray(GLsizei w, GLsizei h, GLsizei d, GLint _format, bool _mipmap)
{
    if (!GLEW_EXT_texture_array)
    {
        qDebug() << "GL_EXT_texture_array not supported";
        return;
    }

    if (_format == -1)_format = GL_RGBA8;

    for (int i = 0; texturetype[i].intformat != 0; ++i)
    {
        if ( texturetype[i].intformat == _format)
        {
            formatindex = i;
            break;
        }
    }
    mipmap = _mipmap;

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);

    if (!texture)
        glGenTextures(1, (GLuint*)&texture);
    type = GL_TEXTURE_2D_ARRAY_EXT;

    width = w;
    height = h;
    depth = d;
    test_w_h_d_limit();

    glBindTexture(type, texture);

    int mipw = w;
    int miph = h;

    int miplevels = 1;
    if (mipmap) miplevels += int(log2 (width | height | depth));

    for (int mip = 0; mip < miplevels; mip ++)
    {
        glTexImage3D (type, mip, texturetype[formatindex].intformat, mipw, miph, d, 0, texturetype[formatindex].format, GL_FLOAT, nullptr);
        mipw = mipw >> 1;
        if (mipw == 0) mipw = 1;
        miph = miph >> 1;
        if (miph == 0) miph = 1;
    }

    glTexParameteri (type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    qDebug() << "Image2dArray GL_TEXTURE_2D_ARRAY_EXT";
}


/*!
void Image3d(number width, number height, number depth, enum textureformat, bool mipmap)\n
Format as 3D texture...
*/

void Item_texture::Image3d(GLsizei w, GLsizei h, GLsizei d, GLint _format, bool _mipmap)
{
    if (_format == -1)_format = GL_RGBA8;

    for (int i = 0; texturetype[i].intformat != 0; i++)
    {
        if ( texturetype[i].intformat == _format)
        {
            formatindex = i;
            break;
        }
    }

    mipmap = _mipmap;

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);

    if (!texture)
        glGenTextures(1, (GLuint*)&texture);
    type = GL_TEXTURE_3D;

    width = w;
    height = h;
    depth = d;
    test_w_h_d_limit();

    glBindTexture(type, texture);

    int mipw = w;
    int miph = h;
    int mipd = d;

    int miplevels = 1;
    if (mipmap) miplevels += int(log2 (width | height | depth));

    for (int mip = 0; mip < miplevels; mip ++)
    {
        glTexImage3D (GL_TEXTURE_3D, mip, texturetype[formatindex].intformat, mipw, miph, mipd, 0, texturetype[formatindex].format, GL_FLOAT, nullptr);
        mipw = mipw >> 1;
        if (mipw == 0) mipw = 1;
        miph = miph >> 1;
        if (miph == 0) miph = 1;
        mipd = mipd >> 1;
        if (mipd == 0) mipd = 1;
    }

    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

/*!
void ImageCube(number width, enum textureformat, bool mipmap)\n
Format as Cube texture...
*/
void Item_texture::ImageCube(GLsizei w, GLint _format, bool _mipmap)
{
    GL_CHECK_ERROR();
    if (_format == -1)_format = GL_RGBA8;

    for (int i = 0; texturetype[i].intformat != 0; i++)
    {
        if ( texturetype[i].intformat == _format)
        {
            formatindex = i;
            break;
        }
    }

    mipmap = _mipmap;
    width = w;
    height = w;
    depth = 1;
    test_w_h_d_limit();

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
        glActiveTexture(lastTexture);

    if (!texture)glGenTextures(1, (GLuint*)&texture);
    GL_CHECK_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int miplevels = 1;
    if (mipmap) miplevels += int(log2 (width | height | depth));



    type = GL_TEXTURE_CUBE_MAP;
    for (int cub = 0 ; cub  < 6 ; cub++)
    {
        int mipw = w;
        for (int mip = 0; mip < miplevels; mip ++)
            glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + cub, mip, texturetype[formatindex].intformat, mipw, mipw, 0, texturetype[formatindex].format, GL_FLOAT, nullptr);
    }

    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


/*!
property funtion to set the compare mode
*/
void Item_texture::CompareMode(bool _compareMode)
{
    compareMode = _compareMode;

    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    if (compareMode == false) glTexParameteri( type, GL_TEXTURE_COMPARE_MODE_ARB, GL_FALSE);
    else glTexParameteri( type, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
}

/*!
void WrapS(Enum mode) deprecated \n
WrapS = mode \n
set the T wrapping mode to gl.REPEAT gl.CLAMP or gl.CLAMP_TO_EDGE
*/
void Item_texture::WrapS(int m)
{
    wrapS = m;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_WRAP_S, m );
}

/*!
void WrawT(Enum mode) deprecated \n
WrapT = mode \n
set the T wrapping mode to gl.REPEAT gl.CLAMP or gl.CLAMP_TO_EDGE
*/
void Item_texture::WrapT(int m)
{
    wrapT = m;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_WRAP_T, m );
}

/*!
void WrawR(Enum mode) deprecated \n
WrapR = mode \n
set the T wrapping mode to gl.REPEAT gl.CLAMP or gl.CLAMP_TO_EDGE
*/
void Item_texture::WrapR(int m)
{
    wrapT = m;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }
    glTexParameteri( type, GL_TEXTURE_WRAP_R, m );
}

/*!
void GenerateMipmap() \n
Generates mipmaps
*/
void Item_texture::GenerateMipmap()
{
    if (type == GL_TEXTURE_RECTANGLE) return; //no mipmap for rectangle
    mipmap = true;
    if (boundto > -1)
        glActiveTexture(GL_TEXTURE0 + boundto);
    else
    {
        glActiveTexture(lastTexture);
        glBindTexture(type, texture);
    }

    glGenerateMipmapEXT(type);
}

/*!
void Print(number x, number y, String text)\n
glTexSubImage2D wrapper to print text into a GL_ALPHA texture. Usefull for a textshader.
Only 2d and Rectangle textures are supported
*/
void Item_texture::Print(int x, int y, const QString& text)
{
    if (type != GL_TEXTURE_2D && type !=  GL_TEXTURE_RECTANGLE)return;
    if (texturetype[formatindex].intformat != GL_ALPHA8 && texturetype[formatindex].intformat != GL_LUMINANCE8 && texturetype[formatindex].intformat != GL_INTENSITY8)
    {
        qDebug() << "Wrong internal format for printing to texture";
        return;
    }

    QByteArray t = text.toLatin1();

    int len = t.length();
    if ((len + x) > width) len = width - x;

    const char *tp =  t.data();
    qDebug() << tp;

    glTexSubImage2D(type, 0, x, y, len, 1, GL_ALPHA, GL_UNSIGNED_BYTE, tp);

}

QString Item_texture::getType() const
{
    return QString("Texture");
}

void Item_texture::addMenu(QMenu *menu)
{
    menu->addAction ( QIcon(":/images/xpm/load.xpm"), tr("Load File..."), this, SLOT(load()) );
}

void Item_texture::test_w_h_d_limit()
{
    //dunno, this does not seem as working, but let it be for now.
    //will add custom base64 & zip maybe
    if (context() && static_cast<double>(width) * height * depth > std::numeric_limits<int64_t>::max())
        context()->throwError("Texture's width * height * depth are overflowing.");
}

/*!
void getFormat()\n
Returns the OpenGL Texture format enum.
*/
int Item_texture::getFormat()
{
    return texturetype[formatindex].intformat;
}

QString Item_texture::getFormatString() const
{
    return QString(texturetype[formatindex].name);
}
/*!
Returns if the texture has mipmaps
*/
bool Item_texture::hasMipMap()
{
    return mipmap;
}

/*!
search the textureformat enum by string
*/

int Item_texture::formatFromString(const QString& n)
{
    for (int i = 0; texturetype[i].intformat != 0; i++)
        if (n == texturetype[i].name)
            return texturetype[i].intformat;
    return 0;
}
