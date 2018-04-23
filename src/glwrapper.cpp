/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for 
** GLSL shaders. It uses ECMA-script for tools and emulating OpenGL engines.
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

glwrapper_shader* glwrapper::currentShader = NULL;


glwrapper::glwrapper( QObject * parent, QString name ): QObject( parent ){
		setObjectName ( name );
		//printf("glwrapper\n");
		//trasher.setAutoDelete( TRUE );

		}
/*!
internal used slot. don't use...
*/
void glwrapper::cleartrasher(){
	trasher.clear();
	}

/*!
void Clear(Enum bits)\n
gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT
*/
void glwrapper::Clear(int mode){
	glClear(mode); GL_CHECK_ERROR();
	}

/*!
void Enable(Enum state)\n
Enable an OpenGL state like gl.DEPTH_TEST, gl.ALPHA_TEST, gl.BLEND or gl.CULL_FACE
*/
void glwrapper::Enable(int v){
	glEnable(v); GL_CHECK_ERROR();
	}

/*!
void Disable(Enum state)\n
Disable an OpenGL state like gl.DEPTH_TEST, gl.ALPHA_TEST, gl.BLEND or gl.CULL_FACE
*/
void glwrapper::Disable(int v){
	glDisable(v); GL_CHECK_ERROR();
	}

/*!
void CullFace(Enum mode)\n
Set the culface mode gl.FRONT or gl.BACK
*/
void glwrapper::CullFace(int v){
	glCullFace(v); GL_CHECK_ERROR();
	}
/*!
void AlphaFunc(Enum func, number ref)\n
Enum: gl.LESS, gl.[L|G|NOT]EQUAL, gl.GREATER, gl.ALWAYS. ref: Reference value
*/
void glwrapper::AlphaFunc(int func, double ref){
	glAlphaFunc(func,ref); GL_CHECK_ERROR();
	}

/*!
void BlendFunc(Enum a, Enum b)\n
gl.BLEND, gl.ZERO, gl.ONE, gl.DST_COLOR, gl.SRC_COLOR, gl.ONE_MINUS_DST_COLOR, gl.ONE_MINUS_SRC_COLOR, gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.DST_ALPHA, gl.ONE_MINUS_DST_ALPHA, gl.SRC_ALPHA_SATURATE 
*/
void glwrapper::BlendFunc (int a, int b){
	glBlendFunc(a,b); GL_CHECK_ERROR();
	}

/*!
void StencilOp(enum stencil_fail, enum Z_fail, enum Z_pass)\n
set the functions to: gl.KEEP, gl.ZERO, gl.REPLACE, gl_INCR, gl.DECR, gl.INVERT
*/
void glwrapper::StencilOp (int a, int b, int c){
	glStencilOp(a,b,c); GL_CHECK_ERROR();
	}

/*!
void StencilFunc(enum func, number ref, number mask)\n
func: Enum: gl.NEVER, gl.LESS, gl.[L|G|NOT]EQUAL, gl.GREATER, gl.ALWAYS.
*/
void glwrapper::StencilFunc(int func, int ref, int mask){
	glStencilFunc(func, ref, mask); GL_CHECK_ERROR();
	}

/*!
void DepthFunc(enum func);\n
func: Enum: gl.NEVER, gl.LESS, gl.[L|G|NOT]EQUAL, gl.GREATER, gl.ALWAYS.
Don't forget to restore the default gl.LESS
*/
void glwrapper::DepthFunc(int func){
	glDepthFunc(func);GL_CHECK_ERROR();
	}
/*!
void DepthMask(bool b);\n
Enables or disables writing to the depthbuffer.
Don't forget to restore the default "true"
*/
void glwrapper::DepthMask(bool b){
	glDepthMask(b?GL_TRUE:GL_FALSE);
	}

/*!
void PolygonOffset(number factor, number units);\n
set the polygonofset. Enable also gl.POLYGON_OFFSET_[FILL|LINE|POINT]
*/
void glwrapper::PolygonOffset(float factor, float units){
	glPolygonOffset(factor,units);
	}

/*!
void Begin(enum mode)\n
Start drawing primitives. enum: gl.POINTS gl.POINT_SPRITES gl.TRIANGLES gl.QUADS gl.TRIANGLE_STRIP gl.QUAD_STRIP
*/
void glwrapper::Begin(int mode){
	glBegin( mode );
	}

/*!
void End()\n
End drawing primitives
*/
void glwrapper::End(){
	glEnd(); GL_CHECK_ERROR();
	}

/*!
void Vertex(number x=0, number y=0, number z=0, number w=1)\n
Set the vertex position. Only valid between Begin() and End()
*/
void glwrapper::Vertex (double x, double y, double z, double w){
	glVertex4f (x, y, z, w);
	}

/*!
void TexCoord(number s=0, number t=0, number p=0, number q=0)\n
Set the texture coordinates. Only valid between Begin() and End()
*/
void glwrapper::TexCoord (double s, double t, double p, double q){
	glTexCoord4d (s, t, p, q);
	}

/*!
void Normal(number x=0, number y=0, number z=1)\n
Set the normal vector. Only valid between Begin() and End()
*/
void glwrapper::Normal (double x, double y, double z){
	glNormal3d(x, y, z);
	}

/*!
void Color (number r=0, number g=0, number b=0, number a=1)\n
Set the color. Only valid between Begin() and End()
*/
void glwrapper::Color (double r, double g, double b, double a){
	glColor4d (r, g, b, a);
	}

/*!
void Color (QColor color)\n
Set the color. Only valid between Begin() and End()
*/
void glwrapper::Color (const QColor& c){
	glColor3d(c.red()/255.0, c.green()/255.0, c.blue()/255.0);
	}

/*!
void Translate(number x, number y, number z)\n
Translate matrix function
*/
void glwrapper::Translate(double x, double y, double z){
	glTranslated(x, y, z);
	}

/*!
void Rotate(number angle, number x, number y, number z)\n
Rotate matrix function
*/
void glwrapper::Rotate(double angle, double x, double y, double z){
	glRotated(angle, x, y, z);
	}

/*!
void Scale(number x, number y, number z)\n
Scale matrix function
*/
void glwrapper::Scale(double x, double y, double z){
	glScaled(x, y, z);
	}

/*!
void PushMatrix()\n
Push a matrix onto the stack
*/
void glwrapper::PushMatrix(){
	glPushMatrix();
	}

/*!
void PopMatrix()\n
Pop a matrix from the stack
*/
void glwrapper::PopMatrix(){
	glPopMatrix();
	}

/*!
void LoadIdentity()\n
Load the Identity Matrix
*/
void glwrapper::LoadIdentity(){
	glLoadIdentity();
	}

/*!
void Flush()\n
Flush the OpenGL pipeline. It's not recommend to use it
*/
void glwrapper::Flush(){
	glFlush();
	}

/*!
void DrawArrays(enum mode, number first, number count)\n
Draw an array. Not recommend to use. Use the streams Draw() functions.
*/
void glwrapper::DrawArrays(int mode, int first, int count){
	glDrawArrays(mode, first, count);
	}

/*!
void Light(number Lightnum, enum mode, number x=0, number y=0, number z=0, number w=0)\n
Set light parameters. 
*/
void glwrapper::Light(int l, int n, float x , float y, float z, float w){
qDebug() << "glwrapper::Light(int l, int n, float x , float y, float z, float w)";
	float val[4];
	val[0]=x; val[1]=y; val[2]=z; val[3] = w;
	::glLightfv(GL_LIGHT0 +l, n, val);
	}

/*!
void Light(number Lightnum, enum mode, QColor color)\n
Set light parameters. (Color version)
*/
void glwrapper::Light(int l, int n, const QColor & col){
qDebug() << "glwrapper::Light(int l, int n, const QColor & col)";
	float val[4];
	val[0]= col.red()/255.0;
	val[1]= col.green()/255.0;
	val[2]= col.blue()/255.0;
	val[3] = 1.0;
	::glLightfv(GL_LIGHT0 +l, n, val);
	}


/*!
void PointParameter(enum mode, number x, number y, number z=0)\n
Set point parameter. Not recommend to use.
*/
void glwrapper::PointParameter(int mode, float x, float y, float z){
	switch (mode){
		case GL_POINT_DISTANCE_ATTENUATION_ARB:{
			float p[3] = {x,y,z};
			glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, p);
			break;
			}
		case GL_POINT_SIZE_MAX_ARB:{
			float maxSize = 0.0;
			glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
			if (x > maxSize) x = maxSize;
			glPointSize( x ); // 2x ??
			glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, x );
			break;
			}
		case GL_POINT_SIZE_MIN_ARB:
			glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, x );
		}
	}


/*!
Object Shader(String|Item vertexshader);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QObject* _inVertex){
	Item_shader* inVertex = dynamic_cast<Item_shader*>(_inVertex);
	
	glwrapper_shader* tmp = new glwrapper_shader(this,inVertex->text());
	trasher.append(tmp);
	return tmp;
	}

/*!
Object Shader(String|Item vertexshader);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QString vertex){
	glwrapper_shader* tmp = new glwrapper_shader(this,vertex);
	trasher.append(tmp);
	return tmp;
	}

/*!
Object Shader(String|Item vertexshader, String|Item fragmentshader);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QObject* _inVertex, QObject* _inFragment){
	Item_shader* inVertex = dynamic_cast<Item_shader*>(_inVertex);
	Item_shader* inFragment = dynamic_cast<Item_shader*>(_inFragment);

	glwrapper_shader* tmp = new glwrapper_shader(this,inVertex->text(),inFragment->text());
	trasher.append(tmp);
	return tmp;
	}

/*!
Object Shader(String|Item vertexshader, String|Item fragmentshader);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QString vertex, QString fragment){
	glwrapper_shader* tmp = new glwrapper_shader(this,vertex,fragment);
	trasher.append(tmp);
	return tmp;
	}


/*!
Object Shader(String|Item vertexshader, String|Item geometryshader, enum inPrimitive, enum outPrimitive, number maxOutVertices);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QObject* _inVertex, QObject* _inGeometric, int inPrimitive, int outPrimitive, int outVertices){
	Item_shader* inVertex = dynamic_cast<Item_shader*>(_inVertex);
	Item_shader* inGeometric = dynamic_cast<Item_shader*>(_inGeometric);
	
	glwrapper_shader* tmp = new glwrapper_shader(this,inVertex->text(),inGeometric->text(), inPrimitive, outPrimitive, outVertices);
	trasher.append(tmp);
	return tmp;
	}
/*!
Object Shader(String|Item vertexshader, String|Item geometryshader, enum inPrimitive, enum outPrimitive, number maxOutVertices);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QString vertex, QString geometric, int inPrimitive, int outPrimitive, int outVertices){
	glwrapper_shader* tmp = new glwrapper_shader(this,vertex,geometric,  inPrimitive, outPrimitive, outVertices);
	trasher.append(tmp);
	return tmp;
	}


/*!
Object Shader(String|Item vertexshader, String|Item geometryshader, String|Item fragmentshader, enum inPrimitive, enum outPrimitive, number maxOutVertices);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QObject* _inVertex, QObject* _inGeometric, QObject* _inFragment, int inPrimitive, int outPrimitive, int outVertices){
	Item_shader* inVertex = dynamic_cast<Item_shader*>(_inVertex);
	Item_shader* inGeometric = dynamic_cast<Item_shader*>(_inGeometric);
	Item_shader* inFragment = dynamic_cast<Item_shader*>(_inFragment);

	glwrapper_shader* tmp = new glwrapper_shader(this,inVertex->text(),inGeometric->text(),inFragment->text(), inPrimitive, outPrimitive, outVertices);
	trasher.append(tmp);
	return tmp;
	}

/*!
Object Shader(String|Item vertexshader, String|Item geometryshader, String|Item fragmentshader, enum inPrimitive, enum outPrimitive, number maxOutVertices);
compile all shaders at the beginning of a script.
*/
QObject* glwrapper::Shader(QString vertex, QString geometric, QString fragment, int inPrimitive, int outPrimitive, int outVertices){
	glwrapper_shader* tmp = new glwrapper_shader(this,vertex,geometric,fragment,  inPrimitive, outPrimitive, outVertices);
	trasher.append(tmp);
	return tmp;
	}


/*!
void Framebuffer()\n
Create a new Framebuffer object. This is usefull for multiple rendertarget
*/
QObject* glwrapper::Framebuffer(){
	glwrapper_framebuffer* tmp = new glwrapper_framebuffer(this);
	trasher.append(tmp);
	return tmp;
	}

/*!
Screenshot(String filename)\n
Save the actual buffer as image. Usefull for creating images or videos. PNG and JPG supported
*/
void glwrapper::Screenshot(const QString& filename){
	glFinish();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	int w = viewport[2];
	int h = viewport[3];

	unsigned char* buffer = new unsigned char[w * h * 4];
	glReadPixels(0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
	QImage image(w, h, QImage::Format_ARGB32);
	for (int y = 0; y < h; y++) {
		int OpenGL_y = h - y - 1;
		memcpy(image.scanLine(y),&buffer[OpenGL_y * w * 4],   w * 4);
		}
	qDebug() << "Screenshot" << w << h << filename;
	delete[] buffer;
	//image.rgbSwapped().save(filename);
	image.save(filename);
	}


/*!
BeginTransformFeedback();\n
Begin of transform feedback
*/
void glwrapper::BeginTransformFeedback(int primitive, bool discard){
	if(!GLEW_NV_transform_feedback){
		qDebug() << "Transform Feedback not suported";
		return;
		}
	if(!currentShader){
		qDebug() << "A shader have to be bound for Transform Feedback";
		return;
		}
	static GLint max = 0;
	if (!max)glGetIntegerv( GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_NV, &max);

	GLint *a = new GLint[max];


	int n= 0;
	for (int i = 0; i < Item_buffer::boundedTransformFeedback.size(); ++i) {
		QString name = Item_buffer::boundedTransformFeedback.at(i)->transformFeedbackName;

		int loc = glGetVaryingLocationNV( currentShader->getShaderHandle(), name.toLatin1());
		//qDebug() << name << loc;

		if(loc < 0)continue;
		if(n >= max){
			qDebug() << "to many GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_NV";
			break;
			}
		a[n] = loc;

		Item_buffer::boundedTransformFeedback.at(i)->transformFeedbackBindPrivate(n);
		n++;
		}

	glTransformFeedbackVaryingsNV(currentShader->getShaderHandle(), n, a, GL_SEPARATE_ATTRIBS_NV);

	delete a;

	glBeginTransformFeedbackNV(primitive);
	if(discard)glEnable(GL_RASTERIZER_DISCARD_NV);
	}


/*!
EndTransformFeedback();\n
End of transform feedback
*/
void glwrapper::EndTransformFeedback(){
	if(!GLEW_NV_transform_feedback)return;

	glDisable(GL_RASTERIZER_DISCARD_NV);
	glEndTransformFeedbackNV();
	}
