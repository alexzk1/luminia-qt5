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

#include "factory.h"
#include <QtGui>
#include "dialog_factory_private.h"


class QColorPicker : public QFrame
{
    Q_OBJECT
public:
    QColorPicker(QWidget* parent);
    ~QColorPicker();

public slots:
    void setCol(int h, int s);

signals:
    void newCol(int h, int s);

protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
	void resizeEvent ( QResizeEvent *);
private:
    int hue;
    int sat;

    QPoint colPt();
    int huePt(const QPoint &pt);
    int satPt(const QPoint &pt);
    void setCol(const QPoint &pt);

    QPixmap *pix;
};

//static int pWidth = 220;
//static int pHeight = 200;

class QColorLuminancePicker : public QFrame
{
    Q_OBJECT
public:
    QColorLuminancePicker(QWidget* parent=0);
    ~QColorLuminancePicker();

public slots:
    void setCol(int h, int s, int v);
    void setCol(int h, int s);

signals:
    void newHsv(int h, int s, int v);

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int val;
    int hue;
    int sat;

    int y2val(int y);
    int val2y(int val);
    void setVal(int v);

    QPixmap *pix;
};



class ColorWidgetPrivate : public QWidget {
	Q_OBJECT
public:
	ColorWidgetPrivate(QWidget* parent=0) : QWidget(parent){
		luminacePicker = new QColorLuminancePicker(0);
		colorPicker = new QColorPicker(0);
		layout = new QHBoxLayout();
		layout->addWidget(colorPicker);
		layout->addWidget(luminacePicker);
		setLayout(layout);

		connect(colorPicker, SIGNAL(newCol(int,int)),luminacePicker,SLOT(setCol(int,int)));
		connect(luminacePicker,SIGNAL(newHsv(int, int, int)), this, SLOT(colorChanged(int, int, int)));
		 setColor(Qt::white);
		}

	QColor getColor() const{
		return col;
		}

	void setColor(const QColor & c){
		int h,s,v;
		c.getHsv(&h, &s, &v);
		colorPicker->setCol(h,s);
		luminacePicker->setCol(h,s,v);
		}
		
	void setColor(int r, int g, int b){
		int h,s,v;
		QColor(r,g,b).getHsv(&h, &s, &v);
		colorPicker->setCol(h,s);
		luminacePicker->setCol(h,s,v);
		}


private slots:

	void colorChanged(int h, int s, int v){
		col.setHsv ( h, s, v);
		emit changed();
		}

signals:
	void changed();


private:
	QColorLuminancePicker *luminacePicker;
	QColorPicker *colorPicker;
	QHBoxLayout *layout;

	QColor col;

	};


namespace QS{


//class ColorWidget : public Labeled{
class ColorWidget : public Widget{
	Q_OBJECT
	Q_PROPERTY(QColor color READ getColor() WRITE setColor)

	inline  ColorWidgetPrivate *d() const { return (ColorWidgetPrivate*)widget; }



public:
	//ColorWidget(QObject* = NULL) : Labeled(Input::tr("Color:"), new ColorWidgetPrivate(0)) {}
	ColorWidget(QObject* = NULL) : Widget(new ColorWidgetPrivate(0)) {
		connect(d(),SIGNAL(changed()),this, SLOT(sendChanged()));
		}


	QColor getColor() const{
		return d()->getColor();
		}
	void setColor(const QColor & c){
		d()->setColor(c);
		}
signals:
	void changed();

private slots:
	void sendChanged(){
		emit changed();
		}



	};





	} //namespace QS