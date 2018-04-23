/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for 
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** This code based on Trolltechs QSA input dialog factory.
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

#include "dialog_factory_private.h"

#include <QtGui>

namespace QS{

DockPrivate::DockPrivate(QWidget *parent) : QFrame(parent), width(0), lastWidget(0), invisibleButtonGroup(0), tabWidget(0), groupBox(0){
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	this->parent = this;

	QVBoxLayout *vbox = new QVBoxLayout(this);
	setLayout(vbox);
	//vbox->setMargin(0);
	hbox = new QHBoxLayout;
	vbox->addLayout(hbox);


	grid = new QGridLayout;
	hbox->addLayout(grid);
	grid->setAlignment(Qt::AlignTop);
 
	dock = new QDockWidget("Script Dock");
	dock->setAllowedAreas(Qt::RightDockWidgetArea); // lumina specific
	dock->setWidget(this);

	//search MainWindow
	QWidgetList l = QApplication::topLevelWidgets();
	for (int i = 0; i < l.size(); i++){
		if (QMainWindow* w = dynamic_cast<QMainWindow*>(l.at(i))) w->addDockWidget(Qt::RightDockWidgetArea, dock);
		}

	}

DockPrivate::~DockPrivate(){
	dock->deleteLater();
	}


void DockPrivate::addSpace(int space){
	QSpacerItem *spacer = new QSpacerItem(0, qMax(space,0), QSizePolicy::Fixed, QSizePolicy::Fixed);
	grid->addItem(spacer, grid->rowCount(), 1);
	invisibleButtonGroup = 0;
	}

void DockPrivate::add(Widget *widget){
	QWidget *w = widget->widget;
	int row = grid->rowCount();
	if (qobject_cast<Labeled *>(widget)) {
		QLabel *label = ((Labeled*)widget)->labelWidget;
		label->setShown(!label->text().isEmpty());
		grid->addWidget(label, row, 0);
		grid->addWidget(w, row, 1);
		}
	else{
		grid->addWidget(w, row, 0,  1, 2);
		}
	if (w && qobject_cast<QRadioButton *>(w)) {
		if (!invisibleButtonGroup)
			invisibleButtonGroup = new QButtonGroup(this);
		invisibleButtonGroup->addButton(qobject_cast<QRadioButton *>(w));
		}
	lastWidget = w;
	}

void DockPrivate::show(){
	dock->show();
	}

void DockPrivate::hide(){
	dock->hide();
	}

void DockPrivate::setTitle(const QString &title){
	dock->setWindowTitle(title);
	setWindowTitle(title);
	}

Dock::Dock(const QString &title, QWidget *parent){
	d = new DockPrivate(parent);
	widget = d;
	if (!title.isEmpty())
		setTitle(title);
	}

Dock::Dock(QObject*){
	d = new DockPrivate(NULL);
	widget = d;
	}

Dock::~Dock(){
	delete d;
	}

void Dock::setTitle(const QString &title){
	d->setTitle(title);
	}

/*!
title property
*/
QString Dock::title() const {
	return d->windowTitle();
	}

void Dock::setWidth(int width){
	d->width = width;
	}

/*!
width property
*/
int Dock::width() const {
	return d->width;
	}
/*!
void newTab(String name)\n
add a new tab with name "name"
*/
void Dock::newTab(const QString &label){
	if (!d->tabWidget) {
		d->tabWidget = new QTabWidget(d);
		int row = d->grid->rowCount();
		d->grid->addWidget(d->tabWidget, row, 0, 1, 2);
		}
	QWidget *w = new QWidget;
	d->tabWidget->addTab(w, label);
	d->parent = w;
	d->hbox = new QHBoxLayout(w);
	d->grid = new QGridLayout(0);
	d->hbox->addLayout(d->grid);
	d->grid->setAlignment(Qt::AlignTop);
	}
/*!
String currentTab()\n
returns the name of the current active Tab.
*/
QString Dock::currentTab() const{
	if (!d->tabWidget) {
		return QString("");
		}
	return d->tabWidget->tabText(d->tabWidget->currentIndex ());
	}
/*!
newColumn();\n
start a new column of widgets
*/
void Dock::newColumn(){
	if (d->grid->rowCount()) {
		d->hbox->addSpacing(17);
		d->grid = new QGridLayout(0);
		d->hbox->addLayout(d->grid);
		d->grid->setAlignment(Qt::AlignTop);
    		}
	}
/*!
addSpace(number);\n
add a Spacer
*/
void Dock::addSpace(int space){
	d->addSpace(space);
	}

/*!
add(Widget);\n
add a Widget to the dialog
*/
void Dock::add(Widget *widget){
	d->add(widget);
	}

/*!
show();\n
show the dock.
*/
void Dock::show(){
	d->show();
	}
/*!
hide();\n
hide the dock.
*/
void Dock::hide(){
	d->hide();
	}

} // namespace QS



