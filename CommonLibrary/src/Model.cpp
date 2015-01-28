#include "Model.h"
#include "fichiers.h"
#include "CustomResource.h"

#ifdef __ANDROID__
#include <android/log.h>
#define  LOG_TAG    "montestgljni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

// yeah !
#ifdef __ANDROID__
#define myread(pointer,size) AAsset_read(fp,pointer,size )
#else
#define myread(pointer,size) fread(pointer, size, 1, fp )
#endif

using namespace glm;

Model::Model()
{

}

Model::~Model(void)
{
}

bool Model::Load( const string& filename )
{

#ifdef __ANDROID__
	AAssetDir* assetDir = AAssetManager_openDir(MyAssetManager::get()->mgr, "");
	const char* dirfilename = (const char*)NULL;
	AAsset* fp = NULL;
	while ((dirfilename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		if( strcmp(filename.c_str(), dirfilename) != 0 )
			continue;

		fp = AAssetManager_open(MyAssetManager::get()->mgr, dirfilename, AASSET_MODE_UNKNOWN);
	    break;
	}
    if( fp == NULL )
    {
    	LOGE("Can't find asset %s", filename.c_str());
    	return false;
    }
#else
    FILE *fp = CustomResource_GetFile( filename.c_str() );  //fopen( filename.c_str(), "rb" );
	if( fp == NULL )
	{
		fp = fopen( filename.c_str(), "rb" );
		assert( fp != NULL );
	}
#endif

	// Read each bones :
	int nbBones = 0;
	myread( &nbBones, sizeof(int) );
	for( int i=0; i< nbBones; i++ )
	{
		Bone b;
		if( ! b.read( fp ) )
		{
		//	cout<<"Couldn't read bone"<<endl;
			return false;
		}
		skeleton.push_back(b);
	}

	// Read each meshes :
	int nbMeshes = 0;
	myread( &nbMeshes, sizeof(int) );
	for( int i=0; i< nbMeshes; i++ )
	{
		//cout<<"mesh "<<i<<" sur "<<nbMeshes<<endl;

		Mesh m;
		if( ! m.read( fp ) )
		{
		//	cout<<"Couldn't read mesh"<<endl;
			return false;
		} 
		meshes.push_back(m);
	}

	// read every animations :
	int nbAnim = 0;
	myread( &nbAnim, sizeof(int) );
	for( int i=0; i< nbAnim; i++ )
	{
		BoneAnimation newAnimation;
		if( ! newAnimation.read( fp ) )
		{
			//cout<<"couldn't read animation"<<endl;
			return false;
		}
		animations.push_back( newAnimation );
	}

	// read each materials :
	int nbMat = 0;
	myread( &nbMat, sizeof(int) );
	for( int i=0; i< nbMat; i++ )
	{
		Material mat;
		if( ! mat.read( fp ) )
		{

			//cout<<"couldn't read material"<<endl;
			return false;
		}
		materials.push_back(mat);
	}

#ifdef __ANDROID__
	AAsset_close(fp);
#else
	fclose(fp);
#endif
	return true;
}


//**********************************************//
// Model::Bone
//
//**********************************************//

bool Model::Bone::read( FILESTREAM* fp )
{
	int name_size = 0;

	myread( &name_size, sizeof(int) );
	char* newName = new char[name_size +1];
	myread( newName, sizeof(char) * name_size);
	newName[name_size] = 0;
	name = string(newName);
//	std::cout<<name<<std::endl;
	delete[] newName;
	myread( &localTransform, sizeof(mat4) );
	myread( &worldTransform, sizeof(mat4) );

	myread( &parentIndex, sizeof(int) );
	int nbChilds = 0;
	myread( &nbChilds, sizeof(int) );
	for( int i=0; i< nbChilds; i++ )
	{
		int childIndex = 0;
		myread( &childIndex, sizeof(int) );
		childsIndex.push_back( childIndex );
	}

	return true;
}

//**********************************************//
// Model::MorphAnimation
//
//**********************************************//

Model::MorphAnimation::MorphAnimation()
{
	maxTime = -1.0f;
}

bool Model::MorphAnimation::read( FILESTREAM* fp )
{
	myread( &maxTime, sizeof(float) );

	int nbElems = 0;
	myread( &nbElems, sizeof(int) );
	for( int i=0; i<nbElems; i++ )
	{
		float time = 0;
		myread( &time, sizeof(float) );
		float weight = 0;
		myread( &weight, sizeof(float) );

		morphTimeline.push_back( pair<float,float>( time, weight ) );
	}

	return true;
}

//**********************************************//
// Model::ShapeKey
//
//**********************************************//

Model::ShapeKey::ShapeKey()
{
	name           = "morph";
	nbVertices     = 0;
	hasPositions   = false;
	hasNormales    = false;
	hasTexCoords   = false;
	hasTangentes   = false;
	hasBitangentes = false;
}

bool Model::ShapeKey::read( FILESTREAM* fp )
{
	int name_size = 0;
	myread( &name_size, sizeof(int) );
	char* name_c = new char[name_size+1];
	myread( name_c, sizeof(char) * name_size );
	name_c[name_size] = 0;
	name = string(name_c);
	delete[] name_c;

	myread( &nbVertices, sizeof(int) );

	myread( &hasPositions, sizeof(bool) );
	if( hasPositions )
	{
		vertex_meshanim_positions = new vec3[nbVertices];
		myread( vertex_meshanim_positions, sizeof(vec3) * nbVertices );
	}

	myread( &hasNormales, sizeof(bool) );
	if( hasNormales )
	{
		vertex_meshanim_normales  = new vec3[nbVertices];
		myread( vertex_meshanim_normales, sizeof(vec3) * nbVertices );
	}

	myread( &hasTexCoords, sizeof(bool) );
	if( hasTexCoords )
	{
		vertex_meshanim_texcoords = new vec2[nbVertices];
		myread( vertex_meshanim_texcoords, sizeof(vec2) * nbVertices );
	}

	myread( &hasTangentes, sizeof(bool) );
	if( hasTangentes )
	{
		vertex_meshanim_tangentes = new vec3[nbVertices];
		myread( vertex_meshanim_tangentes, sizeof(vec3) * nbVertices );
	}

	myread( &hasBitangentes, sizeof(bool) );
	if( hasBitangentes )
	{
		vertex_meshanim_bitangentes = new vec3[nbVertices];
		myread( vertex_meshanim_bitangentes, sizeof(vec3) * nbVertices );
	}

	animation.read(fp);

	return true;
}

//**********************************************//
// Model::Mesh
//
//**********************************************//

bool Model::Mesh::read( FILESTREAM* fp )
{
	int nbName = 0;
	myread( &nbName, sizeof( int ) );
	if( nbName > 0 )
	{
		char* myName = new char[nbName+1];
		myread( myName, sizeof(char)*nbName );
		myName[nbName] = 0;
		name = std::string( myName );
		delete[] myName;
	}
	 
	myread( &nbVertices, sizeof(int) );
	
	vertex_positions   = new vec3[nbVertices];
	vertex_normales    = new vec3[nbVertices];
	vertex_uvs         = new vec2[nbVertices];
	vertex_tangentes   = new vec3[nbVertices];
	vertex_bitangentes = new vec3[nbVertices];

	// allocate the vectors :
	myread( vertex_positions,   sizeof(vec3) * nbVertices );
	myread( vertex_normales,    sizeof(vec3) * nbVertices );
	myread( vertex_uvs,         sizeof(vec2) * nbVertices );
	myread( vertex_tangentes,   sizeof(vec3) * nbVertices );
	myread( vertex_bitangentes, sizeof(vec3) * nbVertices );

	myread( &hasSecondUVSet, sizeof(bool) );
	myread( &nodeIndex, sizeof(int) );

	hasBones = false; 
	myread( &hasBones, sizeof(bool) );
	if( hasBones )
	{
		bonesWeight      = new vec4[nbVertices];
		bonesIndex       = new ivec4[nbVertices];
		myread( bonesWeight, sizeof(vec4) * nbVertices );
		myread( bonesIndex, sizeof(ivec4) * nbVertices );
	}

	myread( &nbTriangles, sizeof(int) );
	indices = new ivec3[nbTriangles];
	myread( indices, sizeof(ivec3) * nbTriangles );

	int nbBones = 0; 
	myread( &nbBones, sizeof(int) );
	for( int i=0; i< nbBones; i++ )
	{
		int boneIndex = 0;
		mat4 bindPose;
		myread( &boneIndex, sizeof(int) );
		myread( &bindPose, sizeof(mat4) );
		boneIndexOffset.push_back( pair<int,mat4>( boneIndex, bindPose ) );
	}

	myread( &materialIndex, sizeof(unsigned int) );

	myread( &numMeshAnim, sizeof(int) );
	shapekeys = new ShapeKey[numMeshAnim];
	for( int i=0; i< numMeshAnim; i++ )
	{
		ShapeKey key;
		key.read( fp );

		shapekeys[i] = key;
	}


	return true;
}

//**********************************************//
// Model::BoneAnimation
//
//**********************************************//

bool Model::BoneAnimation::read( FILESTREAM* fp )
{	
	myread( &duration, sizeof(float) );
	myread( &ticksPerSecond, sizeof(float) );

	int nbChannels = 0; // 1 channel, 1 bone
	myread( &nbChannels, sizeof(int) );
	for( int i=0; i<nbChannels; i++ )
	{
		BoneAnimationChannel newChannel;
		newChannel.read(fp);
		channels.push_back(newChannel);
	}
	
	return true;
}



bool BoneAnimationChannel::read( FILESTREAM* fp )
{
	myread( &boneIndex, sizeof(int) );
	int nbKeys = 0;
	myread( &nbKeys, sizeof(int) );
	for( int i=0; i< nbKeys; i++ )
	{
		BoneAnimationChannel::LocRotScale newLocRotScale;
		myread( &(newLocRotScale.time), sizeof(float) );
		myread( &(newLocRotScale.rotation), sizeof(Quaternion) );
		myread( &(newLocRotScale.position), sizeof(vec3) );
		myread( &(newLocRotScale.scale), sizeof(vec3) );

		locrotscale.push_back( newLocRotScale );
	}

	return true;
}


//**********************************************//
// Model::Material
//
//**********************************************//

bool Model::Material::read( FILESTREAM* fp )
{
	int matNameSize = 0;
	myread( &matNameSize, sizeof(int) );
	char* matNewName = new char[matNameSize +1];
	myread( matNewName, sizeof(char) * matNameSize );
	matNewName[matNameSize] = 0;
	name = string(matNewName);

	myread( &hasDiffuseTexture, sizeof(bool) );
	if( hasDiffuseTexture )
	{
		int name_size = 0;
		myread( &name_size, sizeof(int) );
		char* newName = new char[name_size +1];
		myread( newName, sizeof(char) * name_size );
		newName[name_size] = 0;
		diffuse_texture = string(newName);
	}

	myread( &hasNormalTexture, sizeof(bool) );
	if( hasNormalTexture )
	{
		int name_size = 0;
		myread( &name_size, sizeof(int) );
		char* newName = new char[name_size +1];
		myread( newName, sizeof(char) * name_size );
		newName[name_size] = 0;
		normal_texture = string(newName);
	}

	myread( &hasSpecularTexture, sizeof(bool) );
	if( hasSpecularTexture )
	{
		int name_size = 0;
		myread( &name_size, sizeof(int) );
		char* newName = new char[name_size +1];
		myread( newName, sizeof(char) * name_size );
		newName[name_size] = 0;
		specular_texture = string(newName);
	}

	return true;
}



#ifndef __ANDROID__



//****************************
// Write functions

//****************************


//**********************************************//
// Model::Material
//
//**********************************************//

bool Model::Material::write( FILE* fp )
{
	int matNameSize = name.size();
	fwrite( &matNameSize, sizeof(int), 1, fp );
	fwrite( name.c_str(), sizeof(char), matNameSize, fp );

	fwrite( &hasDiffuseTexture, sizeof(bool), 1, fp );
	if( hasDiffuseTexture )
	{
		int name_size = diffuse_texture.size();
		fwrite( &name_size, sizeof(int), 1, fp );
		fwrite( diffuse_texture.c_str(), sizeof(char), name_size, fp );
	}

	fwrite( &hasNormalTexture, sizeof(bool), 1, fp );
	if( hasNormalTexture )
	{
		int name_size = normal_texture.size();
		fwrite( &name_size, sizeof(int), 1, fp );
		fwrite( normal_texture.c_str(), sizeof(char), name_size, fp );
	}

	fwrite( &hasSpecularTexture, sizeof(bool), 1, fp );
	if( hasSpecularTexture )
	{
		int name_size = specular_texture.size();
		fwrite( &name_size, sizeof(int), 1, fp );
		fwrite( specular_texture.c_str(), sizeof(char), name_size, fp );
	}

	return true;
}

//**********************************************//
// Model::BoneAnimation
//
//**********************************************//

bool Model::BoneAnimation::write( FILE* fp )
{
	fwrite( &duration, sizeof(float), 1, fp );
	fwrite( &ticksPerSecond, sizeof(float), 1, fp );

	int nbChannels = channels.size(); // 1 channel, 1 bone
	fwrite( &nbChannels, sizeof(int), 1, fp );
	for( int i=0; i<nbChannels; i++ )
	{
		channels[i].write(fp);
	}

	return true;
}

bool BoneAnimationChannel:: write( FILE* fp )
{
	fwrite( &boneIndex, sizeof(int), 1, fp );
	int nbKeys = locrotscale.size();
	fwrite( &nbKeys, sizeof(int), 1, fp );
	for( int i=0; i< nbKeys; i++ )
	{
		fwrite( &(locrotscale[i].time), sizeof(float), 1, fp );
		fwrite( &(locrotscale[i].rotation), sizeof(Quaternion), 1, fp );
		fwrite( &(locrotscale[i].position), sizeof(vec3), 1, fp );
		fwrite( &(locrotscale[i].scale), sizeof(vec3), 1, fp );
	}

	return true;
}


//**********************************************//
// Model::Mesh
//
//**********************************************//

bool Model::Mesh::write( FILE* fp )
{
	int nbName = name.size();
	fwrite( &nbName, sizeof( int ), 1, fp );
	if( nbName > 0 )
	{
		fwrite( name.c_str(), sizeof(char), nbName, fp );
	}

	fwrite( &nbVertices, sizeof(int), 1, fp );

	fwrite( vertex_positions, sizeof(vec3), nbVertices, fp );
	fwrite( vertex_normales, sizeof(vec3), nbVertices, fp );
	fwrite( vertex_uvs, sizeof(vec2), nbVertices, fp );
	fwrite( vertex_tangentes, sizeof(vec3), nbVertices, fp );
	fwrite( vertex_bitangentes, sizeof(vec3), nbVertices, fp );

	fwrite( &hasSecondUVSet, sizeof(bool), 1, fp );
	fwrite( &nodeIndex, sizeof(int), 1, fp );

	fwrite( &hasBones, sizeof(bool), 1, fp );
	if( hasBones )
	{
		fwrite( bonesWeight, sizeof(vec4), nbVertices, fp );
		fwrite( bonesIndex, sizeof(ivec4), nbVertices, fp );
	}

	fwrite( &nbTriangles, sizeof(int), 1, fp );
	fwrite( indices, sizeof(ivec3), nbTriangles, fp );

	int nbBones = boneIndexOffset.size();
	fwrite( &nbBones, sizeof(int),1, fp);
	for( int i=0; i< nbBones; i++ )
	{
		int boneIndex = boneIndexOffset[i].first;
		mat4 bindPose = boneIndexOffset[i].second;
		fwrite( &boneIndex, sizeof(int), 1, fp );
		fwrite( &bindPose, sizeof(mat4), 1, fp );
	}

	fwrite( &materialIndex, sizeof(unsigned int),1, fp);

	fwrite( &numMeshAnim, sizeof(int), 1, fp );
	for( int i=0; i< numMeshAnim; i++ )
	{
		shapekeys[i].write( fp );
	}


	return true;
}

//**********************************************//
// Model::ShapeKey
//
//**********************************************//

bool Model::ShapeKey::write( FILE* fp )
{
	int name_size = name.size();
	fwrite( &name_size, sizeof(int), 1, fp );
	fwrite( name.c_str(), sizeof(char), name_size, fp );

	fwrite( &nbVertices, sizeof(int), 1, fp );

	fwrite( &hasPositions, sizeof(bool), 1, fp );
	if( hasPositions )
	{
		fwrite( vertex_meshanim_positions, sizeof(vec3), nbVertices, fp );
	}

	fwrite( &hasNormales, sizeof(bool), 1, fp );
	if( hasNormales )
	{
		fwrite( vertex_meshanim_normales, sizeof(vec3), nbVertices, fp );
	}

	fwrite( &hasTexCoords, sizeof(bool), 1, fp );
	if( hasTexCoords )
	{
		fwrite( vertex_meshanim_texcoords, sizeof(vec2), nbVertices, fp );
	}

	fwrite( &hasTangentes, sizeof(bool), 1, fp );
	if( hasTangentes )
	{
		fwrite( vertex_meshanim_tangentes, sizeof(vec3), nbVertices, fp );
	}

	fwrite( &hasBitangentes, sizeof(bool), 1, fp );
	if( hasBitangentes )
	{
		fwrite( vertex_meshanim_bitangentes, sizeof(vec3), nbVertices, fp );
	}

	animation.write(fp);

	return true;
}

//**********************************************//
// Model::MorphAnimation
//
//**********************************************//

bool Model::MorphAnimation::write( FILE* fp )
{
	fwrite( &maxTime, sizeof(float), 1, fp );

	int nbElems = morphTimeline.size();
	fwrite( &nbElems, sizeof(int), 1, fp );
	for( int i=0; i<nbElems; i++ )
	{
		float time = morphTimeline[i].first;
		fwrite( &time, sizeof(float), 1, fp );
		float weight = morphTimeline[i].second;
		fwrite( &weight, sizeof(float), 1, fp );
	}

	return true;
}

//**********************************************//
// Model::Bone
//
//**********************************************//

bool Model::Bone::write( FILE* fp )
{
	int name_size = name.size();
	fwrite( &name_size, sizeof(int), 1, fp );
	fwrite( name.c_str(), sizeof(char), name.size(), fp );
	fwrite( &localTransform, sizeof(mat4), 1, fp);
	fwrite( &worldTransform, sizeof(mat4), 1, fp);

	fwrite( &parentIndex, sizeof(int), 1, fp);
	int nbChilds = childsIndex.size();
	fwrite( &nbChilds, sizeof(int), 1, fp );
	for( int i=0; i< nbChilds; i++ )
	{
		fwrite( &childsIndex[i], sizeof(int), 1, fp );
	}

	return true;
}


bool Model::Save( const string& filename )
{
	FILE *fp = fopen( filename.c_str(), "wb" );
	if( fp == NULL )
	{
		return false;
	}

	// Write each bones :
	int nbBones = skeleton.size();
	fwrite( &nbBones, sizeof(int), 1, fp);
	for( auto it = skeleton.begin(); it != skeleton.end(); it++ )
	{
		(*it).write( fp );
	}

	// Write each meshes :
	int nbMeshes = meshes.size();
	fwrite( &nbMeshes, sizeof(int), 1, fp);
	for( auto it = meshes.begin(); it != meshes.end(); it++ )
	{
		(*it).write(fp);
	}

	// write every animations :
	int nbAnim = animations.size();
	fwrite( &nbAnim, sizeof(int), 1, fp);
	for( auto it = animations.begin(); it != animations.end(); it++ )
	{
		(*it).write( fp );
	}

	// write each materials :
	int nbMat = materials.size();
	fwrite( &nbMat, sizeof(int), 1, fp);
	for( auto it = materials.begin(); it != materials.end(); it++ )
	{
		(*it).write( fp );
	}


	fclose(fp);

	return true;
}

bool Model::testImportExport( const string& filename )
{
	if( ! Save( "test.b" ) )
	{
		//cout<<"couldn't save to test.b"<<endl;
		return false;
	};
	if( ! Load( "test.b" ) )
	{
		//cout<<"couldn't load "<<filename<<endl;
		return false;
	}

	// make sure filename and test.b have the same size and content :
	FILE* fp = fopen( filename.c_str(), "rb" );
	FILE* fp2 = fopen( "test.b", "rb" );

	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	rewind (fp);

	fseek(fp2, 0L, SEEK_END);
	long sz2 = ftell(fp2);
	rewind (fp2);

	if( sz != sz2 )
	{
		//cout<<"taille differente "<<sz<<" vs "<<sz2<<endl;
		fclose(fp);
		fclose(fp2);
		return false;
	}

	char* data1 = new char[sz];
	char* data2 = new char[sz2];

	fread( data1, sizeof(char), sz, fp );
	fread( data2, sizeof(char), sz2, fp2 );

	for( int i=0; i< sz; i++ )
	{
		if( data1[i] != data2[i] )
		{
			//cout<<"different data at position "<<i<<endl;
			delete[] data1;
			delete[] data2;
			fclose(fp);
			fclose(fp2);
			return false;
		}
	}


	delete[] data1;
	delete[] data2;
	fclose(fp);
	fclose(fp2);
	return true;
}

#endif







