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

#include <cmath>
#include <QMessageBox>
#include <QDebug>
#include "../mainwindow.h"
/*!
genNormal(Item_component* vertex); \n

function for generating Normals. The argument is optional and only required if
more than one Position is stored in the mesh.
*/

void Item_mesh::genNormal(Item_component* p)
{
    QList<Item_component*> complist = this->findChildren<Item_component*>();

    QList<Item_index*> indexlist = this->findChildren<Item_index*>();

    qDebug() << "complistsize" << complist.size() << "indexlist" << indexlist.size();

    Item_component* I_position = p;
    if (I_position == nullptr)
        for (auto i : complist)
        {
            if (i->getCompType() == Item_component::VERTEX)
            {
                I_position = i;
                break;
            }
        }

    if (I_position == nullptr || indexlist.size() == 0)
    {
        QMessageBox::warning ( MainWindow::instance, QString(tr("Normal generator error")), tr("This generator needs 1 Vertex component and 1 or more indices"));
        return;
    }

    unsigned keyframes = I_position->getKeyFrames();

    Item_component *I_normal = (Item_component*)addComponent(Item_component::VECTOR, QString("Normal"), 3, keyframes);

#define n (*I_normal)
#define v (*I_position)

    for (unsigned k = 0; k < keyframes ; ++k)
    {
        for (auto I_index : indexlist)
        {
            int ipp = I_index->indices_per_primitive;
            qDebug() << "indices per primitive" << ipp;
            if (ipp == 3)for (int t = 0; t < I_index->num_of_primitives * 3; t += 3)
                {
                    int va =  I_index->index[t + 0];
                    int vb =  I_index->index[t + 1];
                    int vc =  I_index->index[t + 2];

                    int na =  I_index->index[t + 0];
                    int nb =  I_index->index[t + 1];
                    int nc =  I_index->index[t + 2];

                    double v1x = v(0, vb, k) - v(0, va, k);
                    double v1y = v(1, vb, k) - v(1, va, k);
                    double v1z = v(2, vb, k) - v(2, va, k);

                    double v2x = v(0, vc, k) - v(0, va, k);
                    double v2y = v(1, vc, k) - v(1, va, k);
                    double v2z = v(2, vc, k) - v(2, va, k);

                    double cx = v1y * v2z - v1z * v2y;
                    double cy = v1z * v2x - v1x * v2z;
                    double cz = v1x * v2y - v1y * v2x;

                    //normalize, can be removed to weigth the triangle size
                    double l = 1.0 / sqrt(cx * cx + cy * cy + cz * cz);
                    cx *= l;
                    cy *= l;
                    cz *= l;

                    n(0, na, k) += cx;
                    n(1, na, k) += cy;
                    n(2, na, k) += cz;
                    n(0, nb, k) += cx;
                    n(1, nb, k) += cy;
                    n(2, nb, k) += cz;
                    n(0, nc, k) += cx;
                    n(1, nc, k) += cy;
                    n(2, nc, k) += cz;


                }
            if (ipp == 4)
                for (int q = 0; q < I_index->num_of_primitives * 4; q += 4)
                {
                    int va = I_index->index[q + 0];
                    int vb = I_index->index[q + 1];
                    int vc = I_index->index[q + 2];
                    int vd = I_index->index[q + 3];

                    int na = I_index->index[q + 0];
                    int nb = I_index->index[q + 1];
                    int nc = I_index->index[q + 2];
                    int nd = I_index->index[q + 3];

                    double v1x = v(0, vd, k) - v(0, vb, k);
                    double v1y = v(1, vd, k) - v(1, vb, k);
                    double v1z = v(2, vd, k) - v(2, vb, k);

                    double v2x = v(0, vc, k) - v(0, va, k);
                    double v2y = v(1, vc, k) - v(1, va, k);
                    double v2z = v(2, vc, k) - v(2, va, k);

                    double cx = v1y * v2z - v1z * v2y;
                    double cy = v1z * v2x - v1x * v2z;
                    double cz = v1x * v2y - v1y * v2x;

                    //normalize, can be removed to weigth the triangle size
                    double l = 1.0 / sqrt(cx * cx + cy * cy + cz * cz);
                    cx *= l;
                    cy *= l;
                    cz *= l;

                    n(0, na, k) += cx;
                    n(1, na, k) += cy;
                    n(2, na, k) += cz;
                    n(0, nb, k) += cx;
                    n(1, nb, k) += cy;
                    n(2, nb, k) += cz;
                    n(0, nc, k) += cx;
                    n(1, nc, k) += cy;
                    n(2, nc, k) += cz;
                    n(0, nd, k) += cx;
                    n(1, nd, k) += cy;
                    n(2, nd, k) += cz;
                }
        }
    }
    //normalize
    for (unsigned k = 0; k < keyframes ; ++k)
    {
        for (uint32_t j = 0; j < this->num_of_vertices; ++j)
        {
            double x = n(0, j, k);
            double y = n(1, j, k);
            double z = n(2, j, k);
            double l = 1.0 / sqrt(x * x + y * y + z * z);
            n(0, j, k) *= l;
            n(1, j, k) *= l;
            n(2, j, k) *= l;
        }
    }
}
