#include "Level.h"
#include <iostream>

#ifdef __ANDROID__
#else
//#include </opt/lib/boost/include/boost/lexical_cast.hpp>
#endif

#include "tinyxml.h"
using namespace glm;

Level::Level()
{
	isLoaded = false;
}

Level::Level(const string& filename)
{
	isLoaded = false;
	Load(filename);
}

bool Level::Load(const string& filename)
{
	cout<<"loading level"<<endl;
	TiXmlDocument doc;
	if( !doc.LoadFile( filename.c_str() ) )
	{
		cout<<"couldn't load "<<filename<<endl;
		return false;
	}
	TiXmlHandle hdl(&doc);


	// load the various object in the map :
	TiXmlElement *elem = hdl.FirstChildElement("Level").FirstChildElement("Objects").FirstChildElement("instance").Element();
	while( elem )
	{
		const char* name        = elem->Attribute("name");
		const char* posX        = elem->Attribute("posX");
		const char* posY        = elem->Attribute("posZ"); // ATTENTION ATTENTION
		const char* posZ        = elem->Attribute("posY");
		const char* rotX        = elem->Attribute("rotX");
		const char* rotY        = elem->Attribute("rotZ"); // ATTENTION ATTENTION
		const char* rotZ        = elem->Attribute("rotY");
		const char* scaleX      = elem->Attribute("sclX");
		const char* scaleY      = elem->Attribute("sclY");
		const char* scaleZ      = elem->Attribute("sclZ");
		const char* animated = elem->Attribute("anim");
		float posx   = atof(posX);
		float posy   = atof(posY);
		float posz   = -atof(posZ);
		float rotx   = atof(rotX);
		float roty   = atof(rotY);
		float rotz   = atof(rotZ);
		float scalex = scaleX == NULL ? 1.0 : atof(scaleX);
		float scaley = scaleY == NULL ? 1.0 : atof(scaleY);
		float scalez = scaleZ == NULL ? 1.0 : atof(scaleZ);
		bool  anim   = animated == NULL ? 0 : animated[0] == '1';

		vec3 pos(posx,posy,posz);
		vec3 rot(rotx,roty,rotz);
		cout<<rotx<<","<<roty<<","<<rotz<<endl;
		vec3 scale(scalex,scaley,scalez);
		LevelObject object;
		object.groupName = string(name);
		object.position = pos;
		object.rotation = rot;
		object.scale    = scale;
		object.animated = anim;
		cout<<"object : "<<name<<endl;
		objects.push_back(object);
		elem = elem->NextSiblingElement();
	}

	elem = hdl.FirstChildElement("Level").FirstChildElement("Paths").FirstChildElement("path").Element();
	while( elem )
	{
		const char* name        = elem->Attribute("name");
		LevelPath path;
		path.pathName = string(name);
		cout<<"path : "<<path.pathName<<endl;
		paths.push_back(path);

		elem = elem->NextSiblingElement();
	}

	elem = hdl.FirstChildElement("Level").FirstChildElement("Heightmap").FirstChildElement("instance").Element();
	while( elem )
	{
		const char* name        = elem->Attribute("name");
		const char* posX        = elem->Attribute("posX");
		const char* posY        = elem->Attribute("posY"); // ATTENTION ATTENTION
		const char* posZ        = elem->Attribute("posZ");
		const char* rotX        = elem->Attribute("rotX");
		const char* rotY        = elem->Attribute("rotY"); // ATTENTION ATTENTION
		const char* rotZ        = elem->Attribute("rotZ");
		const char* scaleX      = elem->Attribute("sclX");
		const char* scaleY      = elem->Attribute("sclY");
		const char* scaleZ      = elem->Attribute("sclZ");

		float posx   = atof(posX);
		float posy   = atof(posY);
		float posz   = atof(posZ);
		float rotx   = atof(rotX);
		float roty   = atof(rotY);
		float rotz   = atof(rotZ);
		float scalex = scaleX == NULL ? 1.0 : atof(scaleX);
		float scaley = scaleY == NULL ? 1.0 : atof(scaleY);
		float scalez = scaleZ == NULL ? 1.0 : atof(scaleZ);

		vec3 pos(posx,posy,posz);
		vec3 rot(rotx,roty,rotz);
		cout<<rotx<<","<<roty<<","<<rotz<<endl;
		vec3 scale(scalex,scaley,scalez);
		LevelHeightmap object;
		object.groupName = string(name);
		object.position = pos;
		object.rotation = rot;
		object.scale    = scale;
		cout<<"heightmap : "<<name<<endl;
		heightmaps.push_back(object);
		elem = elem->NextSiblingElement();
	}



	isLoaded = true;
	return true;
}

