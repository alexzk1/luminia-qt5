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


//#include "factory.h"

#include "dialog_factory_private.h"

#include "../incgl.h"
#include "../glcam.h" // hack, to share the context if possible



class GLWidgetPrivate: public QGLWidget
{
    Q_OBJECT
public:
    GLWidgetPrivate(QWidget* parent) : QGLWidget( parent,  GLCam::shareWidget ){
        qDebug() << "GLWidgetPrivate(QWidget* parent) ";

        setMinimumSize(256,256);
        //setSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding);
        setSizePolicy ( QSizePolicy::MinimumExpanding  ,QSizePolicy::MinimumExpanding );
        setFocusPolicy(Qt::StrongFocus);
    }

    virtual~GLWidgetPrivate() override = default;
protected:
    void initializeGL() override
    {
        glewInit();
        glClearColor(0,0,0,0); 		// Set OpenGL clear to black
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
        glPixelStorei(GL_PACK_ALIGNMENT,1); //avoid trouble with non power of two textures
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        emit initE();
    }

    void paintGL() override
    {
        emit paintE();
    }

    void resizeGL( int width, int height ) override
    {
        makeCurrent();
        double zoom = 1.;

        GLdouble w = zoom * width / height;
        GLdouble h = zoom;

        glViewport( 0, 0, width, height );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glFrustum( -w, w, -h, h, 5.0, 60.0 );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        emit resizeE(width, height);
    }

signals:
    void paintE();
    void initE();
    void resizeE(int,int);
};

namespace QS{

    class GLWidget : public Widget
    {
        Q_OBJECT
        inline  GLWidgetPrivate *d() const { return (GLWidgetPrivate*)widget; }
        //QS_WIDGET(GLWidgetPrivate)
    public:
        GLWidget(QObject* = nullptr) : Widget(new GLWidgetPrivate(nullptr))
        {
            connect(d(), SIGNAL(paintE()), this, SLOT(_paintGL()));
            connect(d(), SIGNAL(resizeE(int,int)), this, SLOT(_resizeGL(int,int)));

        }


    public slots:
        void update(){
            d()->update();
        }

    private slots:
        void _paintGL(){
            emit paintGL();
        }

        void _resizeGL(int w, int h){
            emit resizeGL(w, h);
        }


    signals:
        void paintGL();
        void resizeGL(int, int);
    };



}

