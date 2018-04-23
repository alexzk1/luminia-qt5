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
*********************************************************************************
**
**  Code from qt-x11-opensource-src-4.3.3/src/gui/dialogs/qcolordialog.cpp
**
*********************************************************************************/

#include "colorwidget_factory_private.h"


int QColorLuminancePicker::y2val(int y)
{
    int d = height() - 2*coff - 1;
    return 255 - (y - coff)*255/d;
}

int QColorLuminancePicker::val2y(int v)
{
    int d = height() - 2*coff - 1;
    return coff + (255-v)*d/255;
}

QColorLuminancePicker::QColorLuminancePicker(QWidget* parent)
    :QFrame(parent)
{
    hue = 100; val = 100; sat = 100;
    pix = 0;
    //    setAttribute(WA_NoErase, true);
setFixedWidth (24);

}

QColorLuminancePicker::~QColorLuminancePicker()
{
    delete pix;
}

void QColorLuminancePicker::mouseMoveEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}
void QColorLuminancePicker::mousePressEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}

void QColorLuminancePicker::setVal(int v)
{
    if (val == v)
        return;
    val = qMax(0, qMin(v,255));
    delete pix; pix=0;
    repaint();
    emit newHsv(hue, sat, val);
}

//receives from a hue,sat chooser and relays.
void QColorLuminancePicker::setCol(int h, int s)
{
    setCol(h, s, val);
    emit newHsv(h, s, val);
}

void QColorLuminancePicker::paintEvent(QPaintEvent *)
{
    int w = width() - 5;

    QRect r(0, foff, w, height() - 2*foff);
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if (!pix || pix->height() != hi || pix->width() != wi) {
        delete pix;
        QImage img(wi, hi, QImage::Format_RGB32);
        int y;
        for (y = 0; y < hi; y++) {
            QColor c;
            c.setHsv(hue, sat, y2val(y+coff));
            QRgb r = c.rgb();
            int x;
            for (x = 0; x < wi; x++)
                img.setPixel(x, y, r);
        }
        pix = new QPixmap(QPixmap::fromImage(img));
    }
    QPainter p(this);
    p.drawPixmap(1, coff, *pix);
    const QPalette &g = palette();
    qDrawShadePanel(&p, r, g, true);
    p.setPen(g.foreground().color());
    p.setBrush(g.foreground());
    QPolygon a;
    int y = val2y(val);
    a.setPoints(3, w, y, w+5, y+5, w+5, y-5);
    p.eraseRect(w, 0, 5, height());
    p.drawPolygon(a);
}

void QColorLuminancePicker::setCol(int h, int s , int v)
{
    val = v;
    hue = h;
    sat = s;
    delete pix; pix=0;
    repaint();
}

QPoint QColorPicker::colPt()
{ return QPoint((360-hue)*(width()-1)/360, (255-sat)*(height()-1)/255); }
int QColorPicker::huePt(const QPoint &pt)
{ return 360 - pt.x()*360/(width()-1); }
int QColorPicker::satPt(const QPoint &pt)
{ return 255 - pt.y()*255/(height()-1) ; }
void QColorPicker::setCol(const QPoint &pt)
{ setCol(huePt(pt), satPt(pt)); }





/***********************************************************************/






QColorPicker::QColorPicker(QWidget* parent) : QFrame(parent){
	hue = 0; sat = 0;
	setCol(150, 255);
	pix = NULL;
	resize (width(), height());
	  //setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
	setMinimumSize(150,100);
	setAttribute(Qt::WA_NoSystemBackground);

}

QColorPicker::~QColorPicker()
{
    delete pix;
}


void QColorPicker::resizeEvent ( QResizeEvent *e){
    QImage img(width(), height(), QImage::Format_RGB32);
    int x,y;
    for (y = 0; y < height(); y++)
        for (x = 0; x < width(); x++) {
            QPoint p(x, y);
            QColor c;
            c.setHsv(huePt(p), satPt(p), 200);
            img.setPixel(x, y, c.rgb());
        }

	if(pix)delete pix;
    pix = new QPixmap(QPixmap::fromImage(img));



	}


QSize QColorPicker::sizeHint() const
{
    return QSize(150, 100);
}

void QColorPicker::setCol(int h, int s)
{
    int nhue = qMin(qMax(0,h), 359);
    int nsat = qMin(qMax(0,s), 255);
    if (nhue == hue && nsat == sat)
        return;
    QRect r(colPt(), QSize(20,20));
    hue = nhue; sat = nsat;
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);
    //    update(r);
    repaint(r);
}

void QColorPicker::mouseMoveEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
    emit newCol(hue, sat);
}

void QColorPicker::mousePressEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
    emit newCol(hue, sat);
}

void QColorPicker::paintEvent(QPaintEvent* )
{
    QPainter p(this);
    drawFrame(&p);
    QRect r = contentsRect();

    p.drawPixmap(r.topLeft(), *pix);
    QPoint pt = colPt() + r.topLeft();
    p.setPen(Qt::black);

    p.fillRect(pt.x()-9, pt.y(), 20, 2, Qt::black);
    p.fillRect(pt.x(), pt.y()-9, 2, 20, Qt::black);

}
