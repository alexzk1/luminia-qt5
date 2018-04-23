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

#include "profiler.h"

#include <GL/glew.h>
#include <QtGui>

#define GL_CHECK_ERROR()                         \
          do                                              \
          {                                               \
              GLenum error = glGetError();                \
              if (error != GL_NO_ERROR)                   \
                  fprintf(stderr, "E: %s(%d): %s 0x%X\n", \
                          __FILE__, __LINE__,             \
                          __PRETTY_FUNCTION__, error);    \
          } while(0)


Profiler::Profiler() : QObject(){
	active = false;

	text = new QTextEdit();
	text->setReadOnly(true);

	dock = new QDockWidget("Profiler");
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	dock->setWidget(text);

	QWidgetList l = QApplication::topLevelWidgets();
	for (int i = 0; i < l.size(); i++){
		if (QMainWindow* w = dynamic_cast<QMainWindow*>(l.at(i))) w->addDockWidget(Qt::RightDockWidgetArea, dock);
		}

	dock->hide();

	registered =0;
	count = 0;
	queries = NULL;

	}

Profiler::~Profiler(){
	delete text;
	delete dock;

	glDeleteQueries(registered*2,queries);
	delete[] queries;
	}

void Profiler::newFrame(){
	

	if (active && count > registered){
		if (queries != NULL){
			glDeleteQueries(registered*2,queries);
			delete[] queries;
			}
		registered = count ;
		queries = new GLuint[registered*2];
		glGenQueries (registered*2,queries);
		}
	count = 0;
	}

void Profiler::start(){
	if (active && (count < registered) ){
		glBeginQuery(GL_SAMPLES_PASSED_ARB, queries[count * 2]);
		glBeginQuery(GL_TIME_ELAPSED_EXT, queries[count * 2 + 1]);
		//qDebug() << " start";
		}
	}
 

void Profiler::stop(){
	if (active && count < registered ){
		glEndQuery(GL_TIME_ELAPSED_EXT);
		glEndQuery(GL_SAMPLES_PASSED_ARB);
		}
	count++;
	}



void Profiler::toggle(bool b){
	active = b;
		if (active){
		//w->renderText ( 10, 20, QString ("test") ); //overlay 
		dock->show();
		}
	else{
		dock->hide();
		}
	}


void Profiler::render(QGLWidget* /* w*/){
	if (active && count != 0){
		//w->renderText ( 10, 20, QString ("test") ); //incompatible overlay .
		QString t;

		GLuint time,fragments;
		GL_CHECK_ERROR();
	
		GLint available=0;


		while(available==0){ 
			glGetQueryObjectiv(queries[count * 2  -1 ], GL_QUERY_RESULT_AVAILABLE, &available);
			//usleep(10);
			//qDebug() << "wait";
			}

		for(int i = 0; i < count;i++){
			
			glGetQueryObjectuivARB (queries[i * 2], GL_QUERY_RESULT, &fragments);//GL_CHECK_ERROR();
			glGetQueryObjectuivARB (queries[i * 2 + 1], GL_QUERY_RESULT, &time);//GL_CHECK_ERROR();

			//qDebug() << fragments << "  " << time;

			t += QString("%1 %2ns/pixel\t  %3 µs\t %4 pixel \n").arg(i+1).arg(float(time)/float(fragments)).arg(time/1000.0).arg(fragments);
			}

		text->setText(t); 

		}
	}


