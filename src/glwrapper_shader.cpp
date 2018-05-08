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

#include "glwrapper.h"
#include <QMessageBox>
#include <QDebug>
/*!
constructor for vertex shader only shader object (useful for transformfeedback)
*/
glwrapper_shader::glwrapper_shader( QObject * parent,  QString inVertex): QObject( parent )
{
    GL_CHECK_ERROR();

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    f = 0;
    g = 0;	// no geometry shader

    QByteArray qv = inVertex.toLatin1();

    const char * vv = qv.data ();

    glShaderSourceARB(v, 1, &vv,nullptr);

    glCompileShaderARB(v);
    printInfoLog(v);

    shader = glCreateProgramObjectARB();
    glAttachObjectARB(shader,v);

    if(GLEW_NV_transform_feedback){
        qDebug() << "GLEW_NV_transform_feedback";
        static QRegExp act_var("active\\s*varying\\s+\\w+\\s+(\\w+);");
        int pos = 0;
        while ((pos = act_var.indexIn(inVertex, pos)) != -1) {
            qDebug() << "active varying " << act_var.cap(1);
            glActiveVaryingNV(shader, act_var.cap(1).toLatin1());
            pos += act_var.matchedLength();
        }
    }

    glLinkProgramARB(shader);
    GL_CHECK_ERROR();

}
/*!
constructor for shaderobject with vertex and fragmentshader
*/
glwrapper_shader::glwrapper_shader( QObject * parent,  QString inVertex, QString inFragment): QObject( parent )
{
    GL_CHECK_ERROR();

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    g = 0;	// no geometry shader

    QByteArray qv = inVertex.toLatin1();
    QByteArray qf = inFragment.toLatin1();

    const char * vv = qv.data ();
    const char * ff = qf.data ();

    glShaderSourceARB(f, 1, &ff, nullptr);
    glShaderSourceARB(v, 1, &vv, nullptr);

    glCompileShaderARB(v);
    printInfoLog(v);
    glCompileShaderARB(f);
    printInfoLog(f);
    shader = glCreateProgramObjectARB();
    glAttachObjectARB(shader,v);
    glAttachObjectARB(shader,f);

    if(GLEW_NV_transform_feedback){
        qDebug() << "GLEW_NV_transform_feedback";
        static QRegExp act_var("active\\s*varying\\s+\\w+\\s+(\\w+);");
        int pos = 0;
        while ((pos = act_var.indexIn(inVertex, pos)) != -1) {
            qDebug() << "active varying " << act_var.cap(1);
            glActiveVaryingNV(shader, act_var.cap(1).toLatin1());
            pos += act_var.matchedLength();
        }
    }

    glLinkProgramARB(shader);
    GL_CHECK_ERROR();

}

/*!
constructor for shaderobject with vertex, geometry and fragmentshader
*/
glwrapper_shader::glwrapper_shader( QObject * parent,  QString inVertex, QString inGeometric, QString inFragment, int inPrimitive, int outPrimitive, int out_vertices): QObject( parent ){
    shader = 0; //protection against buggy implementations
    if (!GL_EXT_geometry_shader4){
        qDebug() << "Panic no GL_EXT_geometry_shader4 support";
        return;
    }
    if(!glProgramParameteriEXT){
        qDebug() << "Panic glProgramParameteriEXT has NULL Pointer";
        return;
    }

    GLint  temp;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
    //printf( "GL_MAX_GEOMETRY_OUTPUT_VERTICES = %i\n" ,temp);

    if (out_vertices > temp){
        qDebug() << "GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT is "<< temp;
        out_vertices = temp;
    }

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    g = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_EXT);

    QByteArray qv = inVertex.toLatin1();
    QByteArray qf = inFragment.toLatin1();
    QByteArray qg = inGeometric.toLatin1();

    const char * vv = qv.data ();
    const char * ff = qf.data ();
    const char * gg = qg.data ();

    glShaderSourceARB(f, 1, &ff,NULL);
    glShaderSourceARB(v, 1, &vv,NULL);
    glShaderSourceARB(g, 1, &gg,NULL);

    glCompileShaderARB(v);
    glCompileShaderARB(f);
    glCompileShaderARB(g);

    shader = glCreateProgramObjectARB();
    glAttachObjectARB(shader,v);
    glAttachObjectARB(shader,g);
    glAttachObjectARB(shader,f);

    glProgramParameteriEXT(shader,GL_GEOMETRY_INPUT_TYPE_EXT ,inPrimitive);
    glProgramParameteriEXT(shader,GL_GEOMETRY_OUTPUT_TYPE_EXT ,outPrimitive);
    glProgramParameteriEXT(shader,GL_GEOMETRY_VERTICES_OUT_EXT,out_vertices);

    if(GLEW_NV_transform_feedback){
        qDebug() << "GLEW_NV_transform_feedback";
        static QRegExp act_var("active\\s*varying\\s+\\w+\\s+(\\w+);");
        int pos = 0;
        while ((pos = act_var.indexIn(inVertex, pos)) != -1) {
            qDebug() << "active varying " << act_var.cap(1);
            glActiveVaryingNV(shader, act_var.cap(1).toLatin1());
            pos += act_var.matchedLength();
        }
    }

    glLinkProgramARB(shader);
    printInfoLog(shader);
}

/*!
constructor for shaderobject with vertex and geometry but without fragmentshader. usefully for transform feedback
*/
glwrapper_shader::glwrapper_shader( QObject * parent,  QString inVertex, QString inGeometric, int inPrimitive, int outPrimitive, int out_vertices): QObject( parent ){
    shader = 0; //protection against buggy implementations
    if (!GL_EXT_geometry_shader4){
        qDebug() << "Panic no GL_EXT_geometry_shader4 support";
        return;
    }
    if(!glProgramParameteriEXT){
        qDebug() << "Panic glProgramParameteriEXT has NULL Pointer";
        return;
    }

    int  temp;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
    //printf( "GL_MAX_GEOMETRY_OUTPUT_VERTICES = %i\n" ,temp);

    if (out_vertices > temp){
        qDebug() << "GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT is "<< temp;
        out_vertices = temp;
    }

    v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    g = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_EXT);
    f = 0;

    QByteArray qv = inVertex.toLatin1();
    QByteArray qg = inGeometric.toLatin1();

    const char * vv = qv.data ();
    const char * gg = qg.data ();

    glShaderSourceARB(v, 1, &vv,NULL);
    glShaderSourceARB(g, 1, &gg,NULL);

    glCompileShaderARB(v);
    glCompileShaderARB(g);

    shader = glCreateProgramObjectARB();
    glAttachObjectARB(shader,v);
    glAttachObjectARB(shader,g);

    glProgramParameteriEXT(shader,GL_GEOMETRY_INPUT_TYPE_EXT ,inPrimitive);
    glProgramParameteriEXT(shader,GL_GEOMETRY_OUTPUT_TYPE_EXT ,outPrimitive);
    glProgramParameteriEXT(shader,GL_GEOMETRY_VERTICES_OUT_EXT,out_vertices);

    if(GLEW_NV_transform_feedback){
        qDebug() << "GLEW_NV_transform_feedback";
        static QRegExp act_var("active\\s*varying\\s+\\w+\\s+(\\w+);");
        int pos = 0;
        while ((pos = act_var.indexIn(inVertex, pos)) != -1) {
            qDebug() << "active varying " << act_var.cap(1);
            glActiveVaryingNV(shader, act_var.cap(1).toLatin1());
            pos += act_var.matchedLength();
        }
    }

    glLinkProgramARB(shader);
    printInfoLog(shader);
}



glwrapper_shader::~glwrapper_shader()
{
    glDeleteProgram(shader);
    glDeleteObjectARB(v);
    if (g) glDeleteObjectARB(g);
    glDeleteObjectARB(f);
}
/*!
Bind the shader
*/
void glwrapper_shader::Bind(){
    glUseProgramObjectARB(shader);
    glwrapper::currentShader = this;
}
/*!
Unbind the shader
*/
void glwrapper_shader::Unbind(){
    glUseProgramObjectARB(0);
    glwrapper::currentShader = nullptr;
}

/*!
number Loc(String name)\n
returns the names attribute location
*/
int glwrapper_shader::Loc(QString var){
    return glGetAttribLocationARB(shader,var.toLatin1().constData());
}

void glwrapper_shader::Uniform (QString var, double x){
    Bind();
    glUniform1fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x);
}

void glwrapper_shader::Uniform (QString var, double x, double y){
    Bind();
    glUniform2fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y);
}

void glwrapper_shader::Uniform (QString var, double x, double y, double z){
    Bind();
    glUniform3fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y,z);
}

void glwrapper_shader::Uniform (QString var, const QColor & col){
    Bind();
    float r,g,b;
    r = col.red()/255.0;
    g = col.green()/255.0;
    b = col.blue()/255.0;
    glUniform3fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),r,g,b);
}

void glwrapper_shader::Uniform (QString var, double x, double y, double z, double w){
    Bind();
    glUniform4fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y,z,w);
}

void glwrapper_shader::Uniformi (QString var, int x){
    Bind();
    glUniform1iARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x);
}

void glwrapper_shader::Uniformi (QString var, int x, int y){
    Bind();
    glUniform2iARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y);
}

void glwrapper_shader::Uniformi (QString var, int x, int y, int z){
    Bind();
    glUniform3iARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y,z);
}

void glwrapper_shader::Uniformi (QString var, int x, int y, int z, int w){
    Bind();
    glUniform4iARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),x,y,z,w);
}




void glwrapper_shader::Uniform (QString var, QObject *obj){
    Item_buffer *b = dynamic_cast<Item_buffer*>(obj);
    if(!b){
        qDebug() << obj << " is not a Item_buffer";
        return;
    }
    if(!GLEW_EXT_bindable_uniform){
        qDebug() << "GL_EXT_bindable_uniform is not supported";
        return;
    }


    Bind();

    int location = glGetUniformLocation(shader, var.toLatin1().constData());
    b->bindableUniformBindPrivate(shader,location);
}






void glwrapper_shader::NormalQuaternion(QString var){
    float mat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,mat);
    float S;

    S = sqrt(mat[0]*mat[0] + mat[4]*mat[4] + mat[8]*mat[8]);
    mat[0]/=S; mat[4]/=S; mat[8]/=S;

    S = sqrt(mat[1]*mat[1] + mat[5]*mat[5] + mat[9]*mat[9]);
    mat[1]/=S; mat[5]/=S; mat[9]/=S;

    S = sqrt(mat[2]*mat[2] + mat[6]*mat[6] + mat[10]*mat[10]);
    mat[2]/=S; mat[6]/=S; mat[10]/=S;

    float Q[4];
    S = 1 + mat[0] + mat[5] + mat[10];
    if  ( S > 0.00000001 ){
        S = sqrt(S) * 2;
        Q[0] = ( mat[6] - mat[9] ) / S;
        Q[1] = ( mat[8] - mat[2] ) / S;
        Q[2] = ( mat[1] - mat[4] ) / S;
        Q[3] = 0.25 * S;
    }
    else{
        if ( mat[0] > mat[5] && mat[0] > mat[10] )  {
            S  = sqrt( 1.0 + mat[0] - mat[5] - mat[10] ) * 2;
            Q[0] = 0.25 * S;
            Q[1] = (mat[1] + mat[4] ) / S;
            Q[2] = (mat[8] + mat[2] ) / S;
            Q[3] = (mat[6] - mat[9] ) / S;
        }
        else if ( mat[5] > mat[10] ) {
            S  = sqrt( 1.0 + mat[5] - mat[0] - mat[10] ) * 2;
            Q[0] = (mat[1] + mat[4] ) / S;
            Q[1] = 0.25 * S;
            Q[2] = (mat[6] + mat[9] ) / S;
            Q[3] = (mat[8] - mat[2] ) / S;
        } else {
            S  = sqrt( 1.0 + mat[10] - mat[0] - mat[5] ) * 2;
            Q[0] = (mat[8] + mat[2] ) / S;
            Q[1] = (mat[6] + mat[9] ) / S;
            Q[2] = 0.25 * S;
            Q[3] = (mat[1] - mat[4] ) / S;
        }
    }

    Bind();
    glUniform4fARB(glGetUniformLocationARB(shader,var.toLatin1().constData()),Q[0],Q[1],Q[2],-Q[3]);
}

/*!
internal function, returns the opengl shader handle
*/
int glwrapper_shader::getShaderHandle(){
    return shader;
}

/*!
internal functio used by constructor, to print out the compiling log. Critical errors are roprted with popups
*/
void glwrapper_shader::printInfoLog(GLhandleARB obj){
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;
    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
    if (infologLength > 0){
        infoLog = (char *)malloc(infologLength);
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);

        QString message(infoLog);
        if (message.contains("error", Qt::CaseInsensitive))QMessageBox::warning ( 0,QString("GLSL compiler error") , message );

        printf("%s\n",infoLog);
        free(infoLog);
    }
}
