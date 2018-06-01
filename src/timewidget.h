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

#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H


/*!
The time widget is a combination of slider play and config button, to controll the time
*/
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QTime>

class TimeWidget : public QWidget
{
    Q_OBJECT
public:
    TimeWidget(QWidget *parent);
    ~TimeWidget() override;
    bool isPlaying()
    {
        return playing;
    }

private slots:
    void valueChanget(int);
    void playPressed();
    void timeconfig();

protected:
    QSlider *timeslider;
    QHBoxLayout *layout;
    QPushButton *play;
    QPushButton *config;

    int fps;
    bool loop;
    bool playing;
    QTime len;
    int local_timer{0};
    void timerEvent( QTimerEvent * ) override;

signals:
    void timeChanged(double);
};
#endif
