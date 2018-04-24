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

#ifndef _DIALOGFACTORYPRIVATE_H
#define _DIALOGFACTORYPRIVATE_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QSlider>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QGridLayout>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QButtonGroup>

#include "factory.h"

extern bool qsa_is_non_gui_thread();

namespace QS{

class Input : public QObject{
    Q_OBJECT
    Q_OVERRIDE(QCString name SCRIPTABLE false)
public:
    Input(QObject *parent = NULL) : QObject(parent){
        setObjectName(QLatin1String("Input"));
        }

public slots:
    QVariant getText(const QString &label = QString::null,
             const QString &text = QString::null,
             const QString &title = QString::null,
             QWidget *parent = 0);
    QVariant getNumber(const QString &label = QString::null,
               double value = 0,
               int decimals = 0,
               double minValue = -2147483647,
               double maxValue = 2147483647,
               const QString &title = QString::null,
               QWidget *parent = 0);
    QVariant getItem(const QString &label,
             const QStringList &itemList,
             const QString &currentItem = QString::null,
             bool editable = false,
             const QString &title = QString::null,
             QWidget *parent = 0);
    QVariant getItem(const QStringList &itemList,
              const QString &currentItem = QString::null,
              bool editable = false,
              QWidget *parent = 0) {
        return getItem(QString::null, itemList, currentItem, editable, QString::null, parent);
        }
    };



class FileDialog : public QObject{
    Q_OBJECT
    Q_OVERRIDE(QCString name SCRIPTABLE false)

public:
    FileDialog(QObject *parent = NULL) : QObject(parent){
        setObjectName(QLatin1String("FileDialog"));
        }

public slots:
    QVariant getOpenFileName(const QString &filter = QString::null,
                  const QString &title = QString::null,
                  QWidget *parent = 0);
    QVariant getSaveFileName(const QString &filter = QString::null,
                  const QString &title = QString::null,
                  QWidget *parent = 0);
    QVariant getExistingDirectory(const QString &dir = QString::null,
                   const QString &title = QString::null,
                   QWidget *parent = 0);
    QStringList getOpenFileNames(const QString &dir = QString::null,
                  const QString &filter = QString::null,
                  const QString &title = QString::null,
                  QWidget *parent = 0);
private:
    QString cap;
    };


class MessageBox : public QObject{
    Q_OBJECT
    Q_OVERRIDE(QCString name SCRIPTABLE false)
    Q_ENUMS(ButtonType)
public:
    enum ButtonType { NoButton, Ok, Cancel, Yes, No, Abort, Retry, Ignore };

    MessageBox(QObject *parent = NULL) : QObject(parent){
        setObjectName(QLatin1String("MessageBox"));
        }

public slots:
    int information(const QString &text,
             ButtonType button0 = Ok,
             ButtonType button1 = NoButton,
             ButtonType button2 = NoButton,
             const QString &title = QString::null,
             QWidget *parent = 0);

    int warning(const QString &text,
         ButtonType button0 = Retry,
         ButtonType button1 = Abort,
         ButtonType button2 = NoButton,
         const QString &title = QString::null,
         QWidget *parent = 0);

    int critical(const QString &text,
          ButtonType button0 = Retry,
          ButtonType button1 = NoButton,
          ButtonType button2 = NoButton,
          const QString &title = QString(),
          QWidget *parent = 0);

private:
    QString cap;
    };



class Widget : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString whatsThis READ whatsThis WRITE setWhatsThis)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(bool enabled READ isEnabled() WRITE setEnabled)
    Q_OVERRIDE(QCString name SCRIPTABLE false)
public:
    Widget(QWidget *w = 0) : widget(w) {}
    Widget(QObject*) : widget(NULL) {}
    QWidget *widget;
    QString whatsThis() const;
    void setWhatsThis(const QString &text) const;
    QString toolTip() const;
    void setToolTip(const QString &text) const;

    bool isEnabled(){
        return widget->isEnabled();
        }
    void setEnabled(bool e){
        widget->setEnabled(e);
        }

    };


#define QS_WIDGET(Class) \
 inline Q##Class *d() const { return (Q##Class*)widget; }

class Label : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    inline QLabel *d() const { return (QLabel *) widget; }
public:
    Label(QObject* = NULL) : Widget(new QLabel(0)) {}
    QString text() const{
        return d()->text();
        }
    void setText(const QString &txt){
        d()->setText(txt);
        }
    };

class Button : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    inline QPushButton *d() const { return (QPushButton *) widget; }
public:
    Button(QObject *) : Widget(new QPushButton(0)) {
        connect(d(), SIGNAL(clicked()), this, SLOT(_clicked()));
        }
    QString text() const{
        return d()->text();
        }
    void setText(const QString &txt){
        d()->setText(txt);
        }
private slots:
    void _clicked(){
        emit clicked();
        }
signals:
        void clicked();
    };


class Labeled : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel)
public:
    Labeled(const QString& label, QWidget *w);
    inline QString label() const {
        return labelWidget->text();
        }
    void setLabel(const QString &txt);
    QLabel *labelWidget;
    };

class LineEdit : public Labeled{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    QS_WIDGET(LineEdit)
public:
    LineEdit(QObject* = NULL) : Labeled(Input::tr("Text:"),  new QLineEdit(0)) {
        connect(d(), SIGNAL( returnPressed ()), this, SLOT(_returnPressed()));
        }
    QString text() const{
        return d()->text();
        }
    void setText(const QString &txt){
        d()->setText(txt);
        }
private slots:
    void _returnPressed(){
        emit returnPressed();
        }
signals:
        void returnPressed();
    };

class NumberEdit : public Labeled{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(double decimals READ decimals WRITE setDecimals)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    QS_WIDGET(LineEdit)
public:
    NumberEdit(QObject* = NULL) : Labeled(Input::tr("Value:"), new QLineEdit(0)), validator(new QDoubleValidator(widget)){
        d()->setValidator(validator);
        validator->setDecimals(0);
        connect(d(), SIGNAL( returnPressed ()), this, SLOT(_returnPressed()));
        }
    double value() const{
        return d()->text().toDouble();
        }
    void setValue(double val){
        d()->setText(QString::number(val));
        }
    double decimals() const{
        return validator->decimals();
        }
    void setDecimals(double decimals){
        validator->setDecimals(int(decimals));
        }
    double minimum() const {
        return validator->bottom();
        }
    void setMinimum(double minimum){
        validator->setBottom(minimum);
        }
    double maximum() const{
        return validator->top();
        }
    void setMaximum(double maximum){
        validator->setBottom(maximum);
        }
private slots:
    void _returnPressed(){
        emit returnPressed();
        }
signals:
        void returnPressed();
private:
    QDoubleValidator *validator;
    };


class Slider : public Labeled{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    QS_WIDGET(Slider)
public:
    Slider(QObject *) : Labeled(Input::tr("Value:"),new QSlider (Qt::Horizontal)) {
        connect(d(), SIGNAL(valueChanged(int)), this, SLOT(_changed(int)));
        }
    int value() const{
        return d()->value();
        }
    void setValue(int val){
        d()->setValue(val);
        }
    int minimum() const {
        return d()->minimum();
        }
    void setMinimum(int min){
        d()->setMinimum(min);
        }
    int maximum() const{
        return d()->maximum();
        }
    void setMaximum(int max){
        d()->setMaximum(max);
        }
private slots:
    void _changed(int){
        emit changed();
        }
signals:
        void changed();
    };


class DateEditEnums : public QObject{
    Q_OBJECT
    Q_ENUMS(Order)
    Q_OVERRIDE(QCString name SCRIPTABLE false)
public:
    enum Order { DMY, MDY, YMD, YDM };
    };


class DateEdit : public Labeled{
    Q_OBJECT
    Q_ENUMS(Order)
    Q_PROPERTY(int order READ order WRITE setOrder)
    Q_PROPERTY(QDate date READ date WRITE setDate)
    Q_PROPERTY(QDate minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(QDate maximum READ maximum WRITE setMaximum)
    QS_WIDGET(DateEdit)
public:
    enum Order { DMY, MDY, YMD, YDM };
    DateEdit(QObject* = NULL) : Labeled(tr("Date:"), new QDateEdit(0)){ }

    int order() const{
        QString format = d()->displayFormat();
        if (format == "dd.MM.yyyy") return DMY;
        else if (format == "MM.dd.yyyy") return MDY;
        else if (format == "yyyy.MM.dd") return YMD;
        else if (format == "yyyy.dd.MM") return YDM;
        return -1;;
        }

    void setOrder(int order){
        if (order == DMY) d()->setDisplayFormat("dd.MM.yyyy");
        else if (order == MDY) d()->setDisplayFormat("MM.dd.yyyy");
        else if (order == YMD) d()->setDisplayFormat("yyyy.MM.dd");
        else if (order == YDM) d()->setDisplayFormat("yyyy.dd.MM");
        }

    QDate date() const {
        return d()->date();
        }
    void setDate(QDate date) {
        d()->setDate(date);
        }
    QDate minimum() const {
        return d()->minimumDate();
        }
    void setMinimum(const QDate &minimum) {
        d()->setMinimumDate(minimum);
        }
    QDate maximum() const {
        return d()->maximumDate();
        }
    void setMaximum(const QDate &maximum) {
        d()->setMaximumDate(maximum);
        }
    };

class TimeEdit : public Labeled{
    Q_OBJECT
    Q_PROPERTY(bool showSeconds READ seconds WRITE setSeconds)
    Q_PROPERTY(bool showAMPM READ ampm WRITE setAmpm)
    Q_PROPERTY(QTime time READ time WRITE setTime)
    Q_PROPERTY(QTime minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(QTime maximum READ maximum WRITE setMaximum)
    QS_WIDGET(TimeEdit)
public:
    TimeEdit(QObject* = NULL) : Labeled(tr("Time:"), new QTimeEdit(0)){ }

    void updateDisplayFormat(bool ampm, bool secs){
            QString baseFormat = "HH::mm";
        if (ampm)
        baseFormat = "AP" + baseFormat;
        if (secs)
        baseFormat += ":ss";
        d()->setDisplayFormat(baseFormat);
        }

    bool seconds() const {
        return d()->displayFormat().contains("ss");
        }

    void setSeconds(bool b) {
        updateDisplayFormat(ampm(), b);
        }

    bool ampm() const {
        return d()->displayFormat().toLower().contains("ap");
        }

    void setAmpm(bool b) {
        updateDisplayFormat(b, seconds());
        }

    QTime time() const {
        return d()->time();
        }

    void setTime(QTime time) {
        d()->setTime(time);
        }

    QTime minimum() const {
        return d()->minimumTime();
        }

    void setMinimum(const QTime &minimum) {
        d()->setMinimumTime(minimum);
        }

    QTime maximum() const {
        return d()->maximumTime();
        }
    void setMaximum(const QTime &maximum){
        d()->setMaximumTime(maximum);
        }
    };

class TextEdit : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    QS_WIDGET(TextEdit)
public:
    TextEdit(QObject* = NULL) : Widget(new QTextEdit(0)){ }
    QString text() const{
        return d()->toPlainText();
        }
    void setText(const QString &txt){
        d()->setPlainText(txt);
        }
    bool isReadOnly(){
        return d()->isReadOnly();
        }
    void setReadOnly (bool r){
        d()->setReadOnly (r);
        }
public slots:
    void append(const QString &txt, int r = 0, int g = 0, int b = 0){
        d()->setTextColor (QColor(r,g,b));
        d()->append(txt);
        d()->setTextColor(QColor(Qt::black));
        }
    };

class SpinBox : public Labeled{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    QS_WIDGET(SpinBox)
public:
    SpinBox(QObject* = NULL) : Labeled(Input::tr("Value:"), new QSpinBox(0)) {}
    int value() const{
        return d()->value(); }
    void setValue(int val){
        d()->setValue(val);
        }
    int minimum() const{
        return d()->value();
        }
    void setMinimum(int minimum){
        d()->setMinimum(minimum);
        }
    int maximum() const{
        return d()->value();
        }
    void setMaximum(int maximum){
        d()->setMaximum(maximum);
        }
    };

class CheckBox : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    QS_WIDGET(CheckBox)
public:
    CheckBox(QObject* = NULL) : Widget(new QCheckBox(0)) {
        connect(d(), SIGNAL(clicked()), this, SLOT(_clicked()));
        }
    QString text() const{
        return d()->text();
        }
    void setText(const QString &txt){
        d()->setText(txt);
        }
    bool isChecked() const{
        return d()->isChecked();
        }
    void setChecked(bool b){
        d()->setChecked(b);
        }
private slots:
    void _clicked(){
        emit clicked();
        }
signals:
        void clicked();
    };


class RadioButton : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    QS_WIDGET(RadioButton)
public:
    RadioButton(QObject* = NULL) : Widget(new QRadioButton(0)) {
        connect(d(), SIGNAL(clicked()), this, SLOT(_clicked()));
        }
    QString text() const{
        return d()->text();
        }
    void setText(const QString &txt){
        d()->setText(txt);
        }

    bool isChecked() const{
        return d()->isChecked();
        }
    void setChecked(bool b){
        d()->setChecked(b);
        }
private slots:
    void _clicked(){
        emit clicked();
        }
signals:
        void clicked();
    };

class ComboBox : public Labeled{
    Q_OBJECT
    Q_PROPERTY(QStringList itemList READ itemList WRITE setItemList)
    Q_PROPERTY(QString currentItem READ currentItem WRITE setCurrentItem)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    QS_WIDGET(ComboBox)
public:
    ComboBox(QObject* = NULL) : Labeled(Input::tr("Item:"), new QComboBox(0)){
        setEditable(false);
        connect(d(), SIGNAL(activated(int)), this, SLOT(changevalue(int)));
        }

    QString currentItem() const{
        return d()->currentText();
        }

    void setCurrentItem(const QString &txt){
        int idx = d()->findText(txt);
        if (idx >= 0)
            d()->setCurrentIndex(idx);
        }

    bool isEditable() const{
        return d()->isEditable();
        }

    void setEditable(bool b){
        d()->setEditable(b);
        }

    QStringList itemList() const;

    void setItemList(const QStringList &l){
        d()->clear();
        d()->insertItems(0, l);
        }
public slots:
    void changevalue(int){
        emit changed();
        }
signals:
        void changed();
    };



class GroupBox : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    QS_WIDGET(GroupBox)
public:
    GroupBox(QObject* = NULL);
    QString title() const{
        return d()->title();
        }
    void setTitle(const QString &title){
        d()->setTitle(title);
        }
public slots:
    void add(Widget *widget);
    void add(QObject* w){
        if (Widget *widget = dynamic_cast<Widget*>(w))add(widget);
        }
    void newColumn();
    void addSpace(int space);
private:
    QButtonGroup *invisibleButtonGroup;
    QHBoxLayout *hbox;
    QGridLayout *grid;
    };

class DialogPrivate;
class Dialog : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(QString okButtonText READ okButtonText WRITE setOkButtonText)
    Q_PROPERTY(QString cancelButtonText READ cancelButtonText WRITE setCancelButtonText)
    DialogPrivate *d;
public:
    Dialog(QObject *);
    Dialog(const QString &caption = QString::null, QWidget *parent=0);
    ~Dialog();

    void setTitle(const QString &title);
    QString title() const;

    void setWidth(int width);
    int width() const;

    void setOkButtonText(const QString &text);
    QString okButtonText() const;

    void setCancelButtonText(const QString &text);
    QString cancelButtonText() const;

public slots:
    void newTab(const QString &label);
    void newColumn();
    void addSpace(int space);
    void add(Widget *widget);
    void add(QObject* w){
        if (Widget *widget = dynamic_cast<Widget*>(w))add(widget);
        }
    bool exec();
    void show();
    };

class DialogPrivate : public QDialog{
public:
    DialogPrivate(QWidget *parent=0);
    int width;
    QSize sizeHint() const { return QDialog::sizeHint().expandedTo(QSize(width, 0)); }
    QWidget *lastWidget;

    QHBoxLayout *hbox;
    QGridLayout *grid;

    QButtonGroup *invisibleButtonGroup;
    QTabWidget *tabWidget;
    QGroupBox *groupBox;

    QHBoxLayout *buttonRow;
    void fixTabOrder();

    QPushButton *okButton, *cancelButton;
    QWidget *parent;

    void addSpace(int space);
    void add(Widget *widget);
    };



class DockPrivate;
class Dock : public Widget{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(int width READ width WRITE setWidth)
    DockPrivate *d;
public:
    Dock(QObject *);
    Dock(const QString &caption = QString::null, QWidget *parent=0);
    ~Dock();

    void setTitle(const QString &title);
    QString title() const;

    void setWidth(int width);
    int width() const;

public slots:
    void newTab(const QString &label);
    QString currentTab() const; // new
    void newColumn();
    void addSpace(int space);
    void add(Widget *widget);
    void add(QObject* w){
        if (Widget *widget = dynamic_cast<Widget*>(w))add(widget);
        }
    void hide();
    void show();
    };



//class DockPrivate : public QDialog{
class DockPrivate : public QFrame{
public:
    DockPrivate(QWidget *parent=0);
    virtual ~DockPrivate();
    int width;
    //QSize sizeHint() const { return QDialog::sizeHint().expandedTo(QSize(width, 0)); }
    QSize sizeHint() const { return QWidget::sizeHint().expandedTo(QSize(width, 0)); }
    QWidget *lastWidget;

    QHBoxLayout *hbox;
    QGridLayout *grid;

    QButtonGroup *invisibleButtonGroup;
    QTabWidget *tabWidget;
    QGroupBox *groupBox;

    QHBoxLayout *buttonRow;

    QWidget *parent;
    QDockWidget *dock;

    void addSpace(int space);
    void add(Widget *widget);
    virtual void show();
    virtual void hide();
    void setTitle(const QString &title);
    };

} //namespace QS

#endif
