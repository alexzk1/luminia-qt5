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


#include "../item.h"
#include "../item_mesh.h"

#include <math.h>
#include <QFile>

namespace ds
{

#pragma pack(1)

    struct chunk_t
    {
        short id;
        int length;
    };

    struct vec3_t
    {
        float x;
        float y;
        float z;
    };

    struct face_t
    {
        unsigned short a;
        unsigned short b;
        unsigned short c;
        unsigned short flags;
    };

    struct vec2_t
    {
        float u;
        float v;
    };

#pragma pack()

    QFile *file;
    chunk_t chunk;

    int num_of_vertex;
    int num_of_texcoords;
    int num_of_mesh;
    int pass;

    Item_mesh *I_model;
    Item_component *I_position;
    Item_component *I_uvcoord;
    //Item_component *I_normal;

    //0x4110
    void process_vertex_list(int first_vertex)
    {
        qDebug() << "vertex list";
        int end = chunk.length + file->pos() - 6;
        unsigned short num;
        file->read((char*)&num, sizeof(short));


        if (pass == 1)
        {
            vec3_t vert;
            for (int i = 0; i < num; i ++ )
            {
                file->read((char*)&vert, sizeof(vec3_t));
                I_position->set(first_vertex + i, vert.x, vert.y, vert.z);
            }
        }

        num_of_vertex += num;
        qDebug() << num << "vertices";
        file->seek(end);
    }

    //0x4120
    void process_face_description(int first_vertex)
    {
        qDebug() << "face description";
        int end = chunk.length + file->pos() - 6;

        unsigned short num;
        file->read((char*)&num, sizeof(short));
        qDebug() << num << "triangles";
        if (pass == 1 && num > 0)
        {
            Item_index *I_index = new Item_index(I_model, "Index", 3, num);
            face_t tri;
            for (int i = 0; i < num; i++)
            {
                file->read((char*)&tri, sizeof(face_t));
                I_index->set(i, tri.a + first_vertex, tri.b + first_vertex, tri.c + first_vertex);
            }
        }

        file->seek(end);
    }

    //0x4140
    void process_texcoords(int first_vertex)
    {
        qDebug() << "texture coordinates";
        int end = chunk.length + file->pos() - 6;

        unsigned short num;
        file->read((char*)&num, sizeof(short));
        qDebug() << num << "texcoords";


        if (pass == 1)
        {
            vec2_t vert;
            for (int i = 0; i < num; i ++ )
            {
                file->read((char*)&vert, sizeof(vec2_t));
                I_uvcoord->set(first_vertex + i, vert.u, vert.v);
            }
        }
        num_of_texcoords += num;

        file->seek(end);
    }

    //0x4100
    void process_triangular_mesh()
    {
        qDebug() << "triangular mesh";
        num_of_mesh++;
        int first_vertex = num_of_vertex;
        int stop_pos = chunk.length + file->pos();
        while ( stop_pos > file->pos())
        {
            file->read((char*)&chunk, sizeof(chunk_t));
            switch (chunk.id)
            {
                case 0x4110:
                    process_vertex_list(first_vertex);
                    break;
                case 0x4120:
                    process_face_description(first_vertex);
                    break;
                case 0x4140:
                    process_texcoords(first_vertex);
                    break;
                default:
                    qDebug() << "Unknown chunk " << chunk.id;
                    file->seek(chunk.length + file->pos() - 6);
            }
        }
    }

    //0x4000
    void process_object_block()
    {
        qDebug() << "object block";
        int stop_pos = chunk.length + file->pos();

        char c[20];
        for (int i = 0; i < 20 ; i ++)
        {
            file->getChar(c + i);
            if ( c[i] == '\0') break;
        }
        qDebug() << "    Name:"  << c;

        while ( stop_pos > file->pos())
        {
            file->read((char*)&chunk, sizeof(chunk_t));
            if (chunk.id == 0x4100)
                process_triangular_mesh();
            else
            {
                qDebug() << "Unknown chunk " << chunk.id;
                file->seek(chunk.length + file->pos() - 6);
            }
        }
    }

    //0x3D3D
    void process_editor_chunk()
    {
        int stop_pos = chunk.length + file->pos();
        while ( stop_pos > file->pos())
        {
            file->read((char*)&chunk, sizeof(chunk_t));
            if (chunk.id == 0x4000)
                process_object_block();

            else
            {
                qDebug() << "Unknown chunk " << chunk.id;
                file->seek(chunk.length + file->pos() - 6);
            }

        }
    }

    //0x4d4d
    void process_main_chunk()
    {
        int stop_pos = chunk.length + file->pos();
        while ( stop_pos > file->pos())
        {
            file->read((char*)&chunk, sizeof(chunk_t));
            if (chunk.id == 0x3D3D)
                process_editor_chunk();
            else
            {
                qDebug() << "Unknown chunk " << chunk.id;
                file->seek(chunk.length + file->pos() - 6);
            }
        }
    }

}

using namespace ds;

void Item_node::import3DS(const QString& fn)
{

    qDebug() << "3DS importer start";

    file =  new QFile(fn);
    try
    {
        num_of_vertex = 0;
        num_of_texcoords = 0;
        num_of_mesh = 0;

        pass = 0;

        if ( !file->open( QIODevice::ReadOnly ) )throw "File open failed";

        file->read((char*)&chunk, sizeof(chunk_t));
        if (chunk.id != 0x4d4d) throw "No main chunk found";
        process_main_chunk();

        qDebug() << "Vertices:" << num_of_vertex << "Texcoords" << num_of_texcoords << "Meshes" << num_of_mesh;

        I_model = new Item_mesh(this, "Model",  num_of_vertex);
        I_position = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3, 1);
        //Item_component *I_normal = new Item_component(I_model, "Normal", Item_component::VECTOR, 3, header.num_frames);
        if (num_of_texcoords != 0)
            I_uvcoord  = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2, 1);

        file->seek(6);
        num_of_vertex = 0;
        num_of_texcoords = 0;
        num_of_mesh = 0;
        pass = 1;
        process_main_chunk();
    }

    catch (char *e)
    {
        qDebug() << "3DS Importer: " << e;
    }

    delete file;
}

