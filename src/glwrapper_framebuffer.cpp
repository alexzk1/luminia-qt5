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

#include "glwrapper.h"
#include <QMessageBox>

glwrapper_framebuffer::glwrapper_framebuffer( QObject * parent): QObject( parent ){
    viewport[0]= -1;
    glGenFramebuffersEXT (1, &fbo);

    //glGetIntergerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
    maxbuffers = 4;
    activelayer = new bool[maxbuffers];
    buffers = new GLenum[maxbuffers];

    for (unsigned int ac= 0; ac < maxbuffers; ac++){
        activelayer[ac] = false;
        }
    }

glwrapper_framebuffer::~glwrapper_framebuffer(){
    glDeleteFramebuffersEXT (1, &fbo);
    delete[] activelayer;
    }



void glwrapper_framebuffer::Bind(){
    GL_CHECK_ERROR();
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo);
    /*for(int i = 0; i < 16; i++){}*/
    /*glDrawBuffer (GL_FALSE);
    glReadBuffer (GL_FALSE);
    */
    //glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    int found = 0;
    for (unsigned ac = 0; ac < maxbuffers; ac++){
        if (activelayer[ac]){
            buffers[found] = GL_COLOR_ATTACHMENT0_EXT + found;
            //qDebug() << "bind" << found;
            found++;
            }
        }
    if (found != 0){
        glDrawBuffers(found, buffers);
        }
    else{
        glDrawBuffer (GL_FALSE);
        }

    //save viewport and replace it;
    if (viewport[0]== -1){

        glGetIntegerv(GL_VIEWPORT,(GLint*)viewport);
        glViewport(0,0,w,h);
        //qDebug() << w << h;
        }
    GL_CHECK_ERROR();
    }


void glwrapper_framebuffer::Unbind(){
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
    glViewport (viewport[0],viewport[1],viewport[2],viewport[3]);
    viewport[0]= -1;
    }
/*!
attach a texture to a framebuffer

*/
void glwrapper_framebuffer::Append(QObject* _tex, int layer, int slice){
    GL_CHECK_ERROR();
    if (maxbuffers < unsigned(layer)){
        //Warn
        return;
        }
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo);

    Item_texture* tex = dynamic_cast<Item_texture*>(_tex);
    if (!tex)return;
    int type = tex->getTextureType();
    //int color_depth = tex->isDepth()? GL_DEPTH_ATTACHMENT_EXT
    int attachment = tex->isDepth()?GL_DEPTH_ATTACHMENT_EXT:GL_COLOR_ATTACHMENT0_EXT+layer;

    qDebug() << type;

    switch (type){
        case GL_TEXTURE_3D:
            glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_3D, tex->getTextureID(), 0 ,slice);
            qDebug() << "append 3d slice";
            GL_CHECK_ERROR();
            break;
        case GL_TEXTURE_2D_ARRAY_EXT:
        case GL_TEXTURE_CUBE_MAP:
            qDebug() << "GL_TEXTURE_2D_ARRAY_EXT / TEXTURE_CUBE" << slice;
            if (slice==-1){ //not tested code....
                glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, attachment, tex->getTextureID(), 0);
                qDebug() << "glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, attachment = "<<attachment << ", tex->getTextureID(), 0);";
                }
            else glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, attachment, tex->getTextureID(), 0, slice);
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE:
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, type, tex->getTextureID(), 0);
            qDebug() << (type==GL_TEXTURE_2D?"append 2d":"append rectangle");
            break;
        default: qDebug() << "Panic: Unknown texture type" << type;

        }

    if (!tex->isDepth()){
        // marks the layer as active
        activelayer[layer] = true;
        if (layer ==0){
            w = int(tex->Width());
            h = int(tex->Height());
            }
        }

    GLenum status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    switch (status){
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            QMessageBox::warning ( 0,QString("FBO configuration unsupported") ,QString("FBO configuration unsupported") );
            break;
        default:
            QMessageBox::warning ( 0,QString("FBO error") , QString("FBO error") );
            printf( "FBO error %i\n ",status );
            }

    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
    }
