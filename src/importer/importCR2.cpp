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
**********************************************************************************

		More informations about the poser file format:
http://www.kuroyumes-developmentzone.com/poser/poserfilespec/products_poser_cr2.html

*********************************************************************************/

#include <QtGui>
#include "../item.h"
#include "../item_mesh.h"

#include "parsercodec.h"
#include "CR2Resources.h"
namespace cr2{

	class Skip{}skip;
	QTextStream &operator>>(QTextStream& in, Skip&){
		int count = 0;
		while(true){
			QString s;
			in >> s;
			//qDebug() << "Skip: " << s;
			if(s == "{")count++;
			else if(s == "}"){
				count--;
				if (count ==0)break;
				}
			}
		return in;
		}

/**************************************************************************/

	class Endl{}endl;
	QTextStream &operator>>(QTextStream& in, Endl&){
		while(true){
			char s;
			in >> s;

			if (s == '\r' || s == '\n')break;
			}
		return in;
		}

/**************************************************************************/

	class vec2{
	public:
		float x,y;
		};
	QTextStream &operator>>(QTextStream& in, vec2& v){
		in >> v.x >> v.y;
		return in;	
		}

/**************************************************************************/

	class vec3{
	public:
		float x,y,z;
		};
	QTextStream &operator>>(QTextStream& in, vec3& v){
		in >> v.x >> v.y >> v.z;
		return in;	
		}

/**************************************************************************/

	class vec4{
	public:
		float x,y,z,w;
		};
	QTextStream &operator>>(QTextStream& in, vec4& v){
		in >> v.x >> v.y >> v.z >> v.w;
		return in;	
		}

/**************************************************************************/

	class SphereMatsRaw{
	public:
		SphereMatsRaw(){
			for (int i = 0; i < 32; i++)val[i] = 0.0;
			}

		float val[32];
		};
	QTextStream &operator>>(QTextStream& in, SphereMatsRaw& v){
		for (int i = 0 ; i < 32; i++){
			in >> *(v.val+i);
			}
		return in;	
		}

/**************************************************************************/

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

/**************************************************************************/

	class Version{
	public:
		float number;
		};
	QTextStream &operator>>(QTextStream& in, Version& v){
		S o("{"); S c("}"); S number("number");
		in >> o >> number >> v.number >> c >> endl;
		return in;	
		}

/**************************************************************************/

	class Joint{
	public:
		QString name;
		vec3 centre;
		vec4 angles;
		SphereMatsRaw sphereMatsRaw;
		};
	QTextStream &operator>>(QTextStream& in, Joint& j){
		in >> j.name >> endl;
		qDebug() << "Joint " << j.name;
		S o("{"); 
		in >> o;
		while (!in.atEnd()){
			QString tag;
			in >> tag;
			//qDebug() << tag;
			if (tag == "}"){
				return in;
			 	}
			else if (tag == "centre"){
				in >> j.centre >> endl;
				}
			else if (tag == "angles"){
				in >> j.angles >> endl;
				}
			else if (tag == "sphereMatsRaw"){
				in >> j.sphereMatsRaw >> endl; 
				}
			else if (tag == "keys"){
				in >> skip;
				}
			else in >> endl;
			}
		in >> skip;
		return in;
		}

/**************************************************************************/

	class Twist{
	public:
		QString name;
		vec3 centre;
		float startPt;
		float endPt;
		SphereMatsRaw sphereMatsRaw;
		};
	QTextStream &operator>>(QTextStream& in, Twist& t){
		in >> t.name >> endl;
		qDebug() << "Joint " << t.name;
		S o("{"); 
		in >> o;
		while (!in.atEnd()){
			QString tag;
			in >> tag;
			//qDebug() << tag;
			if (tag == "}"){
				return in;
			 	}
			else if (tag == "centre"){
				in >> t.centre >> endl;
				}
			else if (tag == "startPt"){
				in >> t.startPt >> endl;
				}
			else if (tag == "endPt"){
				in >> t.endPt >> endl;
				}
			else if (tag == "sphereMatsRaw"){
				in >> t.sphereMatsRaw >> endl; 
				}
			else if (tag == "keys"){
				in >> skip;
				}
			else in >> endl;
			}
		in >> skip;
		return in;
		}

/**************************************************************************/

	class Channels{
	public:
		Channels(){
			hasJointX = false;
			hasJointY = false;
			hasJointZ = false;
			hasTwistX = false;
			hasTwistY = false;
			hasTwistZ = false;

			}
		bool hasJointX;
		bool hasJointY;
		bool hasJointZ;

		Joint jointx;
		Joint jointy;
		Joint jointz;

		bool hasTwistX;
		bool hasTwistY;
		bool hasTwistZ;
		
		Twist twistx;
		Twist twisty;
		Twist twistz;
		};
	QTextStream &operator>>(QTextStream& in, Channels& c){
		S o("{"); 
		in >> o;
		while (!in.atEnd()){
			QString tag;
			in >> tag;
			//qDebug() << tag;
			if (tag == "}"){
				return in;
			 	}
			else if ( tag == "valueParm" ){
				in >> skip;
				}
			else if ( tag == "jointX"){
				in >> c.jointx >> endl;
				c.hasJointX = true;
				}
			else if ( tag == "jointY"){
				in >> c.jointy >> endl;
				c.hasJointY = true;
				}
			else if ( tag == "jointZ"){
				in >> c.jointz >> endl;
				c.hasJointZ = true;
				}

			else if ( tag == "twistX"){
				in >> c.twistx >> endl;
				c.hasTwistX = true;
				}
			else if ( tag == "twistY"){
				in >> c.twisty >> endl;
				c.hasTwistY = true;
				}
			else if ( tag == "twistZ"){
				in >> c.twistz >> endl;
				c.hasTwistZ = true;
				}

			else in >> skip;
			}
		return in;
		}	

/**************************************************************************/

	class Actor{
	public:
		QString name;
		int name_number;

		char on;
		char bend;
		char dynamicsLock;
		char hidden;
		char addToMenu;
		char castsShadow;
		char includeInDepthCue;
		QString parent;
		QString smartparent;
		QString inkyParent;
		QString nonInkyParent;
		QString conformingTarget;
		//alternateGeom <name> LIST

		vec3 endPoint;
		vec3 origin;
		vec3 orientation;
		char displayOrigin;
		QString displayMode;
		char customMaterial;
		char locked;

		Channels channels;
		};
	QTextStream &operator>>(QTextStream& in, Actor& a){
		S o("{"); 
		in >> o;
		while (!in.atEnd()){
			QString tag;
			in >> tag;
			qDebug() << tag;
			if (tag == "name"){
				QString s1;
				in >> s1;
				if (s1 == "GetStringRes"){
					S co("("); S cc(")"); S ck(","); S c1024("1024");
					in >> co >> c1024 >> ck >> a.name_number >> cc >> endl;
					//use string resources to find the name
					}
				else{
					in >> a.name;
					a.name_number = -1; //search a free bone number later
					}

				}
			else if (tag == "}"){
				qDebug () << "return";
				return in;
				}
			//alternae Geometry
			else if (tag == "alternateGeom"){
				qDebug() << ">>>>       alternateGeom";
				in >> skip;
				}
			//chanels
			else if (tag == "channels"){
				qDebug() << ">>>>      channels ";
				
				in >> a.channels;
				}



			else if (tag == "on"){
				a.on = true;
				in >> endl;
				}
			else if (tag == "off"){
				a.on = false;
				in >> endl;
				}
			else if (tag == "bend"){
				in >> a.bend >> endl; 
				} 
			else if (tag == "dynamicsLock"){
				in >> a.dynamicsLock >> endl;
				}
			else if (tag == "hidden"){
				in >> a.hidden >> endl;
				}
			else if (tag == "addToMenu"){
				in >> a.addToMenu >> endl;
				}
			else if (tag == "castsShadow"){
				in >> a.castsShadow >> endl;
				}
			else if (tag == "includeInDepthCue"){
				in >> a.includeInDepthCue >> endl;
				}
			else if (tag == "parent"){
				in >> a.parent >> endl;
				}
			else if (tag == "smartparent"){
				in >> a.smartparent >> endl;
				}
			else if (tag == "inkyParent"){
				in >> a.inkyParent >> endl;
				}
			else if (tag == "nonInkyParent"){
				in >> a.nonInkyParent >> endl;
				}
			else if (tag == "conformingTarget"){
				in >> a.conformingTarget >> endl;
				}
			else if (tag == "endPoint"){
				in >> a.endPoint >> endl;
				}
			else if (tag == "origin"){
				in >> a.origin >> endl;
				}
			else if (tag == "orientation"){
				in >> a.orientation >> endl;
				}
			else if (tag == "displayOrigin"){
				in >> a.displayOrigin >> endl;
				}
			else if (tag == "displayMode"){
				in >> a.displayMode >> endl;
				}
			else if (tag == "customMaterial"){
				in >> a.customMaterial >> endl;
				}
			else if (tag == "material"){
				in >> skip >> endl;
				}

			else in >>  endl;
			}
		return in;	
		}


}

using namespace cr2;



void Item_node::importCR2(const QString& fn){
	qDebug() << "CR2 importer start";

	QFile file(fn);
	if ( !file.open( QIODevice::ReadOnly ) ) {
		qDebug() << "CR2_importer: File open failed";
		return;
		}


	try{
		QTextStream in( &file ); 
		ParserCodec Parser;
		in.setCodec (&Parser);

		QString f;
		in >> f;
		if(f != "{")throw "not a poser cr2 file";

		QString figureResFile;
		QStringList actorDeclaredList;

		Item_buffer *buf = new Item_buffer(parent(),"Spheres");
		int scount =0;
buf->setDim( 4, 400, 1);

		while (!in.atEnd()){
			QString tag;
			in >> tag;
			//qDebug()<< tag;
	
			if (tag == "version"){
				Version v;
				in >> v;
				qDebug() << "version" << v.number;
				}
			else if (tag == "figureResFile"){
				figureResFile = in.readLine();
				qDebug() << "figureResFile" << figureResFile;
				}
			else if (tag == "actor"){
				QString name; S d(":"); int num;

				in >> name >> d >> num;
				QString n = name;
				n.append(num);

				if(!actorDeclaredList.contains (n)){
					//declare the actor
					actorDeclaredList << n;
					in >> skip; 
					}
				else{
					Actor a;
					in >> a;
					float *f = a.channels.jointx.sphereMatsRaw.val;
					if(a.channels.hasJointX){
						scount++;
						buf->setDim( 4, scount * 8 + 8);

						for ( int i = 0 ; i < 32 ; i += 4){
							qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
							buf->set(scount * 8 + i/4, f[i], f[i+1] ,f[i+2] ,f[i+3]);
							}
						}
					qDebug() ;

/*

					f = a.channels.jointy.sphereMatsRaw.val;
					if(a.channels.hasJointY)for ( int i = 0 ; i < 32 ; i += 4){
						qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
						}
					qDebug() ;


					f = a.channels.jointz.sphereMatsRaw.val;
					if(a.channels.hasJointZ)for ( int i = 0 ; i < 32 ; i += 4){
						qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
						}
					qDebug() ;

					f = a.channels.twistx.sphereMatsRaw.val;
					if(a.channels.hasTwistX)for ( int i = 0 ; i < 32 ; i += 4){
						qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
						}
					qDebug() ;


					f = a.channels.twisty.sphereMatsRaw.val;
					if(a.channels.hasTwistY)for ( int i = 0 ; i < 32 ; i += 4){
						qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
						}
					qDebug() ;


					f = a.channels.twistz.sphereMatsRaw.val;
					if(a.channels.hasTwistZ)for ( int i = 0 ; i < 32 ; i += 4){
						qDebug() << f[i] << f[i+1] << f[i+2] <<f[i+3];
						}
					qDebug() ;
*/
					}


				qDebug() << "actor " << name << num; 

				}
			else if (tag == "figure"){
				qDebug() << "figure";
				in >> skip; 
				}
			}
		}
	catch(char *e){
		qDebug() << "CR2 Importer: " << e;
		}

	file.close();
	}

