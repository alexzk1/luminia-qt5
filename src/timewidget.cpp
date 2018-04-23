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
#include "timewidget.h"



TimeWidget::TimeWidget(QWidget * parent = 0):QWidget(parent){
	layout = new QHBoxLayout();
	layout->setSpacing (0);
	layout->setMargin (0);

	play = new QPushButton(this);
	play->setFlat(true);
	play->setIcon(QIcon(":/images/xpm/start.xpm"));

	config = new QPushButton(this);
	config->setFlat(true);
	config->setIcon(QIcon(":/images/xpm/config.xpm"));
	config->setToolTip("Configure time bar");

	timeslider = new QSlider(Qt::Horizontal,this);
	timeslider->setMaximum ( 150);
	timeslider->setMinimum ( 0);
	timeslider->setValue ( 0 );
	layout->addWidget(play);
	layout->addWidget(config);
	layout->addWidget(timeslider);

	this->setLayout(layout);

	connect (config, SIGNAL(clicked()),this,SLOT (timeconfig()));
	connect (play, SIGNAL(clicked()),this,SLOT (playPressed()));
	connect (timeslider, SIGNAL(valueChanged (int)),this,SLOT (valueChanget(int)));

	playing = false;
	fps = 25;
	len = QTime ( 0, 0, 6, 0 );

	}

TimeWidget::~TimeWidget(){
	delete layout;
	delete play;
	delete config;
	delete timeslider;
	}

/*!
A smal configuration dialog
*/
void TimeWidget::timeconfig(){
	 QDialog d;

	//QWidget *vbox = new QWidget();
	QVBoxLayout vboxlayout;

	d.setLayout(&vboxlayout);

	QWidget gridwidget;
	vboxlayout.addWidget(&gridwidget);
	QGridLayout grid( &gridwidget);
	//grid.cellRect(2, column)
	//gridwidget.setLayout(&grid);

	QLabel l1("FPS",0);
	grid.addWidget(&l1,0,0);

	QSpinBox f(0);
	grid.addWidget(&f,0,1);
	f.setValue(fps);
	f.setRange(0,100);

	QLabel l2("Time",0);
	grid.addWidget(&l2,1,0);

	QTimeEdit t(len, 0);
	grid.addWidget(&t,1,1);
	t.setTime(len);
	
/*
	QCheckBox l("Loop", 0);
	l.setChecked(loop);
	grid.addWidget(&l,2,1);
*/

	QWidget buttonwidget;
	vboxlayout.addWidget(&buttonwidget);
	QHBoxLayout buttons;
	buttonwidget.setLayout(&buttons);
	
	QPushButton cancel("Cancel",0);
	connect (&cancel, SIGNAL(clicked()),&d,SLOT (reject()));
	buttons.addWidget(&cancel);

	QPushButton ok("OK",0);
	connect (&ok, SIGNAL(clicked()),&d,SLOT (accept()));
	buttons.addWidget(&ok);

	if (d.exec()==QDialog::Accepted){
		printf ("Accepted\n");

		fps = f.value();
		//loop = l.isChecked();
		len = t.time();
		timeslider->setMaximum ((len.hour ()* 3600 + len.minute ()*60 + len.second ())*fps);
		timeslider->setPageStep(fps);
		}
	}


void TimeWidget::timerEvent( QTimerEvent* ){
	timeslider->setValue( (timeslider->value() +1 ) % timeslider->maximum());
	}

/*!
signal to trigger a new frame rendering
*/
void TimeWidget::valueChanget(int in){
	emit timeChanged((double)in / (double)fps);
	}

/*!
toggles play/pause
*/
void TimeWidget::playPressed(){
	static int local_timer=0;
	
	if (!playing){
		local_timer = startTimer(1000/fps);
		playing = true;
		play->setIcon(QIcon(":/images/xpm/breaks.xpm"));
		play->setToolTip("Stop");
		}
	else{
		killTimer(local_timer);
		playing = false;
		play->setIcon(QIcon(":/images/xpm/start.xpm"));
		play->setToolTip("Run"); 
		}
	}
