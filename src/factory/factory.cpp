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
#include "file_factory_private.h"
#include "network_factory_private.h"
#include "glwidget_factory_private.h"
#include "colorwidget_factory_private.h"
#include "factory.h"

using namespace QS;

#ifndef QTSCRIPT

#include <qsinterpreter.h>



Factory::Factory(){

    registerClass(QString::fromLatin1("Dialog"), &Dialog::staticMetaObject);
    registerClass(QString::fromLatin1("Dock"), &Dock::staticMetaObject);

    registerClass(QString::fromLatin1("Label"), &Label::staticMetaObject);
    registerClass(QString::fromLatin1("Button"), &Label::staticMetaObject);

    registerClass(QString::fromLatin1("LineEdit"), &LineEdit::staticMetaObject);
    registerClass(QString::fromLatin1("NumberEdit"), &NumberEdit::staticMetaObject);
    registerClass(QString::fromLatin1("Slider"), &Slider::staticMetaObject);
    registerClass(QString::fromLatin1("DateEdit"), &DateEdit::staticMetaObject, new DateEditEnums);
    registerClass(QString::fromLatin1("TimeEdit"), &TimeEdit::staticMetaObject);
    registerClass(QString::fromLatin1("TextEdit"), &TextEdit::staticMetaObject);
    registerClass(QString::fromLatin1("SpinBox"), &SpinBox::staticMetaObject);
    registerClass(QString::fromLatin1("CheckBox"), &CheckBox::staticMetaObject);
    registerClass(QString::fromLatin1("RadioButton"), &RadioButton::staticMetaObject);
    registerClass(QString::fromLatin1("ComboBox"), &ComboBox::staticMetaObject);
    registerClass(QString::fromLatin1("GroupBox"), &GroupBox::staticMetaObject);
    registerClass(QString::fromLatin1("MessageBox"), 0, new MessageBox());
    registerClass(QString::fromLatin1("FileDialog"), 0, new FileDialog());
    registerClass(QString::fromLatin1("Input"), 0, new Input());

    registerClass(QString::fromLatin1("File"), &File::staticMetaObject);
    registerClass(QString::fromLatin1("UdpServer"), &UdpServer::staticMetaObject);
    registerClass(QString::fromLatin1("TcpClient"), &TcpClient::staticMetaObject);

    registerClass(QString::fromLatin1("GLWidget"), &GLWidget::staticMetaObject);
    registerClass(QString::fromLatin1("ColorWidget"), &GLWidget::staticMetaObject);
}

QObject *Factory::create(const QString &className,const QVariantList &arguments, QObject * /*context*/){

    if (className == QString::fromLatin1("Dialog")) {
        QString title = arguments.size() >= 1 ? qvariant_cast<QString>(arguments[0]) : QString();
        QObject *obj = arguments.size() >= 2 ? qvariant_cast<QObject *>(arguments[1]) : 0;
        QWidget *widget = obj && qobject_cast<QWidget *>(obj) ? (QWidget*) obj : 0;
        return new Dialog(title, widget);
    }
    else if (className == QString::fromLatin1("Dock")) {
        QString title = arguments.size() >= 1 ? qvariant_cast<QString>(arguments[0]) : QString();
        QObject *obj = arguments.size() >= 2 ? qvariant_cast<QObject *>(arguments[1]) : 0;
        QWidget *widget = obj && qobject_cast<QWidget *>(obj) ? (QWidget*) obj : 0;
        return new Dock(title, widget);
    }
    else if (className == QString::fromLatin1("Label")){
        return new Label;
    }
    else if (className == QString::fromLatin1("Button")){
        return new Label;
    }
    else if (className == QString::fromLatin1("LineEdit")) {
        return new LineEdit;
    }
    else if (className == QString::fromLatin1("NumberEdit")) {
        return new NumberEdit;
    }
    else if (className == QString::fromLatin1("Slider")) {
        return new Slider;
    }
    else if (className == QString::fromLatin1("DateEdit")) {
        return new DateEdit;
    }
    else if (className == QString::fromLatin1("TimeEdit")) {
        return new TimeEdit;
    }
    else if (className == QString::fromLatin1("TextEdit")) {
        return new TextEdit;
    }
    else if (className == QString::fromLatin1("SpinBox")) {
        return new SpinBox;
    }
    else if (className == QString::fromLatin1("CheckBox")) {
        return new CheckBox;
    }
    else if (className == QString::fromLatin1("RadioButton")) {
        return new RadioButton;
    }
    else if (className == QString::fromLatin1("ComboBox")) {
        return new ComboBox;
    }
    else if (className == QString::fromLatin1("GroupBox")) {
        return new GroupBox;
    }

    else if (className == QString::fromLatin1("File")) {
        if (arguments.size() == 0) {
            interpreter()->throwError(QString::fromLatin1("Cannot construct File, missing argument filename"));
            return 0;
        }
        QVariant fn = arguments[0];
        if (fn.type() != QVariant::String) {
            interpreter()->throwError(QString::fromLatin1("Cannot construct File, Filename must be of type string"));
            return 0;
        }
        return new File(qvariant_cast<QString>(arguments[0]));
    }
    else if (className == QString::fromLatin1("UdpServer")) {
        if (arguments.size() == 0) {
            interpreter()->throwError(QString::fromLatin1("Cannot construct UdpServer, missing argument port"));
            return 0;
        }
        return new UdpServer(qvariant_cast<QString>(arguments[0]));
    }
    else if (className == QString::fromLatin1("TcpClient")) {
        if (arguments.size() == 0) {
            interpreter()->throwError(QString::fromLatin1("Cannot construct TcpClient, missing argument hostname:port"));
            return 0;
        }
        return new UdpServer(qvariant_cast<QString>(arguments[0]));
    }

    else if (className == QString::fromLatin1("GLWidget")) {
        return new GLWidget;
    }

    else if (className == QString::fromLatin1("ColorWidget")) {
        return new ColorWidget;
    }

    return 0;
}

#endif

#ifdef QTSCRIPT
#include <qscriptengine.h>

//dialog factory prototype only
Q_SCRIPT_DECLARE_QMETAOBJECT(Dialog, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(Dock, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(Label, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(Button, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(LineEdit, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(NumberEdit, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(Slider, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(DateEdit, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(TimeEdit, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(TextEdit, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(SpinBox, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(CheckBox, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(RadioButton, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(ComboBox, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(GroupBox, QObject*);

//object and prototype
Q_SCRIPT_DECLARE_QMETAOBJECT(QS::MessageBox, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(FileDialog, QObject*);
Q_SCRIPT_DECLARE_QMETAOBJECT(Input, QObject*);



//file factory
Q_SCRIPT_DECLARE_QMETAOBJECT(File, QString);
//network factory
Q_SCRIPT_DECLARE_QMETAOBJECT(UdpServer, QString);
Q_SCRIPT_DECLARE_QMETAOBJECT(TcpClient, QString);

Q_SCRIPT_DECLARE_QMETAOBJECT(GLWidget, QObject*);

Q_SCRIPT_DECLARE_QMETAOBJECT(ColorWidget, QObject*);

void Factory::Factory(QScriptEngine &eng)
{
    //dialog factory
    eng.globalObject().setProperty("Dialog" ,eng.scriptValueFromQMetaObject<Dialog>());
    eng.globalObject().setProperty("Dock" ,eng.scriptValueFromQMetaObject<Dock>());
    eng.globalObject().setProperty("Label" ,eng.scriptValueFromQMetaObject<Label>());
    eng.globalObject().setProperty("Button" ,eng.scriptValueFromQMetaObject<Button>());

    eng.globalObject().setProperty("LineEdit" ,eng.scriptValueFromQMetaObject<LineEdit>());
    eng.globalObject().setProperty("NumberEdit" ,eng.scriptValueFromQMetaObject<NumberEdit>());
    eng.globalObject().setProperty("Slider" ,eng.scriptValueFromQMetaObject<Slider>());
    eng.globalObject().setProperty("DateEdit" ,eng.scriptValueFromQMetaObject<DateEdit>());
    eng.globalObject().setProperty("TimeEdit" ,eng.scriptValueFromQMetaObject<TimeEdit>());
    eng.globalObject().setProperty("TextEdit" ,eng.scriptValueFromQMetaObject<TextEdit>());
    eng.globalObject().setProperty("SpinBox" ,eng.scriptValueFromQMetaObject<SpinBox>());
    eng.globalObject().setProperty("CheckBox" ,eng.scriptValueFromQMetaObject<CheckBox>());
    eng.globalObject().setProperty("RadioButton" ,eng.scriptValueFromQMetaObject<RadioButton>());
    eng.globalObject().setProperty("ComboBox" ,eng.scriptValueFromQMetaObject<ComboBox>());
    eng.globalObject().setProperty("GroupBox"  ,eng.scriptValueFromQMetaObject<GroupBox>());

    QScriptValue sv;

    sv = eng.newQObject(new QS::MessageBox(),QScriptEngine::ScriptOwnership);
    sv.setPrototype(eng.scriptValueFromQMetaObject<QS::MessageBox>());
    eng.globalObject().setProperty("MessageBox", sv);

    sv = eng.newQObject(new FileDialog(),QScriptEngine::ScriptOwnership);
    sv.setPrototype(eng.scriptValueFromQMetaObject<FileDialog>());
    eng.globalObject().setProperty("FileDialog", sv);

    sv = eng.newQObject(new Input(),QScriptEngine::ScriptOwnership);
    sv.setPrototype(eng.scriptValueFromQMetaObject<Input>());
    eng.globalObject().setProperty("Input" , sv);

    //file factory
    eng.globalObject().setProperty("File" ,eng.scriptValueFromQMetaObject<File>());
    eng.globalObject().setProperty("UdpServer" ,eng.scriptValueFromQMetaObject<UdpServer>());
    eng.globalObject().setProperty("TcpClient" ,eng.scriptValueFromQMetaObject<TcpClient>());

    eng.globalObject().setProperty("GLWidget" ,eng.scriptValueFromQMetaObject<GLWidget>());
    eng.globalObject().setProperty("ColorWidget" ,eng.scriptValueFromQMetaObject<ColorWidget>());

}

#endif

const QMetaObject* Factory::metaObjectFrom(QString classname)
{
    if      (classname == "Dialog")		return &Dialog::staticMetaObject;
    else if (classname == "Dock")		return &Dock::staticMetaObject;
    else if (classname == "Label")		return &Label::staticMetaObject;
    else if (classname == "Button")		return &Button::staticMetaObject;

    else if (classname == "LineEdit") 	return &LineEdit::staticMetaObject;
    else if (classname == "NumberEdit")	return &NumberEdit::staticMetaObject;
    else if (classname == "Slider")		return &Slider::staticMetaObject;
    else if (classname == "DateEdit")	return &DateEdit::staticMetaObject;
    else if (classname == "TimeEdit")	return &TimeEdit::staticMetaObject;
    else if (classname == "TextEdit")	return &TextEdit::staticMetaObject;
    else if (classname == "SpinBox")	return &SpinBox::staticMetaObject;
    else if (classname == "CheckBox")	return &CheckBox::staticMetaObject;
    else if (classname == "RadioButton")	return &RadioButton::staticMetaObject;
    else if (classname == "ComboBox")	return &ComboBox::staticMetaObject;
    else if (classname == "GroupBox")	return &GroupBox::staticMetaObject;

    else if (classname == "File")		return &File::staticMetaObject;
    else if (classname == "UdpServer")	return &UdpServer::staticMetaObject;
    else if (classname == "TcpClient")	return &TcpClient::staticMetaObject;

    else if (classname == "GLWidget")	return &GLWidget::staticMetaObject;
    else if (classname == "ColorWidget")	return &ColorWidget::staticMetaObject;
    return nullptr;
}
