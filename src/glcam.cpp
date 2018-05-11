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
#include "glcam.h"
#include <QWheelEvent>
#include <QKeyEvent>
#include "globals.h"

QGLWidget* GLCam::shareWidget = nullptr;


//GLCam::GLCam( QWidget* parent, Item_cam* _cam, const QGLWidget *shareWidget): QGLWidget( parent, shareWidget ){
GLCam::GLCam( QWidget* parent, Item_cam* _cam):
    QGLWidget( parent, GLCam::shareWidget )
{
    if (!shareWidget)
        shareWidget = this;

    cam = _cam;

    object = 0;
    zoom = 1.0 ;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumHeight ( 240 );
    setMinimumWidth ( 320 );
    setFocusPolicy(Qt::StrongFocus);

    near_plane = 5.0;
    far_plane = 60.0;
}


GLCam::~GLCam()
{
    makeCurrent();
}

/*!
paintfuntion cals every "render" function from every active script
*/
void GLCam::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);

    glPointSize(5.0);

    cam->world->setCamSize(width(), height());

    cam->profiler->newFrame();

    QTreeWidgetItemIterator it(cam->world);
    while (*it)
    {
        float mat[16];
        cam->getInverseMatrix(mat);
        glLoadMatrixf(mat);
        if (auto* scriptitem = dynamic_cast<Item_script*>(*it))
        {
            //simple code to apply the nodes matices, a stack based could be better
            QList<float*> matrixlist;
            auto* m = reinterpret_cast<Item_matrix*>(scriptitem); //Only used to get the parent
            while ( (m = dynamic_cast<Item_matrix*>(m->parent())) != nullptr)
            {
                float *mat =  m->getMatrix();
                matrixlist.append(mat);
            }

            for (int k = matrixlist.size() - 1; k >= 0; k--)
                glMultMatrixf(matrixlist.at(k));

            scriptitem->Call("render");
            Item_buffer::UnbindAll(); //unbind all VBOs
        }
        ++it;
    }

    glUseProgramObjectARB(0); //disable all active shaders

    GL_CHECK_ERROR();
    cam->profiler->render(this);
    GL_CHECK_ERROR();
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLCam::initializeGL()
{
    glewInit();
    if (!GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
        FATAL_ERROR("No GLSL support\n");
    if (!GLEW_EXT_framebuffer_object)
        FATAL_ERROR("No GL_EXT_framebuffer_object support\n");

    glClearColor(0, 0, 0, 0);   // Set OpenGL clear to black
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB); //don't know why that state exist....

    GLint i;
    glGetIntegerv( GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_NV, &i);
    glGetIntegerv( GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS_NV, &i);
    glGetIntegerv( GL_MAX_BINDABLE_UNIFORM_SIZE_EXT, &i);
    glPixelStorei(GL_PACK_ALIGNMENT, 1); //avoid trouble with non power of two textures
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

/*!
mousePressEvent placeholder
*/
void GLCam::mousePressEvent (QMouseEvent*)
{
    /*
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -10.0 );
    scale=zoom;
    glScalef( scale, scale, scale );
    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 1.0, 0.0 );
    glRotatef( zRot, 0.0, 0.0, 1.0 );
    glPointSize(5.0);
    glBegin( GL_POINTS );
    for (int i =0;i<data->num_of_vertices;i++){
        glColor3f(0,0,1.0*(i+1)/data->num_of_vertices);
        glVertex3f( data->vertices[i].x,data->vertices[i].y,data->vertices[i].z );
        //printf("Vertex: %f %f %f\n",data->vertices[i].x,data->vertices[i].y,data->vertices[i].z);
        }
    glEnd();
    //paintGL();
    glReadPixels(ev->pos().x(),height()- ev->pos().y(),1,1,GL_RGB,GL_FLOAT,&readpixel);
    printf("Color: %d %d %d\n",abs(readpixel[0]*255),abs(readpixel[1]*255),abs(readpixel[2]*255));
    //abs((rgbpixels[p]*255)
    return; */
}

/*!
mouseMoveEvent placeholder
*/
void GLCam::mouseMoveEvent (QMouseEvent*)
{
}

/*!
mouseReleaseEvent placeholder
*/
void GLCam::mouseReleaseEvent (QMouseEvent*)
{
    /*glReadPixels(ev->pos().x(),height()- ev->pos().y(),1,1,GL_RGB,GL_FLOAT,&readpixel);
    printf("Color: %d %d %d\n",abs(readpixel[0]*255),abs(readpixel[1]*255),abs(readpixel[2]*255));
    //abs((rgbpixels[p]*255)*/
}

/*!
wheel event modifies the FOV (zoom) of the projection matrix
*/
void GLCam::wheelEvent (QWheelEvent *ev)
{
    makeCurrent();
    if (ev->delta() > 0.0)    zoom *= 0.75;
    else    zoom *= 1.33333333;
    //resizeGL(width(),height());

    GLdouble w = zoom * width() / height();
    GLdouble h = zoom;

    //glViewport( 0, 0, width, height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum( -w, w, -h, h, near_plane, far_plane );
    glMatrixMode(GL_MODELVIEW);

    paintGL();
    swapBuffers();

}


/*!
Set up the OpenGL view port, matrix mode, etc.
*/

void GLCam::resizeGL( int width, int height )
{
    makeCurrent();
    GLdouble w = zoom * width / height;
    GLdouble h = zoom;

    glViewport( 0, 0, width, height );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum( -w, w, -h, h, 5.0, 60.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    QTreeWidgetItemIterator it(cam->world);
    while (*it)
    {
        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it))scriptitem->Call(QString("resizeEvent"), QVariantList() << width << height);
        ++it;
    }
}


void GLCam::resizeEvent(QResizeEvent *e )
{
    QGLWidget::resizeEvent(e);
}

/*!
keypress event calles the "keypressEvent" function from each running script
*/
void GLCam::keyPressEvent(QKeyEvent *e)
{
    static QWidget *fullscreen = nullptr;
    if (e->key() == Qt::Key_F && !fullscreen)
    {
        fullscreen = parentWidget();
        setParent(nullptr);
        showFullScreen();
    }
    else
        if (e->key() == Qt::Key_F && fullscreen)
        {
            setParent(fullscreen);
            if (QDockWidget *w = dynamic_cast<QDockWidget*>(fullscreen))w->setWidget(this);
            fullscreen = nullptr;
        }

    QTreeWidgetItemIterator it(cam->world);
    while (*it)
    {
        //#ifndef QTSCRIPT
        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it))scriptitem->Call(QString("keypressEvent"), QVariantList() << e->text ());
        //#endif
        //#ifdef QTSCRIPT
        //if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it))scriptitem->call(QString("keypressEvent"),  QScriptValueList()<< QScriptValue (e->text ()));
        //#endif

        ++it;
    }
}

/*!
internal function for setting the Near plane
*/
void GLCam::setNear(double val)
{
    near_plane = val;

    GLdouble w = zoom * width() / height();
    GLdouble h = zoom;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -w, w, -h, h, near_plane, far_plane );
    glMatrixMode(GL_MODELVIEW);
}

/*!
internal function for setting the Far plane
*/
void GLCam::setFar(double val)
{
    far_plane = val;

    GLdouble w = zoom * width() / height();
    GLdouble h = zoom;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -w, w, -h, h, near_plane, far_plane );
    glMatrixMode(GL_MODELVIEW);
}


