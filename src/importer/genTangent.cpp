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

#include "../item_mesh.h"

#include <cmath>
#include <QMessageBox>
#include "../mainwindow.h"
#include <QDebug>
/*!
genTangent(Item_component* vertex, Item_component* normal, Item_component* texcoord); \n

function for generating Normals. The argument is optional and only required if
more than one Position is stored in the mesh.
*/

void Item_mesh::genTangent(Item_component* _position, Item_component* _normal, Item_component* _texcoord)
{

    QList<Item_component*> complist = this->findChildren<Item_component*>();
    QList<Item_index*> indexlist = this->findChildren<Item_index*>();

    qDebug() << "complistsize" << complist.size() << "indexlist" << indexlist.size();

    Item_component* I_position = _position;
    if (!I_position)
        for (auto i : complist)
        {
            if (i->getCompType() == Item_component::VERTEX)
            {
                I_position = i;
                break;
            }
        }

    Item_component* I_normal = _normal;
    if (!I_normal)
        for (auto i : complist)
        {
            if (i->getCompType() == Item_component::VECTOR)
            {
                I_normal = i;
                break;
            }
        }

    Item_component* I_texcoord = _texcoord;
    if (!I_texcoord)
        for (auto i : complist)
        {
            if (i->getCompType() == Item_component::UVCOORDS)
            {
                I_texcoord = i;
                break;
            }
        }

    qDebug() << I_position << I_normal << I_texcoord << indexlist.size();

    if (I_position == nullptr || I_normal == nullptr || I_texcoord == nullptr || indexlist.size() == 0)
    {
        QMessageBox::warning ( MainWindow::instance, tr("Tangent generator error"), tr("This generator needs 1 Vertex component 1 Vector component, 1 UVcoord and 1 or more indices") );
        return;
    }

#define n (*I_normal)
#define v (*I_position)
#define tex (*I_texcoord)

    unsigned keyframes = I_position->getKeyFrames();
    //int keyframes_tex = I_texcoord->getKeyFrames();
    if (keyframes != I_normal->getKeyFrames() )
    {
        QMessageBox::warning ( MainWindow::instance, tr("Tangent generator error"), tr("Vertex (Position) and Vector (Normal) don't have the same number of keyframes"));
        return;
    }

    Item_component *I_tangent = (Item_component*)addComponent(Item_component::VECTOR, QString("Tangent"), 3, keyframes);
#define tan (*I_tangent)

    for ( unsigned k = 0; k < keyframes ; ++k)
    {
        for (auto I_index : indexlist)
        {
            int ipp = I_index->indices_per_primitive;
            qDebug() << "indices per primitive" << ipp;
            if (ipp == 3)for (int t = 0; t < I_index->num_of_primitives * 3; t += 3)
                {
                    int ia = I_index->index[t + 0];
                    int ib = I_index->index[t + 1];
                    int ic = I_index->index[t + 2];

                    /*
                            for face in msh.faces:
                              v1 = face.v[1].co - face.v[0].co
                              v2 = face.v[2].co - face.v[0].co
                              t1 = face.uv[1][1] - face.uv[0][1]
                              t2 = face.uv[2][1] - face.uv[0][1]
                              sdir = v1 * t2 - v2 * t1
                              sdir.normalize()
                    */
                    double v1x = v(0, ib, k) - v(0, ia, k);
                    double v1y = v(1, ib, k) - v(1, ia, k);
                    double v1z = v(2, ib, k) - v(2, ia, k);

                    double v2x = v(0, ic, k) - v(0, ia, k);
                    double v2y = v(1, ic, k) - v(1, ia, k);
                    double v2z = v(2, ic, k) - v(2, ia, k);

                    double t1y = tex(1, ib, 0) - tex(1, ia, 0); //use always the first keyframe?
                    double t2y = tex(1, ic, 0) - tex(1, ia, 0); //


                    double cx = v1x * t2y - v2x * t1y;
                    double cy = v1y * t2y - v2y * t1y;
                    double cz = v1z * t2y - v2z * t1y;

                    //normalize, can be removed to weigth the triangle size
                    double l = 1.0 / sqrt(cx * cx + cy * cy + cz * cz);
                    cx *= l;
                    cy *= l;
                    cz *= l;

                    tan(0, ia, k) += cx;
                    tan(1, ia, k) += cy;
                    tan(2, ia, k) += cz;
                    tan(0, ib, k) += cx;
                    tan(1, ib, k) += cy;
                    tan(2, ib, k) += cz;
                    tan(0, ic, k) += cx;
                    tan(1, ic, k) += cy;
                    tan(2, ic, k) += cz;

                }

            if (ipp == 4)
                for (int q = 0; q < I_index->num_of_primitives * 4; q += 4)
                {
                    int ia = I_index->index[q + 0];
                    int ib = I_index->index[q + 1];
                    int ic = I_index->index[q + 2];
                    int id = I_index->index[q + 3];

                    double v1x = v(0, id, k) - v(0, ib, k);
                    double v1y = v(1, id, k) - v(1, ib, k);
                    double v1z = v(2, id, k) - v(2, ib, k);

                    double v2x = v(0, ic, k) - v(0, ia, k);
                    double v2y = v(1, ic, k) - v(1, ia, k);
                    double v2z = v(2, ic, k) - v(2, ia, k);

                    double t1y = tex(1, id, 0) - tex(1, ib, 0);
                    double t2y = tex(1, ic, 0) - tex(1, ia, 0);

                    double cx = v1x * t2y - v2x * t1y;
                    double cy = v1y * t2y - v2y * t1y;
                    double cz = v1z * t2y - v2z * t1y;

                    //normalize, can be removed to weigth the quad size
                    double l = 1.0 / sqrt(cx * cx + cy * cy + cz * cz);
                    cx *= l;
                    cy *= l;
                    cz *= l;

                    tan(0, ia, k) += cx;
                    tan(1, ia, k) += cy;
                    tan(2, ia, k) += cz;
                    tan(0, ib, k) += cx;
                    tan(1, ib, k) += cy;
                    tan(2, ib, k) += cz;
                    tan(0, ic, k) += cx;
                    tan(1, ic, k) += cy;
                    tan(2, ic, k) += cz;
                    tan(0, id, k) += cx;
                    tan(1, id, k) += cy;
                    tan(2, id, k) += cz;
                }
        }
    }
    for ( unsigned k = 0; k < keyframes ; ++k)
    {
        for (uint32_t j = 0; j < this->num_of_vertices; j++)
        {
            //orthogonalize
            //tan[i] = tan[i] - normal[i] *  Dot(tan[i], normal[i])
            double dot = tan(0, j, k) * n(0, j, k) + tan(1, j, k) * n(1, j, k) + tan(2, j, k) * n(2, j, k);

            tan(0, j, k) -= dot * n(0, j, k);
            tan(0, j, k) -= dot * n(0, j, k);
            tan(0, j, k) -= dot * n(0, j, k);

            //normalize
            double x = tan(0, j, k);
            double y = tan(1, j, k);
            double z = tan(2, j, k);
            double l = 1.0 / sqrt(x * x + y * y + z * z);
            tan(0, j, k) *= l;
            tan(1, j, k) *= l;
            tan(2, j, k) *= l;
        }
    }
}
