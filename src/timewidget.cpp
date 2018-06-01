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

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QTimeEdit>
#include <QDialogButtonBox>

TimeWidget::TimeWidget(QWidget *parent): QWidget(parent)
{
    layout = new QHBoxLayout();
    layout->setSpacing (0);
    layout->setMargin (0);

    play = new QPushButton(this);
    play->setFlat(true);
    play->setIcon(QIcon(":/images/xpm/start.xpm"));
    play->setToolTip(tr("Run"));

    config = new QPushButton(this);
    config->setFlat(true);
    config->setIcon(QIcon(":/images/xpm/config.xpm"));
    config->setToolTip(tr("Configure time bar"));

    timeslider = new QSlider(Qt::Horizontal, this);
    timeslider->setMaximum ( 150);
    timeslider->setMinimum ( 0);
    timeslider->setValue ( 0 );
    layout->addWidget(play);
    layout->addWidget(config);
    layout->addWidget(timeslider);

    this->setLayout(layout);

    connect (config, SIGNAL(clicked()), this, SLOT (timeconfig()));
    connect (play, SIGNAL(clicked()), this, SLOT (playPressed()));
    connect (timeslider, SIGNAL(valueChanged (int)), this, SLOT (valueChanget(int)));

    playing = false;
    fps = 25;
    len = QTime ( 0, 0, 6, 0 );
    setToolTip(tr("World time. When it runs on each tick it calls render() for each enabled script."));
}

TimeWidget::~TimeWidget()
{
    delete layout;
    delete play;
    delete config;
    delete timeslider;
}

/*!
A smal configuration dialog
*/
void TimeWidget::timeconfig()
{
    QDialog d;

    //QWidget *vbox = new QWidget();
    QVBoxLayout vboxlayout;

    d.setLayout(&vboxlayout);

    QWidget gridwidget;
    vboxlayout.addWidget(&gridwidget);
    QGridLayout grid( &gridwidget);
    //grid.cellRect(2, column)
    //gridwidget.setLayout(&grid);

    QLabel l1(tr("FPS"), nullptr);
    grid.addWidget(&l1, 0, 0);

    QSpinBox f(nullptr);
    grid.addWidget(&f, 0, 1);
    f.setValue(fps);
    f.setRange(0, 100);

    QLabel l2(tr("Time"), nullptr);
    grid.addWidget(&l2, 1, 0);

    QTimeEdit t(len, nullptr);
    grid.addWidget(&t, 1, 1);
    t.setTime(len);

    /*
        QCheckBox l("Loop", 0);
        l.setChecked(loop);
        grid.addWidget(&l,2,1);
    */

    QDialogButtonBox btns(nullptr);
    btns.setStandardButtons({QDialogButtonBox::Ok, QDialogButtonBox::Cancel});
    vboxlayout.addWidget(&btns);

    connect(&btns, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted)
    {
        bool p = playing;
        if (p)
            playPressed();
        fps = f.value();
        //loop = l.isChecked();
        len = t.time();
        timeslider->setMaximum ((len.hour () * 3600 + len.minute () * 60 + len.second ())*fps);
        timeslider->setPageStep(fps);
        if (p)
            playPressed();
    }
}

void TimeWidget::timerEvent( QTimerEvent* )
{
    timeslider->setValue( (timeslider->value() + 1 ) % timeslider->maximum());
}

/*!
signal to trigger a new frame rendering
*/
void TimeWidget::valueChanget(int in)
{
    emit timeChanged((double)in / (double)fps);
}

/*!
toggles play/pause
*/
void TimeWidget::playPressed()
{
    if (!playing)
    {
        local_timer = startTimer(1000 / fps);
        playing = true;
        play->setIcon(QIcon(":/images/xpm/breaks.xpm"));
        play->setToolTip(tr("Stop"));
    }
    else
    {
        killTimer(local_timer);
        playing = false;
        play->setIcon(QIcon(":/images/xpm/start.xpm"));
        play->setToolTip(tr("Run"));
    }
}
