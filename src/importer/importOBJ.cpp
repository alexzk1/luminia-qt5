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


#include "../item.h"
#include "../item_mesh.h"

#include <math.h>
#include <QFile>

namespace obj{

    class Index{
    public:
        int v;
        int t;
        int n;

        Index(const QString &s){
            //convert OBJ (offset +1) index to C index
            QStringList l = s.split("/");
            v = l.size()>0 ? l.at(0).toInt()-1:0;
            t = l.size()>1 ? l.at(1).toInt()-1:0;
            n = l.size()>2 ? l.at(2).toInt()-1:0;
            }

        Index(int _v, int _t, int _n){
            v= _v; t = _t; n = _n;
            }

        bool operator<(const Index& o) const{
            return (v<o.v) || (v==o.v && ( t<o.t || ( t==o.t && n<o.n)));
            }
        bool operator==(const Index& o) const{
            return (v==o.v && t==o.t &&  n==o.n);
            }
        };

    struct vec3_t{
        float x;
        float y;
        float z;
        };

    struct vec2_t{
        float u;
        float v;
        };
    }

using namespace obj;

void Item_node::importOBJ(const QString& fn){
    QFile *file;
    file = new QFile(fn);

    vec3_t *vertices = NULL;
    vec3_t *normals = NULL;
    vec2_t *texcoords = NULL;

    try{
        qDebug() << "OBJ importer start";

        if ( !file->open( QIODevice::ReadOnly | QIODevice::Text) ) throw  "OBJ_importer: File open failed";

        int num_of_verts = 0;
        int num_of_normals = 0;
        int num_of_texcoords = 0;
        int num_of_faces = 0;
        int num_of_tris = 0;

        QTextStream in(file);
        QList<Index> I_list;

        while (!in.atEnd()) {
            QString line = in.readLine();

            if (line.startsWith("vt")) num_of_texcoords++;
            else if (line.startsWith("vn")) num_of_normals++;
            else if (line.startsWith("v")) num_of_verts++;
            else if (line.startsWith("f")) {
                num_of_tris += line.count(QRegExp("[0-9]\\s[0-9]")) - 1;
                QStringList list =  line.split(" ", QString::SkipEmptyParts);
                for (int i = 1; i < list.size(); ++i){
                    I_list << list.at(i);
                    }
                }
            }

        qSort( I_list.begin(), I_list.end());

        for (int i = I_list.size()-2; i >= 0 ; i--)if (I_list.at(i) == I_list.at(i+1)) I_list.removeAt(i);

        //for (int i = 0; i < I_list.size(); ++i)qDebug() << I_list.at(i).v << I_list.at(i).t << I_list.at(i).n;

        qDebug() << "OBJ importer: " << num_of_verts << num_of_normals << num_of_texcoords << num_of_faces << num_of_tris << I_list.size();

        //temp memory
        vertices = new vec3_t[num_of_verts];
        normals = new vec3_t[num_of_normals];
        texcoords = new vec2_t[num_of_texcoords];

        if (num_of_verts == 0 || num_of_tris == 0) throw "no Content";

        Item_mesh* I_model = new Item_mesh(this, "Model", I_list.size());
        Item_component *I_position = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3);
        Item_component *I_normal   = new Item_component(I_model, "Normal", Item_component::VECTOR, 3);
        Item_component *I_uvcoord  = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2);
        Item_index *I_index = new Item_index(I_model, "Index", 3, num_of_tris);

        num_of_verts = 0;
        num_of_normals = 0;
        num_of_texcoords = 0;
        num_of_tris = 0;

        in.seek(0);
        while (!in.atEnd()) {
            QString line = in.readLine();

            if (line.startsWith("vt")){
                QStringList list =  line.split(" ", QString::SkipEmptyParts);
                if (list.size() < 3) throw "not enough values in vt line";
                texcoords[num_of_texcoords].u = list.at(1).toFloat();
                texcoords[num_of_texcoords].v = list.at(2).toFloat();
                num_of_texcoords++;
                }
            else if (line.startsWith("vn")){
                QStringList list =  line.split(" ", QString::SkipEmptyParts);
                if (list.size() < 4) throw "not enough values in vn line";
                normals[num_of_normals].x = list.at(1).toFloat();
                normals[num_of_normals].y = list.at(2).toFloat();
                normals[num_of_normals].z = list.at(3).toFloat();
                num_of_normals++;
                }
            else if (line.startsWith("v")){
                QStringList list =  line.split(" ", QString::SkipEmptyParts);
                if (list.size() < 4) throw "not enough values in v line";
                vertices[num_of_verts].x = list.at(1).toFloat();
                vertices[num_of_verts].y = list.at(2).toFloat();
                vertices[num_of_verts].z = list.at(3).toFloat();
                num_of_verts ++;
                }
            else if (line.startsWith("f")) {
                QStringList list = line.split(" ", QString::SkipEmptyParts);
                QList<int> face;
                Index comp(num_of_verts,num_of_texcoords,num_of_normals);

                for (int i = 1; i < list.size(); ++i){
                    int indexnum = qBinaryFind (I_list, Index(list.at(i)))-I_list.begin();
                    if (comp < I_list.at(indexnum))throw "Index out of range";
                    int v = I_list.at(indexnum).v;
                    I_position->set(indexnum,vertices[v].x , vertices[v].y, vertices[v].z );

                    if (num_of_texcoords){
                        v = I_list.at(indexnum).t;
                        I_uvcoord->set(indexnum, texcoords[v].u,  texcoords[v].v);
                        }

                    if (num_of_normals){
                        v = I_list.at(indexnum).n;
                        I_normal->set(indexnum, normals[v].x, normals[v].y, normals[v].z);
                        }
                    face << indexnum;
                    }
                //qDebug() << face;
                for (int i = 2; i < face.size(); ++i){
                    I_index->set(num_of_tris,face.at(0),face.at(i-1),face.at(i));
                    num_of_tris++;
                    }
                }
            }
        }

    catch(char *e){
        qDebug() << "OBJ Importer: " << e;
        }

    if(vertices) delete[]vertices;
    if(normals) delete[]normals;
    if(texcoords) delete[]texcoords;
    delete file;
    }













