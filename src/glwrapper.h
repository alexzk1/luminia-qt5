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


#ifndef GLWRAPPER_H
#define GLWRAPPER_H

#include "incgl.h"
#include <math.h>
#include "item.h"

#define GL_CHECK_ERROR()                         \
    do                                              \
{                                               \
    GLenum error = glGetError();                \
    if (error != GL_NO_ERROR)                   \
    fprintf(stderr, "E: %s(%d): %s 0x%X\n", \
    __FILE__, __LINE__,             \
    __PRETTY_FUNCTION__, error);    \
    } while(0)


class glwrapper_shader: public QObject
{
    Q_OBJECT
public:
    glwrapper_shader( QObject * parent,  QString inVertex);
    glwrapper_shader( QObject * parent,  QString inVertex, QString inFragment);

    glwrapper_shader( QObject * parent,  QString inVertex, QString inGeometric, QString inFragment, int inPrimitive, int outPrimitive, int outVertices);
    glwrapper_shader( QObject * parent,  QString inVertex, QString inGeometric, int inPrimitive, int outPrimitive, int outVertices);

    ~glwrapper_shader();

public slots:
    void Bind();
    void Unbind();

    int Loc(QString var);

    void Uniform (QString var, double x);
    void Uniform (QString var, double x, double y);
    void Uniform (QString var, double x, double y, double z);
    void Uniform (QString var, double x, double y, double z, double w);
    void Uniform (QString var, const QColor& col);

    void Uniformi (QString var, int x);
    void Uniformi (QString var, int x, int y);
    void Uniformi (QString var, int x, int y, int z);
    void Uniformi (QString var, int x, int y, int z, int w);

    void Uniform (QString var, QObject *obj); //bindable uniform

    void NormalQuaternion(QString var);

    //deprecated functions use a buffer object instead
    //void UniformArray(QString var, int comp, QStringList val);
    //void UniformArrayInterpolator(QString var, int comp, float mix, QStringList val1, QStringList val2);

public:
    int getShaderHandle();

protected:
    GLhandleARB shader;
    void printInfoLog(GLhandleARB obj);
    GLhandleARB  v,g,f;
};



class glwrapper_framebuffer: public QObject {
    Q_OBJECT
public:
    glwrapper_framebuffer( QObject * parent);
    ~glwrapper_framebuffer();

public slots:
    void Bind();
    void Unbind();
    void Append(QObject* _tex, int layer = 0, int slice = 0);

private:
    GLuint fbo;
    GLuint maxbuffers;
    GLenum *buffers;
    bool *activelayer;
    int viewport[4];
    int w,h;
};

class glwrapper: public QObject
{
    Q_OBJECT
    Q_ENUMS(glconst)

    friend class glwrapper_shader;

public:
    glwrapper( QObject * parent, QString name = "gl");
    ~glwrapper() override
    {trasher.clear();}

    enum glconst {
        POINTS			= GL_POINTS,
        LINES			= GL_LINES,
        LINE_LOOP		= GL_LINE_LOOP,
        LINE_STRIP		= GL_LINE_STRIP,
        TRIANGLES		= GL_TRIANGLES,
        TRIANGLE_STRIP		= GL_TRIANGLE_STRIP,
        TRIANGLE_FAN 		= GL_TRIANGLE_FAN,
        QUADS			= GL_QUADS,
        QUAD_STRIP		= GL_QUAD_STRIP,
        POLYGON			= GL_POLYGON,
        POINT_SPRITE		= GL_POINT_SPRITE_ARB,
        LINES_ADJACENCY 	= GL_LINES_ADJACENCY_EXT,
        LINE_STRIP_ADJACENCY	= GL_LINE_STRIP_ADJACENCY_EXT,
        TRIANGLES_ADJACENCY	= GL_TRIANGLES_ADJACENCY_EXT,
        TRIANGLE_STRIP_ADJACENCY= GL_TRIANGLE_STRIP_ADJACENCY_EXT,

        //POINT_SIZE_MIN			= GL_POINT_SIZE_MIN_ARB,
        //POINT_SIZE_MAX			= GL_POINT_SIZE_MAX_ARB,
        //POINT_FADE_THRESHOLD			= GL_POINT_FADE_THRESHOLD_ARB,
        //POINT_DISTANCE_ATTENUATION		= GL_POINT_DISTANCE_ATTENUATION_ARB,
        //COORD_REPLACE				= GL_COORD_REPLACE_ARB,
        //VERTEX_PROGRAM_POINT_SIZE		= GL_VERTEX_PROGRAM_POINT_SIZE_ARB,

        ACCUM		= GL_ACCUM,
        LOAD		= GL_LOAD,
        RETURN		= GL_RETURN,
        MULT		= GL_MULT,
        ADD		= GL_ADD,

        NEAREST		= GL_NEAREST,
        LINEAR		= GL_LINEAR,
        CLAMP_TO_EDGE	= GL_CLAMP_TO_EDGE,
        CLAMP		= GL_CLAMP,
        REPEAT		= GL_REPEAT,


        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,

        // float Texturformnats
        RGBA32F			= GL_RGBA32F_ARB,
        RGB32F			= GL_RGB32F_ARB,
        ALPHA32F		= GL_ALPHA32F_ARB,
        INTENSITY32F		= GL_INTENSITY32F_ARB,
        LUMINANCE32F		= GL_LUMINANCE32F_ARB,
        LUMINANCE_ALPHA32F	= GL_LUMINANCE_ALPHA32F_ARB,

        RGBA16F			= GL_RGBA16F_ARB,
        RGB16F			= GL_RGB16F_ARB,
        ALPHA16F		= GL_ALPHA16F_ARB,
        INTENSITY16F		= GL_INTENSITY16F_ARB,
        LUMINANCE16F		= GL_LUMINANCE16F_ARB,
        LUMINANCE_ALPHA16F	= GL_LUMINANCE_ALPHA16F_ARB,

        //Texture formates

        R3_G3_B2		= GL_R3_G3_B2,
        ALPHA4			= GL_ALPHA4,
        ALPHA8			= GL_ALPHA8,
        ALPHA12			= GL_ALPHA12,
        ALPHA16			= GL_ALPHA16,
        LUMINANCE4		= GL_LUMINANCE4,
        LUMINANCE8		= GL_LUMINANCE8,
        LUMINANCE12		= GL_LUMINANCE12,
        LUMINANCE16		= GL_LUMINANCE16,
        LUMINANCE4_ALPHA4	= GL_LUMINANCE4_ALPHA4,
        LUMINANCE6_ALPHA2	= GL_LUMINANCE6_ALPHA2 ,
        LUMINANCE8_ALPHA8	= GL_LUMINANCE8_ALPHA8 ,
        LUMINANCE12_ALPHA4	= GL_LUMINANCE12_ALPHA4 ,
        LUMINANCE12_ALPHA12	= GL_LUMINANCE12_ALPHA12 ,
        LUMINANCE16_ALPHA16	= GL_LUMINANCE16_ALPHA16,
        INTENSITY		= GL_INTENSITY,
        INTENSITY4		= GL_INTENSITY4,
        INTENSITY8		= GL_INTENSITY8,
        INTENSITY12		= GL_INTENSITY12,
        INTENSITY16		= GL_INTENSITY16,
        RGB4			= GL_RGB4,
        RGB5			= GL_RGB5,
        RGB8			= GL_RGB8,
        RGB10			= GL_RGB10,
        RGB12			= GL_RGB12,
        RGB16			= GL_RGB16,
        RGBA2			= GL_RGBA2,
        RGBA4			= GL_RGBA4,
        RGB5_A1			= GL_RGB5_A1,
        RGBA8			= GL_RGBA8,
        RGB10_A2		= GL_RGB10_A2,
        RGBA12			= GL_RGBA12,
        RGBA16			= GL_RGBA16,

        //Depth Textures
        DEPTH_COMPONENT16	= GL_DEPTH_COMPONENT16,
        DEPTH_COMPONENT24	= GL_DEPTH_COMPONENT24,
        DEPTH_COMPONENT32	= GL_DEPTH_COMPONENT32,
        DEPTH24_STENCIL8	= GL_DEPTH24_STENCIL8_EXT,

        COLOR_INDEX	= GL_COLOR_INDEX,
        STENCIL_INDEX   = GL_STENCIL_INDEX,
        DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
        RED             = GL_RED,
        GREEN           = GL_GREEN,
        BLUE            = GL_BLUE,
        ALPHA           = GL_ALPHA,
        RGB             = GL_RGB,
        RGBA            = GL_RGBA,
        LUMINANCE       = GL_LUMINANCE,
        LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,


        UNSIGNED_BYTE 	= GL_UNSIGNED_BYTE,
        UNSIGNED_SHORT	= GL_UNSIGNED_SHORT,
        UNSIGNED_INT	= GL_UNSIGNED_INT,

        BYTE		= GL_BYTE,
        SHORT		= GL_SHORT,
        INT		= GL_INT,
        HALF		= GL_HALF_FLOAT_ARB,
        HALF_FLOAT	= GL_HALF_FLOAT_ARB,
        FLOAT		= GL_FLOAT,
        BITMAP		= GL_BITMAP,

        AMBIENT		= GL_AMBIENT,
        DIFFUSE		= GL_DIFFUSE,
        SPECULAR	= GL_SPECULAR,
        POSITION	= GL_POSITION,
        SPOT_DIRECTION  = GL_SPOT_DIRECTION,
        SPOT_EXPONENT	= GL_SPOT_EXPONENT,
        SPOT_CUTOFF     = GL_SPOT_CUTOFF,
        CONSTANT_ATTENUATION	= GL_CONSTANT_ATTENUATION,
        LINEAR_ATTENUATION	= GL_LINEAR_ATTENUATION,
        QUADRATIC_ATTENUATION	= GL_QUADRATIC_ATTENUATION,

        COLOR_BUFFER_BIT 	= GL_COLOR_BUFFER_BIT,
        DEPTH_BUFFER_BIT	= GL_DEPTH_BUFFER_BIT,
        STENCIL_BUFFER_BIT	= GL_STENCIL_BUFFER_BIT,

        ALPHA_TEST = GL_ALPHA_TEST,
        LESS = GL_LESS,
        EQUAL = GL_EQUAL,
        LEQUAL = GL_LEQUAL,
        GREATER = GL_GREATER,
        NOTEQUAL = GL_NOTEQUAL,
        GEQUAL = GL_GEQUAL,
        ALWAYS = GL_ALWAYS,

        DEPTH_TEST = GL_DEPTH_TEST,

        //culling

        CULL_FACE = GL_CULL_FACE,
        FRONT = GL_FRONT,
        BACK = GL_BACK,

        //poligonoffset

        POLYGON_OFFSET_FILL	= GL_POLYGON_OFFSET_FILL,
        POLYGON_OFFSET_LINE	= GL_POLYGON_OFFSET_LINE,
        POLYGON_OFFSET_POINT	= GL_POLYGON_OFFSET_POINT,

        //blending

        BLEND	= GL_BLEND,
        ZERO	= GL_ZERO,
        ONE	= GL_ONE,
        DST_COLOR	= GL_DST_COLOR,
        SRC_COLOR	= GL_SRC_COLOR,
        ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
        ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
        SRC_ALPHA	= GL_SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA	= GL_ONE_MINUS_SRC_ALPHA,
        DST_ALPHA	= GL_DST_ALPHA,
        ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
        SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,

        KEEP	= GL_KEEP,
        //ZERO	= GL_ZERO,
        REPLACE = GL_REPLACE,
        INCR	= GL_INCR,
        DECR	= GL_DECR,
        INVERT	= GL_INVERT,
        NEVER	= GL_NEVER,

        RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD_NV
    };

    void cleartrasher();
public slots:
    void Clear(int mode);

    void Enable(int v);
    void Disable(int v);
    void CullFace(int v);

    void AlphaFunc(int func, double ref);
    void BlendFunc (int a, int b);
    void StencilOp (int a, int b, int c);
    void StencilFunc(int func, int ref, int mask);
    void DepthFunc(int func);
    void DepthMask(bool b);
    void PolygonOffset(float factor, float units);


    void Begin(int mode);
    void End();
    void Vertex (double x=0, double y=0, double z=0, double w=1);
    void TexCoord (double s=0, double t=0, double p=0, double q=0);
    void Normal (double x=0, double y=0, double z=1);
    void Color (double r=0, double g=0, double b=0, double a=1);
    void Color (const QColor& col);

    void Translate(double x=0, double y=0, double z=0);
    void Rotate(double angle =0 , double x=1, double y=0, double z=0);
    void Scale(double x=0, double y=0, double z=0);
    void PushMatrix();
    void PopMatrix();
    void LoadIdentity();
    void Flush();
    void DrawArrays(int mode, int first, int count);
    void Light(int l, int n, float x = 0, float y = 0, float z = 0, float w = 0);
    void Light(int l, int n, const QColor & col);
    void PointParameter(int mode, float x, float y = 0.0, float z = 0.0);

    QObject* Shader(QObject* inVertex, QObject* inFragment);
    QObject* Shader(QString vertex, QString fragment);

    QObject* Shader(QObject* inVertex);
    QObject* Shader(QString vertex);

    QObject* Shader(QObject* inVertex, QObject* inGeometric, QObject* inFragment, int inPrimitive, int outPrimitive, int outVertices);
    QObject* Shader(QString vertex, QString geometric, QString fragment, int inPrimitive, int outPrimitive, int outVertices);

    QObject* Shader(QObject* inVertex, QObject* inGeometric, int inPrimitive, int outPrimitive, int outVertices);
    QObject* Shader(QString vertex, QString geometric, int inPrimitive, int outPrimitive, int outVertices);

    QObject* Framebuffer();

    void Screenshot(const QString& filename);

    void BeginTransformFeedback(int primitive, bool discard = false);
    void EndTransformFeedback();


protected:
    static glwrapper_shader *currentShader;

private:
    QList<QObject*> trasher;
};
#endif
