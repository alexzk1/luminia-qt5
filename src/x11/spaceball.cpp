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
#include "spaceball.h"

#include <QX11Info>
#include <iostream>

//#undef QApplication
#define INVALID_EVENT_TYPE	-1


QXiApplication::QXiApplication(int & argc, char **argv) : QApplication(argc,argv){
	motion_type = INVALID_EVENT_TYPE;
	button_press_type = INVALID_EVENT_TYPE;
	button_release_type = INVALID_EVENT_TYPE;
	//first = true;
	int num_devices;
	XDeviceInfo *devices = XListInputDevices(QX11Info::display(), &num_devices);
	for(int i=0; i < num_devices; i++) {
		std::cout << devices[i].id << " " << devices[i].name  << (char*)((devices[i].use == IsXExtensionDevice) ? "XExtensionDevice" : ((devices[i].use == IsXPointer) ? "XPointer" : "XKeyboard")) << std::endl;

		XAnyClassPtr any = (XAnyClassPtr) (devices[i].inputclassinfo);
		int buttons = 0;
		int axes = 0;

		XValuatorInfoPtr v;
		XButtonInfoPtr b;

		for (int c = 0; c < devices[i].num_classes; c++){
			switch (any->c_class){
				case ButtonClass:
					b = (XButtonInfoPtr) any;
					buttons = b->num_buttons;
					std::cout << "   Buttons: " << b->num_buttons << std::endl;
					
					break;
				case ValuatorClass:
					v =(XValuatorInfoPtr) any;
					axes = v->num_axes;
					std::cout << "   Axes: " << (int)v->num_axes << std::endl;
					break;
				}
				any = (XAnyClassPtr) ((char *) any + any->length);
			}

		if (axes == 6 && buttons != 0){
			std::cout << "   Registering 6 DOF device" << std::endl;
			Register_6dof(&devices[i]);
			} 
		}
	XFreeDeviceList (devices);
	}


QXiApplication::~QXiApplication(){
	for (int i = 0; i < devices.size(); i++){
		XCloseDevice(QX11Info::display(), devices.at(i));
		}
	}


void QXiApplication::Register_6dof(XDeviceInfo *info){
	int number = 0;
	XEventClass event_list[7];
	XDevice *device = XOpenDevice(QX11Info::display(), info->id);
	if (!device) return;
	int i;
	XInputClassInfo *ip = device->classes;
	for (i=0; i < info->num_classes && i < 7; ip++, i++) {
		switch (ip->input_class) {
			case ButtonClass:
				DeviceButtonPress(device, button_press_type, event_list[number]);
				number++;
				DeviceButtonRelease(device,button_release_type , event_list[number]); 					number++;
				break;
			case ValuatorClass:
				DeviceMotionNotify(device, motion_type, event_list[number]);
				number++;
				break;
				}
			}
	if (XSelectExtensionEvent(QX11Info::display(), QX11Info::appRootWindow(), event_list, number)) {
		std::cout << "error selecting extended events" << std::endl;
		return;
		}
	devices << device;

	}

bool QXiApplication::x11EventFilter ( XEvent * e ){
	
	int axes[7];
	if (e->type == motion_type) {
		XDeviceMotionEvent *motion = (XDeviceMotionEvent *) e;
		int axes[7];
		axes[6]=0;

		for(int i=0; i < motion->axes_count; i++) {
			//if (last[i] != motion->axis_data[i]){
			axes[i] = motion->axis_data[i]-last[i];
			last[i] = motion->axis_data[i];
			//}	
			//std::cout << "   a[" << motion->first_axis + i<< "]=" << axes[i];		
			}
		emit spaceballEvent(axes);
		//first = false;
		//std::cout << std::endl;
		return true;
		}
	else if ((e->type == button_press_type) ||(e->type == button_release_type)) {
		XDeviceButtonEvent *button = (XDeviceButtonEvent *) e;

		std::cout << "button " << (char*)((e->type == button_release_type) ? "release" : "press  ") <<  button->button;
		std::cout << std::endl;
		
		for(int i = 0; i < 6;i++)axes[i]=0;
		axes[6]= button->button;
		emit spaceballEvent(axes);

		return true;
		}

	//return false;
	return QApplication::x11EventFilter(e);
	}

//*/



