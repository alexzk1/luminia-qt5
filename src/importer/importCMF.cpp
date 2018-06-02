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
**********************************************************************************
**
**                             Cal3d mesh importer.
**
*********************************************************************************/

#include "../item.h"
#include "../item_mesh.h"
#include <QFile>

namespace cmf
{
#pragma pack(1)
    struct vec3_t
    {
        float x;
        float y;
        float z;
    };

    struct vec2_t
    {
        float x;
        float y;
    };

    struct header_t
    {
        int magic;
        int version;
        int submesh;
    };

    struct submesh_t
    {
        int material;
        int num_of_vertices;
        int num_of_faces;
        int num_of_lod;
        int num_of_springs;
        int num_of_maps;
    };

    struct influence_t
    {
        int bone_id;
        float weight;
    };
#pragma pack()

    class Vertex
    {
    public:
        Vertex(QFile *file, int _num_of_uvcoods, int num_of_springs)
        {
            file->read((char*)&position, sizeof(vec3_t));
            file->read((char*)&normal, sizeof(vec3_t));
            file->read((char*)&colapse_id, sizeof(int));
            file->read((char*)&face_colapse_count, sizeof(int));

            num_of_uvcoods = _num_of_uvcoods;
            uvcoords = nullptr;
            if (num_of_uvcoods > 0)
            {
                uvcoords = new vec2_t[num_of_uvcoods];
                file->read((char*)uvcoords, sizeof(vec2_t) * num_of_uvcoods);
            }

            file->read((char*)&num_of_influences, sizeof(int));
            influences = nullptr;
            if (num_of_influences > 0)
            {
                influences = new influence_t[num_of_influences];
                file->read((char*)influences, sizeof(vec2_t) * num_of_influences);
            }
            if (num_of_springs > 0)
                file->read((char*)&weight, sizeof(float));
        }

        ~Vertex()
        {
            if (uvcoords) delete[] uvcoords;
            if (influences) delete[] influences;
        }

        influence_t influence(int i)const
        {
            return influences[i];       //warning range don't checked
        }

        vec2_t uvcoord(int i)const
        {
            return uvcoords[i];     //warning range don't checked
        }

        vec3_t position;
        vec3_t normal;
        int colapse_id;         // unknown stuff
        int face_colapse_count;     // unknown stuff
        int num_of_influences;

    private:
        influence_t *influences;
        int num_of_uvcoods;
        vec2_t *uvcoords;
        float weight;       //dummy have to be read if  springs > 0
    };

    struct spring_t
    {
        int a;              //vertex id 1
        int b;              //vertex id 2
        float coefficient;
        float idle_length;
    };

    struct triangle_t
    {
        int a;              //vertex id 1
        int b;              //vertex id 2
        int c;              //vertex id 3
    };

}

using namespace cmf;

void Item_node::importCMF(const QString& fn)
{
    qDebug() << "CMF importer start";

    QFile file(fn);
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "CMF_importer: File open failed";
        return;
    }

    try
    {
        header_t header;
        file.read ((char *) &header, sizeof(header_t));

        if ((header.magic != 4607299))throw "Not a cal3d CMF Header";
        qDebug() << "cmf version " << header.version << header.submesh;

        //create

        Item_mesh *I_model = new Item_mesh(this, "Model", 1);

        Item_component *I_position = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3, 1);
        Item_component *I_normal   = new Item_component(I_model, "Normal", Item_component::VECTOR, 3, 1);

        Item_component *I_uvcoords = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2, 1);


        Item_component *I_weigth   = new Item_component(I_model, "Weight", Item_component::GENERIC, 4, 1, GL_UNSIGNED_BYTE, true);
        Item_component *I_boneid   = new Item_component(I_model, "BoneId", Item_component::GENERIC, 4, 1, GL_UNSIGNED_BYTE, false);

        int ofs = 0;

        for (int s = 0 ; s < header.submesh ; s++)
        {
            submesh_t sub;
            file.read ((char *) &sub, sizeof(submesh_t));

            qDebug() << "Submesh mat: " << sub.material << " verts: " << sub.num_of_vertices << " faces: " << sub.num_of_faces << " lod: " << sub.num_of_lod << " springs: " << sub.num_of_springs << " maps: " << sub.num_of_maps;

            I_model->setNumOfVertices(ofs + sub.num_of_vertices); // append


            for (int v = 0 ; v < sub.num_of_vertices ; v++)
            {
                Vertex vert(&file, sub.num_of_maps, sub.num_of_springs);
                //qDebug()  << vert.num_of_influences;

                I_position->set(v + ofs, vert.position.x, vert.position.y, vert.position.z);
                I_normal->set(v + ofs, vert.normal.x, vert.normal.y, vert.normal.z);

                I_uvcoords->set(v + ofs, vert.uvcoord(0).x, vert.uvcoord(0).y);

                int boneid[4] = {0, 0, 0, 0};
                float weight[4] = {0.0, 0.0, 0.0, 0.0};
                unsigned char cweight[4] = {0, 0, 0, 0};
                for (int i = 0 ; i < vert.num_of_influences && i < 4; i++)
                {
                    //qDebug()  << "   " <<  vert.influence(i).bone_id << vert.influence(i).weight;
                    boneid[i] = vert.influence(i).bone_id;
                    weight[i] = vert.influence(i).weight;
                    cweight[i] = weight[i] * 255.0;
                }

                int diff =  255 - int(cweight[0] + cweight[1] + cweight[2] + cweight[3]); // correction against quantisation errors

                I_boneid->set(v + ofs, boneid[0], boneid[1], boneid[2], boneid[3]);
                I_weigth->set(v + ofs, weight[0] + diff / 255.0, weight[1], weight[2], weight[3]);
            }

            for (int sp = 0; sp < sub.num_of_springs; sp++)
            {
                spring_t spring;
                file.read ((char *) &spring, sizeof(spring_t));
            }


            Item_index *I_index = new Item_index(I_model, QString("Mat%1").arg(sub.material), 3, sub.num_of_faces);

            for ( int t = 0; t < sub.num_of_faces; t++)
            {
                triangle_t tri;
                file.read ((char *) &tri, sizeof(triangle_t));
                I_index->set(t, ofs + tri.a, ofs + tri.b, ofs + tri.c);

            }

            ofs += sub.num_of_vertices;
        }

    }
    catch (char *e)
    {
        qDebug() << "CMF Importer: " << e;
    }

    file.close();

}
