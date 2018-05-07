/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007-2008  oc2k1
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
#include "item.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QDebug>

Item_uniform::Item_uniform( Item *parent, const QString& name, unsigned _dim, unsigned _size,  unsigned _keyframes, int _format) : Item( parent, name){

    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDragEnabled);
    setIcon( 0, QIcon(":/images/xpm/uniform.xpm") );

    buf.f = NULL;
    size = 0; dim =0; keyframes =0;
    setDim( _dim, _size, _keyframes, _format);

    ref_pos = -1;
    menuinit = false;
}

Item_uniform::~Item_uniform()
{
    delete [] buf.f;
}


/*!
slot for opening the contextmenu
*/
void Item_uniform::contextmenu(const QPoint& point){

    context = this;

    if(!menuinit){
        menu->addAction( QString( "Properties"), this, SLOT(PropertiesDialog()) );
        menu->addSeparator();
        DQMENU(Item_edit, menu);
        menu->addSeparator();

        SCRIPT2MENU();
        menu->addSeparator();
        menu->addAction( QIcon(":/images/xpm/del.xpm"), QString("Delete") , this, SLOT( deleteLater()));
        menuinit = true;
    }

    menu->popup( point );
}



/*!
void setDim(number dimension, number size, number keyframes, enum format, bool normalized)\n
Set the uniform to dimension with size keyframes and format;
*/
void Item_uniform::setDim(int _dim, int _size, int _keyframes, int _format){

    unsigned tmp_dim =  _dim  < 1 ? 1 : _dim;
    unsigned tmp_size = _size < 1 ? 1 : _size ;
    unsigned tmp_keyframes = _keyframes < 1 ? 1 : _keyframes;

    if (_format != GL_FLOAT &&  _format != GL_INT){
        qDebug() << " Item_uniform::setDim(): Only GL_FLOAT and GL_INT are alowed for uniforms";
        return;
    }

    int max_elements = tmp_dim * tmp_keyframes * tmp_size;
    qDebug() << " Item_uniform::setDim" << tmp_dim << tmp_size << tmp_keyframes << max_elements;

    unsigned elements = tmp_dim * tmp_keyframes * tmp_size;

#define COPY_DATA(dest) 										\
    if (buf.f){											\
    for (unsigned k = 0; k < (tmp_keyframes < keyframes ? tmp_keyframes : keyframes);k++){	\
    for (unsigned j = 0 ; j < (tmp_size < size ? tmp_size : size); j++){		\
    for (unsigned i = 0; i < (tmp_dim < dim ? tmp_dim : dim); i++){		\
    double x = operator()(i,j,k);					\
    dest [i + tmp_dim * j + tmp_dim * tmp_size * k] = x ;		\
}								\
}									\
}										\
}

    switch (_format){

        case GL_INT:
            tmp_buf.i = new int[elements];
            memset(tmp_buf.i,0,elements * 4);
            COPY_DATA(tmp_buf.i)
                    format = GL_INT;
            break;

        case GL_FLOAT:
            tmp_buf.f = new float[elements];
            memset(tmp_buf.f,0,elements * 4);
            COPY_DATA(tmp_buf.f)
                    format =  GL_FLOAT;
            break;
        default:
            qDebug() << "Item_uniform::setDim panic";
    }

    size = tmp_size;
    dim = tmp_dim;
    keyframes = tmp_keyframes;
    format = _format;

    if(!buf.f)delete[]buf.f;
    buf.f = tmp_buf.f;

    ref_pos = -1;
    need_refresh = true;
}

/*!
returns some statur informations about the object
*/
QString Item_uniform::statusText() const{
    QString k("");
    if (keyframes > 1){
        k = QString ("%3 Keyframes with").arg(keyframes);
    }

    switch (format){
        case GL_INT:
            return QString("Uniform %1 %2 Int%4").arg(k).arg(size).arg(dim);
        default:
            return QString("Uniform %1 %2 Float%3").arg(k).arg(size).arg(dim);
    }
}

/*!
Operator to acces the contend witha up to 3 dimensional index.
*/
double& Item_uniform::operator()( unsigned _dim, unsigned _index, unsigned _keyframe){

    unsigned element = _dim + _index * dim + _keyframe * dim * size;

    if (element > dim * size * keyframes){
        qDebug() << objectName() << " Item_uniform::operator(): Index out of range";
        return ref_buf;
    }

    switch (format){
        case GL_INT:
            if(ref_pos>=0)buf.i[ref_pos] = ref_buf;
            ref_buf = buf.i[element];
            break;

        default:
            if(ref_pos>=0)buf.f[ref_pos] = ref_buf;
            ref_buf = buf.f[element];
    }

    ref_pos = element;
    need_refresh = true;
    return ref_buf;
}

/*!
internal function.
*/
void Item_uniform::refresh(){
    if(need_refresh && (ref_pos>=0)){
        switch (format){

            case GL_INT:
                buf.i[ref_pos] = ref_buf;
                break;
            case GL_FLOAT:
                buf.f[ref_pos] = ref_buf;
                break;
            default:
                qDebug() << "Item_uniform::refresh() Panic";
        }
    }
    need_refresh = false;
}



/*!
void set(int index, number x[, number y=0[, number z=0[, number w=0]]])\n
Set the element index.
*/
void Item_uniform::set(int index, double x, double y, double z, double w){
    setInKeyFrame(0, index, x, y, z, w);
}

/*!
void setInKeyFrame(number key, number index, number x[, number y=0[, number z=0[, number w=0]]])\n
*/
void Item_uniform::setInKeyFrame(int key, int index, double x, double y, double z, double w){
    switch (dim){
        case 4:  (*this)(3, index, key) = w;
            [[clang::fallthrough]]; case 3:  (*this)(2, index, key) = z;
            [[clang::fallthrough]]; case 2:  (*this)(1, index, key) = y;
            [[clang::fallthrough]]; default: (*this)(0, index, key) = x;
    }
}

/*!
void set(int index, Array od numbers)\n
Set the element at index.
*/

void Item_uniform::set(int index, const QList<double>& l){
    setInKeyFrame(0, index, l);
}

void Item_uniform::setInKeyFrame(int key, int index, const QList<double>& l){
    if ( (unsigned)index > size) return;
    if ( l.size() != (int)dim ){
        qDebug() << "Item_uniform::set() list size don't match  dim";
    }
    for (unsigned i = 0 ; i < dim; i++){
        (*this)(i,index,key) = l.at(i);
    }
}

/*!
internal function
*/
void Item_uniform::setData(const QString& content){
    QStringList list = content.split(QRegExp("\\s+"));

    unsigned n = dim * size * keyframes;
    if (n > (unsigned)list.count()){
        qDebug() <<  "Item_uniform::setData(): not enough data";
        return;
    };

    double tmp = normalized_scalefactor;
    normalized_scalefactor = 1.0; //don't normalize data in files, to save space for integer formats
    n =0;
    for (unsigned k = 0; k < keyframes ;k++){
        for (unsigned j = 0 ; j < size ; j++){
            for (unsigned i = 0; i < dim; i++){
                (*this)(i,j,k) = list.at(n).toFloat();
                n++;
            }
        }
    }
    normalized_scalefactor = tmp;
}

/*!
internal function
*/
QString Item_uniform::getData(){
    refresh();
    QString data;
    QString num;

    double tmp = normalized_scalefactor;
    normalized_scalefactor = 1.0; //don't normalize data in files, to save space for integer formats

    for (unsigned k = 0; k < keyframes ;k++){
        for (unsigned j = 0 ; j < size ; j++){
            for (unsigned i = 0; i < dim; i++){
                double x = (*this)(i,j,k);
                num.setNum(x);
                data.append(num).append(" ");
            }
            data.append("\n");
        }
    }
    normalized_scalefactor = tmp;

    return data;
}

/*!
internal function \n
returns the dimension i.E. 1,2,3,4,9,16
*/
int Item_uniform::getDim(){
    return dim;
}

/*!
internal function \n
returns the size (number of vertices)
*/
int Item_uniform::getSize(){
    return size;
}

/*!
internal function \n
returns the number of keyframes
*/
int Item_uniform::getKeyFrames(){
    return keyframes;
}

/*!
internal function \n
returns the Opengl format. Only GL_FLOAT, GL_INT
*/
int Item_uniform::getFormat(){
    return format;
}

QString Item_uniform::getType() const
{
    return "Uniform";
}



#include "glwrapper.h"
/*!
Uniform(object shader, string uniformname);\n
function to bind the buffer as uniform array variable, the used format in glsl have to match
the dimension of the buffer.
*/

void Item_uniform::Uniform(QObject* _shader, QString var){
    if (glwrapper_shader* shader = dynamic_cast<glwrapper_shader*>(_shader)){
        refresh();
        int loc = glGetUniformLocationARB(shader->getShaderHandle(),var.toLatin1().constData());

        if (format == GL_FLOAT){
            switch (dim){
                case 16: glUniformMatrix4fv(loc,  size, GL_FALSE, buf.f);	return;
                case 9:  glUniformMatrix3fv(loc,  size, GL_FALSE, buf.f);	return;
                case 4:  glUniform4fvARB(loc, size, buf.f);			return;
                case 3:  glUniform3fvARB(loc, size, buf.f);			return;
                case 2:  glUniform2fvARB(loc, size, buf.f);			return;
                case 1:  glUniform1fvARB(loc, size, buf.f);			return;
            }
        }
        else if (format == GL_INT){
            switch (dim){
                case 4:  glUniform4ivARB(loc, size, (GLint*) buf.i);			return;
                case 3:  glUniform3ivARB(loc, size, (GLint*) buf.i);			return;
                case 2:  glUniform2ivARB(loc, size, (GLint*) buf.i);			return;
                case 1:  glUniform1ivARB(loc, size, (GLint*) buf.i);			return;
            }
        }
        else{
            qDebug() << "Item_uniform::UniformBind(): Only glfloat and glint uniform variables are supported";
        }
        qDebug() << "Item_uniform::UniformBind(): This dimension is not supported as uniform variables";
    }
}

/*!

*/
void Item_uniform::UniformInterpolator( QObject* _shader, const QString& var, float pos){
    int keyframe2 = ceil(pos);
    int keyframe1 = keyframe2 - 1;
    float mix  = pos - (float)keyframe1;

    UniformInterpolator(_shader, var ,mix, keyframe1, keyframe2);
}



/*!
void UniformInterpolator( object shader, String var, num mix, num keyframe1, num keyframe2);\n
function to bind the buffer as uniform array variable, works only with float format uniforms.\n keyframe1 * (1.0 - mix) + keyframe2 * mix
*/
void Item_uniform::UniformInterpolator( QObject* _shader, const QString& var, float mix, int keyframe1, int keyframe2){
    if (glwrapper_shader* shader = dynamic_cast<glwrapper_shader*>(_shader)){
        refresh();
        int loc = glGetUniformLocationARB(shader->getShaderHandle(),var.toLatin1().constData());
        if (format == GL_FLOAT){
            float *fbuf = new float[size * dim];

            for (unsigned i = 0; i < size; i++){
                for (unsigned d = 0 ; d < dim; d++){
                    fbuf[i * dim + d] = (*this)(d,i,keyframe1)*(1.0-mix) + (*this)(d,i,keyframe2)*mix;
                }
            }

            switch (dim){
                case 16: glUniformMatrix4fv(loc,  size, GL_FALSE, fbuf);	return;
                case 9:  glUniformMatrix3fv(loc,  size, GL_FALSE, fbuf);	return;
                case 4:  glUniform4fvARB(loc, size, fbuf);			return;
                case 3:  glUniform3fvARB(loc, size, fbuf);			return;
                case 2:  glUniform2fvARB(loc, size, fbuf);			return;
                case 1:  glUniform1fvARB(loc, size, fbuf);			return;
            }

            delete[] fbuf;
        }

        else{
            qDebug() << "Item_uniform::UniformBind(): Only glfloat and glint uniform variables are supported";
        }
        qDebug() << "Item_uniform::UniformBind(): This dimension is not supported as uniform variables";
    }
}





/*!
Opens the properties dialog
*/
void Item_uniform::PropertiesDialog(){

    QDialog d;
    QVBoxLayout vboxlayout;
    d.setLayout(&vboxlayout);

    QWidget gridwidget;
    vboxlayout.addWidget(&gridwidget);
    QGridLayout grid( &gridwidget);

    int row = 0;

    QLabel l_dim("Dim:",0);
    grid.addWidget(&l_dim,row,0);
    QSpinBox s_dim(0);
    grid.addWidget(&s_dim,row,1);
    s_dim.setValue(getDim());
    s_dim.setRange(1,4);
    row++;

    QLabel l_size("Size:",0);
    QSpinBox s_size(0);
    s_size.setRange(1,1<<24);
    s_size.setValue(getSize());
    if(getType()!= "Component"){
        grid.addWidget(&l_size,row,0);
        grid.addWidget(&s_size,row,1);
        row++;
    }

    QLabel l_key("Keyframes:",0);
    grid.addWidget(&l_key,row,0);
    QSpinBox s_key(0);
    grid.addWidget(&s_key,row,1);
    s_key.setRange(1,10000);
    s_key.setValue(getKeyFrames());
    row++;


    QLabel l_format("Format:");
    grid.addWidget(&l_format,row,0);
    QComboBox c_format(0);
    grid.addWidget(&c_format,row,1);
    c_format.addItems(QStringList() << "Float" << "Int");
    switch (format){
        case GL_FLOAT:		c_format.setCurrentIndex(0);	break;
        case GL_INT:		c_format.setCurrentIndex(1);	break;
        default: 		c_format.setCurrentIndex(0);	break;
    }
    row++;

    QWidget buttonwidget;
    vboxlayout.addWidget(&buttonwidget);
    QHBoxLayout buttons;
    buttonwidget.setLayout(&buttons);

    QPushButton ok("OK",0);
    connect (&ok, SIGNAL(clicked()),&d,SLOT (accept()));
    buttons.addWidget(&ok);

    QPushButton cancel("Cancel",0);
    connect (&cancel, SIGNAL(clicked()),&d,SLOT (reject()));
    buttons.addWidget(&cancel);

    if (d.exec()==QDialog::Accepted){
        int f;
        switch (c_format.currentIndex()){
            case 1:
                f = GL_INT;
                break;
            default:
                f = GL_FLOAT;
        }
        setDim(s_dim.value(),s_size.value(), s_key.value(), f);
    }
}
