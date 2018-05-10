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
** DirectX File Format http://local.wasp.uwa.edu.au/~pbourke/dataformats/directx/
**
*********************************************************************************/


#include "../item.h"
#include "../item_mesh.h"

#include "parsercodec.h"

namespace x
{

    class Skip{}skip;
    QTextStream &operator>>(QTextStream& in, Skip&){
        int count = 0;
        while(true){
            QString s;
            in >> s;
            qDebug() << "Skip: " << s;
            if(s == "{")count++;
            else if(s == "}"){
                count--;
                if (count ==0)break;
            }
        }
        return in;
    }

    //String droping class
    class S{
    public:
        S(const char* _s){s = _s;}
        const char* s;
    };
    QTextStream &operator>>(QTextStream& in, S& s){
        QString i;
        in >> i;
        if (i != s.s){
            qDebug() << "Illegal string/character" << i;
            //throw "Illegal string/character";
        }
        return in;
    }



    class Material;
    class Mesh;
    class Frame;
    class Stuff{
    public:
        QList<Material*> materials;
        QList<Mesh*> meshes;
        QList<Frame*> frames;
    };

    class ColorRGBA{
    public:
        float r,g,b,a;
    };
    QTextStream &operator>>(QTextStream& in, ColorRGBA& c){
        S s(";");
        in >> c.r >> s >> c.g >> s >> c.b >> s >> c.a >> s;
        return in;
    }


    class ColorRGB{
    public:
        float r,g,b;
    };
    QTextStream &operator>>(QTextStream& in, ColorRGB& c){
        S s(";");
        in >> c.r >> s >> c.g >> s >> c.b >> s;
        return in;
    }


    class Material{
    public:
        QString name;
        ColorRGBA faceColor;
        float power;
        ColorRGB specularColor;
        ColorRGB emissiveColor;
        QString filename;
    };
    QTextStream &operator>>(QTextStream& in, Material& m){
        S s(";"); S o("{"); S c("}");
        in >> m.name >> o >> m.faceColor >> s >> m.power >> s;
        in >> m.specularColor >> s >> m.emissiveColor >> s;

        while(!in.atEnd()){
            QString s1;
            in >> s1;
            if (s1 == "}")return in;
            else if ( s1 == "TextureFilename"){
                in >> o >> m.filename >> s >> c ;
                qDebug() << m.filename;
            }
        }
        throw "End of file in Material";
    }


    class Vector3d{
    public:
        float x,y,z;
    };
    QTextStream &operator>>(QTextStream& in, Vector3d& v){
        S s(";");
        in >> v.x >> s >> v.y >> s >> v.z >> s;
        return in;
    }


    class Vector2d{
    public:
        float x,y;
    };
    QTextStream &operator>>(QTextStream& in, Vector2d& v){
        S s(";"); S k(",");
        in >> v.x >> k >> v.y >> s;
        return in;
    }


    class Face{
    public:
        Face(){
            num_of_indices =0;
            indices = NULL;
        }
        ~Face(){
            if(!indices)delete[]indices;
        }
        int num_of_indices;
        int *indices; // 4 should be enough
    };
    QTextStream &operator>>(QTextStream& in, Face& f){
        S s(";"); S k(",");
        in >> f.num_of_indices >> s;
        f.indices = new int[f.num_of_indices];
        for ( int i =0; i < f.num_of_indices -1; i++){
            in >> f.indices[i] >> k;
        }
        in >> f.indices[f.num_of_indices -1] >> s;
        return in;
    }


    class Mesh{ // mesh and normal
    public:
        Mesh(QString _name, Stuff *_stuff){
            name = _name;
            stuff = _stuff;
            num_of_vertices = 0;
            vertices = NULL;
            num_of_texcoords = 0;
            texcoords = NULL;
            num_of_faces = 0;
            faces = NULL;
            normals = NULL;
        }
        ~Mesh(){
            if(!vertices)delete[]vertices;
            if(!texcoords)delete[]texcoords;
            if(!faces)delete[]faces;
            if(!normals)delete normals;
        }
        QString name;
        Stuff *stuff;
        int num_of_vertices;
        Vector3d *vertices;
        int num_of_texcoords;
        Vector2d *texcoords;
        int num_of_faces;
        Face *faces;
        Mesh *normals; //same structure, but without optional members
        Material **facematerials; //owned by stuff.materials
    };
    QTextStream &operator>>(QTextStream& in, Mesh& m){
        S s(";"); S o("{"); S c("}"); S k(",");
        in >> o >> m.num_of_vertices >> s;
        m.vertices = new Vector3d[m.num_of_vertices];

        for (int i = 0; i < m.num_of_vertices - 1; i++){
            in >> m.vertices[i] >> k;
        }
        in >> m.vertices[m.num_of_vertices-1] >> s >> m.num_of_faces >> s;

        m.faces = new Face[m.num_of_faces];
        for (int i = 0; i < m.num_of_faces - 1; i++){
            in >> m.faces[i] >> k;
        }
        in >> m.faces[m.num_of_faces - 1] >> s;

        while(!in.atEnd()){
            QString tag;
            in >> tag;

            if (tag == "}")return in;

            else if (tag == "MeshTextureCoords"){
                if (!m.stuff) throw "MeshTextureCoords in MeshNormals";
                in >> o >> m.num_of_texcoords >> s;
                m.texcoords = new Vector2d[m.num_of_texcoords];
                for (int i = 0; i < m.num_of_texcoords - 1; i++){
                    in >> m.texcoords[i] >> k;
                }
                in >> m.texcoords[m.num_of_texcoords-1] >> s >> c;

            }
            else if (tag == "MeshNormals"){
                if (!m.stuff) throw "MeshNormals in MeshNormals";
                qDebug() << "MeshNormals";
                m.normals = new Mesh("Normals",NULL);
                in >> m.normals[0]; // fake to get a reference
            }
            else if (tag == "MeshMaterialList"){
                if (!m.stuff) throw "MeshMaterialList in MeshNormals";
                //in >> skip;
                int len_mat,len_faces;
                in >> o >> len_mat >> s >> len_faces >> s;
                if (m.num_of_faces != len_faces )throw "MeshMaterialsList: Number of Faces inconsistent";
                QList<int> face_mats;
                int mat_index;
                for (int i = 0; i < m.num_of_faces - 1; i++){
                    in >> mat_index >> k;
                    face_mats << mat_index;
                }
                in >> mat_index >> s >> s;
                face_mats << mat_index;

                QList<Material*> mat_ptr_list;
                for (int i = 0; i < len_mat; i ++ ){
                    QString tag2;
                    in >> tag2;
                    if (tag2 == "{"){
                        QString s3;
                        in >> s3 >> c;
                        for( int k = 0; k < m.stuff->materials.size(); k++){
                            if (m.stuff->materials.at(k)->name == s3){
                                mat_ptr_list.append(m.stuff->materials.at(k));
                            }
                        }
                    }
                    else if (tag2 == "Material") {
                        Material *material = new Material();
                        m.stuff->materials.append(material);
                        mat_ptr_list.append(material);
                        in >> material[0];
                    }

                    else{
                        throw "MeshMaterialsList: Tag is not a material or reference";
                    }
                }

                m.facematerials = new Material*[m.num_of_faces];
                for (int i = 0 ; i < m.num_of_faces; i++){
                    m.facematerials[i] = mat_ptr_list.at(face_mats.at(i));
                }
                in >> c;
            }
        }
        throw "End of file in Mesh";
    }


    class Frame{ // mesh and normal
    public:
        Frame(Stuff *_stuff){
            stuff = _stuff;
            mat[ 0] = 1.0; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = 0.0;
            mat[ 4] = 0.0; mat[ 5] = 1.0; mat[ 6] = 0.0; mat[ 7] = 0.0;
            mat[ 8] = 0.0; mat[ 9] = 0.0; mat[10] = 1.0; mat[11] = 0.0;
            mat[12] = 0.0; mat[11] = 0.0; mat[14] = 0.0; mat[15] = 1.0;

        }
        Stuff *stuff;
        QString name;
        float mat[16];
        QList<Mesh*> meshes;
    };
    QTextStream &operator>>(QTextStream& in, Frame& f){
        S s(";"); S o("{"); S c("}"); S k(",");
        in >> f.name >> o;

        while(!in.atEnd()){
            QString tag;
            in >> tag;
            if (tag == "}")return in;
            else if (tag == "FrameTransformMatrix"){
                in >> o;
                in >> f.mat[ 0] >> k >> f.mat[ 1] >> k >> f.mat[ 2] >> k >> f.mat[ 3] >> k;
                in >> f.mat[ 4] >> k >> f.mat[ 5] >> k >> f.mat[ 6] >> k >> f.mat[ 7] >> k;
                in >> f.mat[ 8] >> k >> f.mat[ 9] >> k >> f.mat[10] >> k >> f.mat[11] >> k;
                in >> f.mat[12] >> k >> f.mat[11] >> k >> f.mat[14] >> k >> f.mat[15] >> s >> s;
                in >> c;
            }
            else if (tag == "Mesh"){
                QString name;
                in >> name;
                qDebug() << "Mesh: " << name;
                Mesh *m = new Mesh(name, f.stuff);
                f.stuff->meshes.append(m);
                f.meshes.append(m);
                in >> m[0];
            }
            else if (tag == "{"){ //reference to mesh
                QString name;
                in >> name >> c;
                for ( int i = 0; i < f.stuff->meshes.size(); i++){
                    if (f.stuff->meshes.at(i)->name == name){
                        f.meshes.append(f.stuff->meshes.at(i));
                    }
                }

            }
            else qDebug() << "Unknown tag in Frame: " << tag;
        }
        throw "Frame: end of file";
    }
}


using namespace x;


void Item_node::importX(const QString& fn){
    qDebug() << "X importer start";

    QFile file(fn);
    if ( !file.open( QIODevice::ReadOnly ) ) {
        qDebug() << "X_importer: File open failed";
        return;
    }


    try{
        QTextStream s( &file );
        ParserCodec Parser;
        s.setCodec (&Parser);

        QString s1,s2,s3;

        s >> s1 >> s2 >> s3;
        if (s1 != "xof" || s2 != "0302txt" || s3 != "0064" )throw "not a x file";

        Stuff stuff;

        while (!s.atEnd()){
            QString tag;
            s >> tag;
            if (tag == "Header") s >> skip;
            else if (tag == "Material") {
                Material *material = new Material();
                stuff.materials.append(material);
                s >> material[0];
            }
            else if (tag == "Mesh"){
                QString name;
                s >> name;
                //qDebug() << "Mesh: " << name;
                Mesh *mesh = new Mesh(name, &stuff);
                stuff.meshes.append(mesh);
                s >> mesh[0];
            }
            else if (tag == "Frame"){
                Frame *frame = new Frame(&stuff);
                stuff.frames.append(frame);
                s >> frame[0];
            }
            else if (tag == "}")break;
            else qDebug() << "Unknown tag: " << tag;
        }

        qDebug() << stuff.frames.size() << stuff.meshes.size() << stuff.materials.size();

        //Collapse all mesh of a Frame into a Lumina single mesh with one index per material.

        for (int f = 0; f < stuff.frames.size(); f++){

            int vert_count = 1;
            for (int m = 0; m < stuff.frames.at(f)->meshes.size(); m++){
                Mesh *mesh = stuff.frames.at(f)->meshes.at(m);
                for(int i = 0 ; i < mesh->num_of_faces ; i++){
                    vert_count += mesh->faces[i].num_of_indices;
                }
            }


            // create mesh object
#define v (*I_position)
#define n (*I_normal)
#define u (*I_uvcoord)
            Item_mesh* I_model = new Item_mesh(this, stuff.frames.at(f)->name , vert_count);
            Item_component v = new Item_component(I_model, "Vertex", Item_component::VERTEX, 3);
            Item_component n = new Item_component(I_model, "Normal", Item_component::VECTOR, 3);
            Item_component u = new Item_component(I_model, "UvCoords", Item_component::UVCOORDS, 2);

            vert_count = 0;


            for (int mat = 0; mat < stuff.materials.size(); mat ++){
                int triangle_count = 0;
                for (int m = 0; m < stuff.frames.at(f)->meshes.size(); m++){
                    Mesh *mesh = stuff.frames.at(f)->meshes.at(m);

                    for(int i = 0 ; i < mesh->num_of_faces ; i++){
                        if (stuff.materials.at(mat) != mesh->facematerials[i])continue;
                        triangle_count += mesh->faces[i].num_of_indices -2;
                    }
                }
                if (triangle_count == 0)continue; //skip unused materials
                qDebug() << stuff.materials.at(mat)->name << triangle_count;

                Item_index *I_index = new Item_index(I_model, stuff.materials.at(mat)->name , 3, triangle_count);

                triangle_count = 0;

                for (int m = 0; m < stuff.frames.at(f)->meshes.size(); m++){
                    Mesh *mesh = stuff.frames.at(f)->meshes.at(m);

                    for(int i = 0 ; i < mesh->num_of_faces ; i++){
                        if (stuff.materials.at(mat) != mesh->facematerials[i])continue;

                        Face *face = mesh->faces + i;
                        //append triangle indices to the mesh  Fan mode
                        for (int k = 0; k < (face->num_of_indices-2); k ++){
                            I_index->set(triangle_count, vert_count, vert_count + k + 1, vert_count + k + 2);
                            qDebug() << triangle_count<< vert_count<< vert_count + k + 1<< vert_count + k + 2;
                            triangle_count ++;
                        }
                        //append vertex data
                        for (int k = 0; k < face->num_of_indices; k ++){

                            Vector3d *vertex = mesh->vertices + face->indices[k];
                            v(0,vert_count,0) = vertex->x;
                            v(1,vert_count,0) = vertex->y;
                            v(2,vert_count,0) = vertex->z;

                            Vector2d *uvcoord = mesh->texcoords + face->indices[k];
                            u(0, vert_count, 0) = uvcoord->x;
                            u(1, vert_count, 0) = uvcoord->y;

                            Vector3d *normal = mesh->normals->vertices + mesh->normals->faces[i].indices[k];
                            n(0,vert_count,0) = normal->x;
                            n(1,vert_count,0) = normal->y;
                            n(2,vert_count,0) = normal->z;
                            vert_count++;
                        }
                    }
                }
            }
        }
    }
    catch(char *e){
        qDebug() << "X Importer: " << e;
    }

    file.close();

}
