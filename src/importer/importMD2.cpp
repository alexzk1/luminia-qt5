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
** Code based on : http://tfc.duke.free.fr/coding/md2-specs-en.html
**
*********************************************************************************/

#include "../item.h"
#include "../item_mesh.h"
#include <QFile>

namespace md2
{
#pragma pack(1)
    struct header_t
    {
        uint32_t ident;                  // magic number: "IDP2"
        uint32_t version;                // version: must be 8

        uint32_t skinwidth;              // texture width
        uint32_t skinheight;             // texture height

        uint32_t framesize;              // size in bytes of a frame

        uint32_t num_skins;              // number of skins
        uint32_t num_vertices;           // number of vertices per frame
        uint32_t num_texCoords;          // number of texture coordinates/
        uint32_t num_triangles;          // number of triangles
        uint32_t num_glcmds;             // number of opengl commands
        uint32_t num_frames;             // number of frames

        uint32_t offset_skins;           // offset skin data
        uint32_t offset_st;              // offset texture coordinate data
        uint32_t offset_tris;            // offset triangle data
        uint32_t offset_frames;          // offset frame data
        uint32_t offset_glcmds;          // offset OpenGL command data
        uint32_t offset_end;             // offset end of file
    };

    using vec3_t = float[3];

    struct skin_t
    {
        char name[64];              // texture file name
    };

    struct texCoord_t
    {
        int16_t s;
        int16_t t;
    };

    struct frame_t
    {
        vec3_t scale;               // scale factor
        vec3_t translate;           // translation vector
        char name[16];              // frame name
        //struct vertex_t *vertices;  // list of frame's vertices
    };

    struct vertex_t
    {
        uint8_t v[3];         // position
        uint8_t normalIndex;  // normal vector index
    };

    struct glcmd_t
    {
        float s;                    // s texture coord.
        float t;                    // t texture coord.
        uint32_t index;                  // vertex index
    };
#pragma pack()

    vec3_t anorms_table[162] =
    {
#include "anorms.h"
    };

}

using namespace md2;



void Item_node::importMD2(const QString& fn)
{
    qDebug() << "MD2 importer start";

    QFile file(fn);
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "MD2_importer: File open failed";
        return;
    }

    skin_t* skins = nullptr;
    texCoord_t* texCoords = nullptr;
    int *glcmds = nullptr;
    vertex_t *vertices = nullptr;

    try
    {
        header_t header;
        file.read ((char *) &header, sizeof(header_t));

        if ((header.ident != 844121161) || (header.version != 8))throw "Not a MD2 Header";

        skins = new skin_t[header.num_skins];
        texCoords = new texCoord_t[header.num_texCoords];
        glcmds = new int[header.num_glcmds];
        vertices = new vertex_t[ header.num_vertices];

        //qDebug() << "Skins: " << header.num_skins << "TexCoord" << header.num_texCoords << "Triangles" << header.num_triangles << "Frames" << header.num_frames << "glcmds" << header.num_glcmds << "Vertices" << header.num_vertices;

        file.seek(header.offset_skins);
        file.read((char*)skins, sizeof(skin_t) * header.num_skins);

        file.seek(header.offset_st);
        file.read((char*)texCoords, sizeof(texCoord_t) * header.num_texCoords);

        file.seek(header.offset_glcmds);
        file.read((char*)glcmds, sizeof(int) * header.num_glcmds);


        //determine the true number of vertices and build index
        int i, *pglcmds = glcmds;
        int vert_count = 0;

        while ((i = *(pglcmds++)) != 0)
        {
            if (i < 0)i = -i;
            vert_count += i;
            pglcmds += 3 * i;
        }

        Item_mesh* I_model = new Item_mesh(this, "Model", vert_count);

        Item_index *I_index = new Item_index(I_model, "Index", 3, header.num_triangles);
        Item_component *I_position = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3, header.num_frames);
        Item_component *I_normal   = new Item_component(I_model, "Normal", Item_component::VECTOR, 3, header.num_frames);
        Item_component *I_uvcoord  = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2, 1);




        //set data for UvCoords and Index
        struct glcmd_t *packets;
        vert_count = 0;
        int index_count = 0;
        pglcmds = glcmds;
        while ((i = *(pglcmds++)) != 0)
        {
            packets = (struct glcmd_t *)pglcmds;
            if (i < 0)
            {
                //Fan Mode
                i = -i;
                for (int k = 0; k < (i - 2); k ++)
                {
                    I_index->set(index_count, vert_count, vert_count + k + 1, vert_count + k + 2);
                    index_count ++;
                }
            }
            else
            {
                //Stripe mode
                for (int k = 0; k < (i - 2); k ++)
                {
                    if (k & 1)I_index->set(index_count, vert_count + k, vert_count + k + 2, vert_count + k + 1);
                    else I_index->set(index_count, vert_count + k, vert_count + k + 1, vert_count + k + 2);
                    index_count ++;
                }
            }
            for (int k = 0 ; k < i; k++)
            {
                I_uvcoord->set(vert_count, packets[k].s, packets[k].t);
                vert_count ++;
            }
            pglcmds += 3 * i;
        }

        file.seek (header.offset_frames);

        frame_t frame;

        for (uint32_t f = 0; f < header.num_frames; ++f)
        {
            file.read((char*)&frame.scale, sizeof (vec3_t));
            file.read((char*)&frame.translate, sizeof (vec3_t));
            file.read((char*)&frame.name, 16);
            file.read((char*)vertices, sizeof(struct vertex_t) * header.num_vertices);

            qDebug() << frame.name << f;
            vert_count = 0;
            pglcmds = glcmds;
            while ((i = *(pglcmds++)) != 0)
            {
                packets = (struct glcmd_t *)pglcmds;
                if (i < 0)i = -i;
                for (int k = 0 ; k < i; k++)
                {

                    float x = frame.scale[0] * vertices[packets[k].index].v[0] + frame.translate[0];
                    float y = frame.scale[1] * vertices[packets[k].index].v[1] + frame.translate[1];
                    float z = frame.scale[2] * vertices[packets[k].index].v[2] + frame.translate[2];
                    I_position->setInKeyFrame(f, vert_count, x, y, z);

                    float *v = anorms_table[vertices[packets[k].index].normalIndex];

                    I_normal->setInKeyFrame(f, vert_count, v[0], v[1], v[2]);

                    vert_count ++;
                }
                pglcmds += 3 * i;
            }
        }
    }
    catch (char *e)
    {
        qDebug() << "MD2 Importer: " << e;
    }

    file.close();
    if (skins)
        delete[] skins;
    if (texCoords)
        delete[] texCoords;
    if (vertices)
        delete[] vertices;
    if (glcmds)
        delete[] glcmds;
}
