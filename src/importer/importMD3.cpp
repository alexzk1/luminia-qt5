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
** Code based on : http://www.icculus.org/homepages/phaethon/q3/formats/md3format.html
**
*********************************************************************************/


#include "../item.h"
#include "../item_mesh.h"

#include <math.h>
#include <QFile>

namespace md3{

    struct header_t{
        int ident;                  // magic number: "IDP2"
        int version;                // version: must be 8
        char name[64];              // name in pk3 file
        int flags;                  // ??
        int num_frames;             // Number of frames
        int num_tags;               // Number of Tags
        int num_surfaces;           // Number of surface objects
        int num_skins;              // NUmber of skins
        int ofs_frames;
        int ofs_tags;
        int ofs_surfaces;
        int ofs_eof;
        };

    struct vec3_t{
        float x;
        float y;
        float z;
        };

    struct frame_t{
        vec3_t min_bounds;           // First corner of the bounding box
        vec3_t max_bounds;           // Second corner of the bounding box
        vec3_t local_orgin;          // Local origin, usually (0, 0, 0)
        float radius;                // Radius of bounding sphere
        char name[16];               // Name of Frame. ASCII character string
        };

    struct tag_t{
        char* name[64];              // Name of Tag object
        vec3_t orgin;                // Coordinates of Tag object
        vec3_t axis[3];              // Orientation of Tag object
        };

    struct surface_t{
        int ident;                   // Magic number IDP3
        char name[64];               // Name of Surface object
        int flags;                   // ???
        int num_frames;              // Number of frames
        int num_shaders;             // Number of shaders
        int num_vertices;            // number of vertices
        int num_triangles;           // number of triangles
        int ofs_triangles;           //
        int ofs_shaders;             //
        int ofs_st;                  //
        int ofs_xyznormal;           //
        int ofs_end;                 //
        };

    struct shader_t{
        char name[64];
        int shader_index;
        };

    struct triangle_t{
        int index[3];
        };

    struct texcoord_t{
        float s;
        float t;
        };

    struct vertex_t{
        short x;
        short y;
        short z;
        short normal;
        };

    vec3_t dec_normal(short normal){
        float lat = ((normal >> 8) & 255) * 6.2831853 / 256.0;
        float lng = (normal & 255) * 6.2831853 / 256.0;
        vec3_t n;
        n.x = cos(lat) * sin(lng);
        n.y = sin(lat) * sin(lng);
        n.z = cos(lng);
        return n;
        }
    }

using namespace md3;

void Item_node::importMD3(const QString& fn){

    qDebug() << "MD3 importer start";

    QFile file(fn);
    if ( !file.open( QIODevice::ReadOnly ) ) {
        qDebug() << "MD3_importer: File open failed";
        return;
        }

    triangle_t *triangles = NULL;
    texcoord_t *texcoords = NULL;
    vertex_t *vertices = NULL;

    try{
        header_t header;
        file.read ((char *) &header, sizeof(header_t));
        if ((header.ident != 860898377) ||(header.version != 15))throw "Not a MD3 Header";

        qDebug () << "Frames " << header.num_frames << "Tags" << header.num_tags << "Surfaces" <<  header.num_surfaces << "Skins" << header.num_skins ;


        surface_t surface;
        int vert_count = 0;
        int max_index = 0;

        int surface_start = header.ofs_surfaces;
        file.seek(surface_start);
        for (int s = 0; s < header.num_surfaces; s++){
            file.read((char*)&surface, sizeof(surface_t));
            if (surface.ident != 860898377) throw "Not a surface header";
            vert_count += surface.num_vertices;
            max_index = (max_index < surface.num_triangles)? surface.num_triangles: max_index;

            //qDebug () << "Frames" << surface.num_frames << "Shaders" << surface.num_shaders << "Vertices" <<  surface.num_vertices << "Triangles" << surface.num_triangles;
            file.seek(surface_start += surface.ofs_end);
            }

        Item_mesh* I_model = new Item_mesh(this, "Model", vert_count);
        Item_component *I_position = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3, header.num_frames);
        Item_component *I_normal   = new Item_component(I_model, "Normal", Item_component::VECTOR, 3, header.num_frames);
        Item_component *I_uvcoord  = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2, 1);

        triangles = new triangle_t[max_index];
        texcoords = new texcoord_t[vert_count]; // max vertices would be enough too
        vertices = new vertex_t[vert_count];

        surface_start = header.ofs_surfaces;
        file.seek(surface_start);
        vert_count = 0;
        for (int s = 0; s < header.num_surfaces; s++){
            file.read((char*)&surface, sizeof(surface_t));
            if (surface.ident != 860898377) throw "Not a surface header";

            Item_index *I_index = new Item_index(I_model, "Index", 3, surface.num_triangles);

            file.seek(surface_start + surface.ofs_triangles);
            file.read((char*)triangles, sizeof(triangle_t) * surface.num_triangles);
            for (int t = 0; t < surface.num_triangles; t++){
                I_index->set(t,triangles[t].index[0] + vert_count,triangles[t].index[1] + vert_count,triangles[t].index[2] + vert_count);
                //qDebug() << triangles[t].index[0] << triangles[t].index[1] << triangles[t].index[2];
                }

            file.seek(surface_start + surface.ofs_st);
            file.read((char*)texcoords, sizeof(texcoord_t) * surface.num_vertices);
            for ( int t = 0; t < surface.num_vertices; t++){
                I_uvcoord->set(t + vert_count,texcoords[t].s,texcoords[t].t);
                //qDebug() << texcoords[t].s<< texcoords[t].t;
                }

            file.seek(surface_start + surface.ofs_xyznormal);
            for ( int f = 0; f < surface.num_frames; f++){
                file.read((char*)vertices, sizeof(vertex_t) * surface.num_vertices);
                for ( int t = 0; t < surface.num_vertices; t++){
                    I_position->setInKeyFrame(f,t + vert_count, vertices[t].x/64.0,vertices[t].y/64.0,vertices[t].z/64.0);
                    vec3_t normal = dec_normal(vertices[t].normal);
                    I_normal->setInKeyFrame(f,t + vert_count, normal.x, normal.y, normal.z);
                    }
                }


            vert_count += surface.num_vertices;
            file.seek(surface_start += surface.ofs_end);
            }

        // handle tags
        int tag_start = header.ofs_tags;

        QList<Item_buffer*> Bufferlist;

        file.seek(tag_start);
        for (int t = 0; t < header.num_tags;  t++){
            tag_t tag;
            file.read((char*)&tag, sizeof(tag_t));
            qDebug() << t <<(char*)tag.name;
            Item_buffer* b = new Item_buffer(this,QString((char*)tag.name), ceil(header.num_frames/5.0), 16);
            Bufferlist << b;
            }

        file.seek(tag_start);
        for (int u = 0; u < ceil (header.num_frames/5.0);  u++){ // only 1 tag / 5 keyframe ?
            for ( int t=0; t <  header.num_tags; t++){
                tag_t tag;
                file.read((char*)&tag, sizeof(tag_t));
                //qDebug() << t <<(char*)tag.name;
                QList<double> l;
                l << tag.axis[0].x << tag.axis[0].y << tag.axis[0].z << 0.0;
                l << tag.axis[1].x << tag.axis[1].y << tag.axis[1].z << 0.0;
                l << tag.axis[2].x << tag.axis[2].y << tag.axis[2].z << 0.0;
                l << tag.orgin.x   << tag.orgin.y   << tag.orgin.z   << 1.0;
                Bufferlist.at(t)->set(u,l);
                }
            }
/*

        int frames_start = header.ofs_frames;
        file.seek(frames_start);
        for (int t = 0; t < header.num_frames; t++){
            frame_t frame;
            file.read((char*)&frame, sizeof(frame_t));
            qDebug() << t <<(char*)frame.name;
            }
*/
        }
    catch(char *e){
        qDebug() << "MD3 Importer: " << e;
        }

    if (triangles != NULL) delete[] triangles;
    if (texcoords != NULL) delete[] texcoords;
    if (vertices != NULL) delete[] vertices;

    file.close();
    }
