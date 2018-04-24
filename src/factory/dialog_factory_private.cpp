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

namespace QS{

    QVariant Input::getText(const QString &label, const QString &text, const QString &title, QWidget *parent){
        bool ok;
        QString lab = label;
        if (lab.isEmpty())
            lab = tr("Text:");
        QString res = QInputDialog::getText(parent ? parent : qApp->activeWindow(), title, lab, QLineEdit::Normal, text, &ok);
        return (ok ? QVariant(res) : QVariant());
    }

    QStringList ComboBox::itemList() const{
        QStringList l;
        for (int i = 0; i < d()->count(); ++i)
            l << d()->itemText(i);
        return l;
    }


    QVariant Input::getNumber(const QString &label, double value, int decimals, double minValue, double maxValue, const QString &title, QWidget *parent){
        QString lab = label;
        if (lab.isEmpty())
            lab = tr("Number:");
        bool ok;
        if (decimals > 0){
            double d = QInputDialog::getDouble(parent ? parent : qApp->activeWindow(), title, lab, value, minValue, maxValue, decimals, &ok);
            return (ok ? QVariant(d) : QVariant());
        }
        int i = QInputDialog::getInt(parent ? parent : qApp->activeWindow(), title, lab, int(value), int(minValue), int(maxValue), 1, &ok);
        return (ok ? QVariant(i) : QVariant());
    }


    QVariant Input::getItem(const QString &label, const QStringList &itemList, const QString &currentItem, bool editable, const QString &title, QWidget *parent){
        QString lab = label;
        if (lab.isEmpty())
            lab = tr("Item:");
        int idx = 0;
        if (!currentItem.isEmpty())
            idx = itemList.indexOf(currentItem);
        bool ok;
        QString res = QInputDialog::getItem(parent ? parent : qApp->activeWindow(), title, lab, itemList, qMax(idx,0), editable, &ok);
        return (ok ?  QVariant(res) : QVariant());
    }

    QVariant FileDialog::getOpenFileName(const QString &filter, const QString &title, QWidget *parent){
        QString str = QFileDialog::getOpenFileName(parent ? parent : qApp->activeWindow(), title, QString::null, filter, 0);
        return str.isNull() ? QVariant() : QVariant(str);
    }

    QVariant FileDialog::getSaveFileName(const QString &filter, const QString &title, QWidget *parent){
        QString str = QFileDialog::getSaveFileName(parent ? parent : qApp->activeWindow(), title, QString::null, filter, 0);
        return str.isNull() ? QVariant() : QVariant(str);
    }

    QVariant FileDialog::getExistingDirectory(const QString &dir, const QString &title, QWidget *parent){
        QString str =  QFileDialog::getExistingDirectory(parent ? parent : qApp->activeWindow(), title, dir, 0);
        return str.isNull() ? QVariant() : QVariant(str);
    }

    QStringList FileDialog::getOpenFileNames(const QString &dir, const QString &filter, const QString &title, QWidget *parent){
        return QFileDialog::getOpenFileNames(parent ? parent : qApp->activeWindow(), title, filter, dir, 0);
    }


    int MessageBox::information(const QString &text, ButtonType button0, ButtonType button1, ButtonType button2, const QString &title, QWidget *parent){
        return QMessageBox::information(parent ? parent : qApp->activeWindow(), title, text, button0, button1, button2);
    }

    /*
MessageBox::ButtonType MessageBox::question(const QString &text, ButtonType button0, ButtonType button1, ButtonType button2){
    return (MessageBox::ButtonType) QMessageBox::question(qApp->activeWindow(), cap, text, button0, button1, button2);
    }*/

    int MessageBox::warning(const QString &text, ButtonType button0, ButtonType button1, ButtonType button2, const QString &title, QWidget *parent){
        return QMessageBox::warning(parent ? parent : qApp->activeWindow(), title, text, button0, button1, button2);
    }

    int MessageBox::critical(const QString &text, ButtonType button0, ButtonType button1, ButtonType button2, const QString &title, QWidget *parent){
        return QMessageBox::critical(parent ? parent : qApp->activeWindow(), title, text, button0, button1, button2);
    }



    QString Widget::whatsThis() const{
        return widget->whatsThis();
    }

    void Widget::setWhatsThis(const QString &text) const{
        widget->setWhatsThis(text);
    }

    QString Widget::toolTip() const{
        return widget->toolTip();
    }

    void Widget::setToolTip(const QString &text) const{
        widget->setToolTip(text);
    }


    void Labeled::setLabel(const QString &txt){
        labelWidget->setText(txt);
        if(txt.isEmpty())
            labelWidget->hide();
    }

    Labeled::Labeled(const QString& label, QWidget *w): Widget(w), labelWidget(new QLabel(label,0)){
        labelWidget->setBuddy(w);
    }


    GroupBox::GroupBox(QObject *) : Widget(new QGroupBox), invisibleButtonGroup(0){
        hbox = new QHBoxLayout(d());
        grid = new QGridLayout();

        hbox->addLayout(grid);
        grid->setAlignment(Qt::AlignTop);
    }

    void GroupBox::add(Widget *widget){
        QWidget *w = widget->widget;

        int row = grid->rowCount();
        if (qobject_cast<Labeled *>(widget)) {
            QLabel *label = ((Labeled*)widget)->labelWidget;
            grid->addWidget(label, row, 0);
            grid->addWidget(w, row, 1);
            label->setVisible(!label->text().isEmpty());
        }
        else {
            grid->addWidget(w, row, 0, 1, 2);
        }

        if (w && qobject_cast<QRadioButton *>(w)) {
            if (!invisibleButtonGroup)
                invisibleButtonGroup = new QButtonGroup(d());
            invisibleButtonGroup->addButton(qobject_cast<QRadioButton *>(w));
        }
    }

    void GroupBox::addSpace(int space){
        QSpacerItem *spacer = new QSpacerItem(0, qMax(space,0), QSizePolicy::Fixed, QSizePolicy::Fixed);
        grid->addItem(spacer, grid->rowCount(), 1);
        invisibleButtonGroup = 0;
    }

    void GroupBox::newColumn(){
        if (grid->rowCount()){
            hbox->addSpacing(17);
            grid = new QGridLayout;
            grid->setAlignment(Qt::AlignTop);
            hbox->addLayout(grid);
        }
    }


    DialogPrivate::DialogPrivate(QWidget *parent) : QDialog(parent), width(0), lastWidget(0), invisibleButtonGroup(0), tabWidget(0), groupBox(0){
        this->parent = this;

        QVBoxLayout *vbox = new QVBoxLayout(this);
        setLayout(vbox);

        hbox = new QHBoxLayout;
        vbox->addLayout(hbox);

        grid = new QGridLayout;
        hbox->addLayout(grid);
        grid->setAlignment(Qt::AlignTop);

        okButton = new QPushButton(QMessageBox::tr("OK"), this);
        okButton->setDefault(true);
        QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
        cancelButton = new QPushButton(QMessageBox::tr("Cancel"), this);
        QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

        buttonRow = new QHBoxLayout;
        vbox->addLayout(buttonRow);
        buttonRow->addStretch(10);
        buttonRow->addWidget(okButton);
        buttonRow->addWidget(cancelButton);
    }

    void DialogPrivate::fixTabOrder(){
        if (lastWidget) {
            if (lastWidget) {
                QWidget::setTabOrder(lastWidget, okButton);
                QWidget::setTabOrder(okButton, cancelButton);
            }
            layout()->activate();
            lastWidget = nullptr;
        }
    }

    void DialogPrivate::addSpace(int space){
        QSpacerItem *spacer = new QSpacerItem(0, qMax(space,0), QSizePolicy::Fixed, QSizePolicy::Fixed);
        grid->addItem(spacer, grid->rowCount(), 1);
        invisibleButtonGroup = nullptr;
    }

    void DialogPrivate::add(Widget *widget){
        QWidget *w = widget->widget;
        int row = grid->rowCount();
        if (qobject_cast<Labeled *>(widget)) {
            QLabel *label = ((Labeled*)widget)->labelWidget;
            label->setVisible(!label->text().isEmpty());
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

    Dialog::Dialog(const QString &title, QWidget *parent){
        d = new DialogPrivate(parent);
        widget = d;
        if (!title.isEmpty())
            setTitle(title);
    }

    Dialog::Dialog(QObject*){
        d = new DialogPrivate(NULL);
        widget = d;
    }

    Dialog::~Dialog(){
        delete d;
    }

    void Dialog::setTitle(const QString &title){
        d->setWindowTitle(title);
    }

    QString Dialog::title() const {
        return d->windowTitle();
    }

    void Dialog::setWidth(int width){
        d->width = width;
    }

    int Dialog::width() const {
        return d->width;
    }

    void Dialog::setOkButtonText(const QString &text){
        d->okButton->setText(text);
    }

    /*!
property holding the OK button text
*/
    QString Dialog::okButtonText() const {
        return d->okButton->text();
    }

    void Dialog::setCancelButtonText(const QString &text) {
        d->cancelButton->setText(text);
    }

    QString Dialog::cancelButtonText() const {
        return d->cancelButton->text();
    }

    void Dialog::newTab(const QString &label){
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

    void Dialog::newColumn(){
        if (d->grid->rowCount()) {
            d->hbox->addSpacing(17);
            d->grid = new QGridLayout(0);
            d->hbox->addLayout(d->grid);
            d->grid->setAlignment(Qt::AlignTop);
        }
    }

    void Dialog::addSpace(int space){
        d->addSpace(space);
    }

    /*!
add a Widget to the dialog
*/
    void Dialog::add(Widget *widget){
        d->add(widget);
    }

    /*!
execute the dialog. It will block the application
*/
    bool Dialog::exec(){
        d->fixTabOrder();
        int res = d->exec();
        return (res == QDialog::Accepted);
    }
    /*!
show the dialog. It will not block the application. Makes only sense for persitent toolscripts
*/
    void Dialog::show(){
        d->fixTabOrder();
        d->show();
        //for(;;)
        //int res = d->exec();
        //return (res == QDialog::Accepted);
    }

} // namespace QS

