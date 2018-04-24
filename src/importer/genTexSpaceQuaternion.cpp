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

#include "../item_mesh.h"

#include "math.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>


#define n (*I_normal)
#define t (*I_tangent)
#define q (*I_quaternion)


void flip_recursive(const QList<int> &in, int k, const QList<int> *interlink, Item_component *I_quaternion, bool *b){
    QList<int> out;
    for (int i = 0 ; i < in.size(); i++ ){
        int Qi = in.at(i);
        if(b[Qi])continue; //skip already processed vertex
        b[Qi]= true;

        for (int e = 0; e < interlink[Qi].size(); e++){ // loop that flips all connected
            if (b[interlink[Qi].at(e)])continue; //skip already processed vertex
            //calculate dot and flip if negative
            int Qe = interlink[Qi].at(e);
            float dot = q(0,Qe,k)*q(0,Qi,k) + q(1,Qe,k)*q(1,Qi,k) + q(2,Qe,k)*q(2,Qi,k) + q(3,Qe,k)*q(3,Qi,k);
            if (dot < 0.0){
                q(0,Qe,k) *= -1.0;
                q(1,Qe,k) *= -1.0;
                q(2,Qe,k) *= -1.0;
                q(3,Qe,k) *= -1.0;
                }
            out.append(Qe); //process new neighbors in next recursion;
            }
        }
    if(out.size()!=0)flip_recursive(out,k,interlink,I_quaternion,b);
    }


/*!
void createTexSpaceQuaternion(Object normal, Object Tangent, String name="Texspace")\n
experimental function to create a texturespace quaternion component stream
from a normal and tangent vector stream.
The blender exporter provides a artifact free algorithm.
This function may be removed or improved in future
*/
void Item_mesh::genTexSpaceQuaternion(Item_component* _I_normal,Item_component* _I_tangent){
    //Item_component* normal = dynamic_cast<Item_component*>(_normal);
    //Item_component* tangent = dynamic_cast<Item_component*>(_tangent);
    Item_component* I_normal  = _I_normal;
    Item_component* I_tangent = _I_tangent;



    if (I_normal == NULL || I_tangent == NULL){
        QList<Item_component*> complist = this->findChildren<Item_component*>();

        QDialog d;
        QVBoxLayout vboxlayout;
        d.setLayout(&vboxlayout);

        QWidget gridwidget;
        vboxlayout.addWidget(&gridwidget);
        QGridLayout grid( &gridwidget);

        QLabel ln("Normal:");
        grid.addWidget(&ln,0,0);
        QComboBox cn(0);
        grid.addWidget(&cn,0,1);

        QLabel lt("Tangent:");
        grid.addWidget(&lt,1,0);
        QComboBox ct(0);
        grid.addWidget(&ct,1,1);

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

        for (int  i = 0; i < complist.size(); i++){
            if (complist.at(i)->getCompType() != Item_component::VECTOR)continue;
            cn.addItem(complist.at(i)->objectName());
            ct.addItem(complist.at(i)->objectName());
            }

        if (d.exec()==QDialog::Accepted){
            I_normal = dynamic_cast<Item_component*>(findChild(cn.currentText()));
            I_tangent = dynamic_cast<Item_component*>(findChild(ct.currentText()));
            }
        }

    if (I_normal == NULL || I_tangent == NULL){
        qDebug() << "createTexSpaceQuaternion failed illegal parameter";
        return;
        }
    if (I_normal->getDim() != 3 || I_tangent->getDim() != 3){
        qDebug() << "createTexSpaceQuaternion failed illegal dimension";
        return;
        }

    int tkey = I_tangent->getKeyFrames();
    int nkey = I_normal->getKeyFrames();
    int keyframes = tkey < nkey ? tkey : nkey;

    Item_component* I_quaternion = static_cast<Item_component*>(addComponent(Item_component::QUATERNION, "TexSpace",4, keyframes));

    float mat[9];

    for (int k = 0 ; k < keyframes; k++){
        for (int i = 0 ; i < num_of_vertices * keyframes ; i++ ){

            float S;

            mat[0]= t(0,i,k);
            mat[1]= t(1,i,k);
            mat[2]= t(2,i,k);
            S = sqrt(mat[0]*mat[0]+mat[1]*mat[1]+mat[2]*mat[2]);
            mat[0] /= S; mat[1]/= S; mat[2] /= S;

            mat[6]= n(0,i,k);
            mat[7]= n(1,i,k);
            mat[8]= n(2,i,k);

            S = sqrt(mat[6]*mat[6]+mat[7]*mat[7]+mat[8]*mat[8]);
            mat[6] /= S; mat[7]/= S; mat[8] /= S;

            mat[3] = mat[2]*mat[7]-mat[1]*mat[8];
            mat[4] = mat[0]*mat[8]-mat[2]*mat[6];
            mat[5] = mat[1]*mat[6]-mat[0]*mat[7];

            //qDebug() << mat[0] <<mat[1]<<mat[2]<<"\n"<<mat[3]<<mat[4]<<mat[5]<<"\n"<<mat[6]<<mat[7]<<mat[8];

            float Q[4];// = I_quaternion->vertices + i * 4;
            S = 1 + mat[0] + mat[4] + mat[8];
            if  ( S > 0.00000001 ){
                S = sqrt(S) * 2;
                Q[0] = ( mat[7] - mat[5] ) / S;
                Q[1] = ( mat[2] - mat[6] ) / S;
                Q[2] = ( mat[3] - mat[1] ) / S;
                Q[3] = 0.25 * S;
                }
            else{
                if ( mat[0] > mat[4] && mat[0] > mat[8] ){
                    S  = sqrt( 1.0 + mat[0] - mat[4] - mat[8] ) * 2;
                    Q[0] = 0.25 * S;
                    Q[1] = (mat[3] + mat[1] ) / S;
                    Q[2] = (mat[2] + mat[6] ) / S;
                    Q[3] = (mat[7] - mat[5] ) / S;
                    }
                else if ( mat[4] > mat[8] ){
                    S  = sqrt( 1.0 + mat[4] - mat[0] - mat[8] ) * 2;
                    Q[0] = (mat[3] + mat[1] ) / S;
                    Q[1] = 0.25 * S;
                    Q[2] = (mat[7] + mat[5] ) / S;
                    Q[3] = (mat[2] - mat[6] ) / S;
                    }
                else{
                    S  = sqrt( 1.0 + mat[8] - mat[0] - mat[4] ) * 2;
                    Q[0] = (mat[2] + mat[6] ) / S;
                    Q[1] = (mat[7] + mat[5] ) / S;
                    Q[2] = 0.25 * S;
                    Q[3] = (mat[3] - mat[1] ) / S;
                    }
                }
            S = 1.0/sqrt(Q[0] * Q[0] + Q[1] * Q[1] + Q[2] * Q[2] + Q[3] * Q[3]);
            //Q[0]/=S; Q[1]/=S; Q[2]/=S; Q[3]/=S;
            //qDebug() << S <<"\n" ;//<< Q[0] << Q[1] << Q[2] << Q[3];
            q(0,i,k)= Q[0] * S;
            q(1,i,k)= Q[1] * S;
            q(2,i,k)= Q[2] * S;
            q(3,i,k)= Q[3] * S;

            }
        }

    //build interlink index
    QList<int> *interlink = new QList<int>[num_of_vertices];

    QList<Item_index*> indexlist = this->findChildren<Item_index*>();
    for (int i = 0; i < indexlist.size(); i++){
        Item_index* I_index = indexlist.at(i);
        for (int k = 0; k < I_index->num_of_primitives; k++){
            int ofs = k * I_index->indices_per_primitive;
            for (int x = 0; x < I_index->indices_per_primitive; x++){
                for (int y = 0; y < I_index->indices_per_primitive; y++){
                    if(x!=y)interlink[I_index->index[ofs + x]].append(I_index->index[ofs + y]);
                    }
                }
            }
        }

    //Flipping all neighbor quaternion to avoid interpolation artifacts
    bool *b = new bool[num_of_vertices];
    for (int k = 0 ; k < keyframes; k++){
        for (int i = 0 ; i < num_of_vertices; i++ )b[i] = false;

        for (int i = 0 ; i < num_of_vertices; i++ ){ // loop that starts the flipping
            if (b[i])continue; //already processed
            flip_recursive(QList<int>() << i,k,interlink,I_quaternion,b);
            }
        }

    delete[] b;
    delete[] interlink;
    }
