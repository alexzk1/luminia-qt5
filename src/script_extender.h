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

#include <QRegExp>
#include <QObject>

#ifndef _SCRIPT_EXTENDER_H
#define _SCRIPT_EXTENDER_H

class SAction{
public:
public:
    SAction(const char ** _icon, const QString& _text, const QString& _slot, const QRegExp& _filter)
    {
        icon = _icon;
        text = _text;
        slot = _slot;
        filter = _filter;
    }

    ~SAction()
    {
        //delete[] icon[0]; //xpm data
        //delete[] icon;  //xpm ptr
    }

    const char **icon;	//xpm storage
    QString text;		//Action text
    QString slot;		//Slot to call
    QRegExp filter;		//filter for the class name
};

class SEngine;

class SSlot
{
    friend class SEngine;
public:
    SSlot(const QString& _type, const QString& _signature, int _id, QRegExp _filter, const QString& _filename){
        type = _type;
        signature = _signature;
        id = _id;
        filter = _filter;
        filename = _filename;
    }


    QString type;			//slot type
    QString signature;		//Slot to call
    QRegExp filter;		//filter for the class name
    int 	id;			//callback id

    QString filename;
};

class ScriptExtender
{
public:

    static void scanFile(const QString& filename);
    static const char** xpm(const QString& in); //move to SAction later
    static void callback(QObject* obj, int id , void** args);
    static void setup();
    static QList<SAction> actionlist;
    static QList<SSlot> slotlist;
    //private:
    static QList<SEngine*> engineList;

};

#endif






