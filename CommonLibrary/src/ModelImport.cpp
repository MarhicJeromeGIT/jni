#include "ModelImport.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

#ifdef __ANDROID__

ModelImport::ModelImport()
{
}

#else

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;


#include <algorithm>


//#include <libgen.h>


ModelImport::ModelImport()
{
}

bool ModelImport::Import( const aiScene* scene, Model* newModel )
{
	if( scene == NULL || newModel == NULL )
	{
		return false;
	}

	// get the bone information :
	if( ! ImportBones( scene, newModel ) )
	{
		cout<<"Couldn't import bones"<<endl;
		return false;
	};
	// get the meshes information :
	if( ! ImportMeshes( scene, newModel ) )
	{
		cout<<"Couldn't import meshes"<<endl;
		return false;
	};
	// get the bone animation :
	if( !ImportBoneAnimations(scene, newModel ) )
	{
		cout<<"Couldn't import animations"<<endl;
		return false;
	}
	// get the shapekey animation
	if( !ImportMorphAnimations(scene, newModel ) )
	{
		cout<<"Coulnd't import morph animations"<<endl;
		return false;
	}
	// get the materials :
	if( !ImportMaterials(scene, newModel ) )
	{
		cout<<"Couldn't import materials"<<endl;
		return false;
	}

	cout<<"model "<<newModel->modelName<<" loaded successfully"<<endl;
	return true;
}

// return the bone index of the last bone created
int ModelImport::createSkeleton( const aiNode* node, int parentIndex, vector<Model::Bone>* skeleton )
{
	Model::Bone newBone;

	mat4 localTransformation;	
	for( int i=0; i<4; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			localTransformation[i][j] = node->mTransformation[i][j];
		}
	}
	
	newBone.localTransform = transpose(localTransformation);
	newBone.parentIndex = parentIndex; // -1 : root

	// compute the worldTransform from the parent :
	if( newBone.parentIndex == -1 ) // root
	{
		newBone.worldTransform = newBone.localTransform;
	}
	else
	{
		newBone.worldTransform = (*skeleton)[ newBone.parentIndex ].worldTransform * newBone.localTransform;
	}

	newBone.name = string( node->mName.C_Str() );
	cout<<newBone.name<<endl;

	skeleton->push_back( newBone );
	int thisBoneIndex = skeleton->size()-1;
	for( int i=0 ; i< node->mNumChildren; i++ )
	{
		int child = createSkeleton( node->mChildren[i] , thisBoneIndex, skeleton );
		(*skeleton)[thisBoneIndex].childsIndex.push_back(child);
	}
	
	return thisBoneIndex;
}

bool ModelImport::ImportBones( const aiScene* scene, Model* newModel )
{
	const aiNode* rootnode = scene->mRootNode;
	createSkeleton( rootnode, -1, &(newModel->skeleton) );

	if( newModel->skeleton.size() > 150 )
	{
		cout<<"Too many bones ! 150 supported"<<endl;
	//	return false;
	}

	return true;
}

// used to sort vector of weights
bool ModelImport_sortWeights ( const pair<int,float>& a, const pair<int,float>& b )
{
	return a.second > b.second;
}

bool ModelImport::ImportMeshes( const aiScene* scene, Model* newModel )
{
	for( int m=0; m< scene->mNumMeshes; m++ )
	{
		aiMesh* mesh = scene->mMeshes[m];

		Model::Mesh newMesh;
	
		newMesh.name = std::string( mesh->mName.C_Str() );
		newMesh.nbVertices = mesh->mNumVertices;
		newMesh.vertex_positions   = new vec3[newMesh.nbVertices];
		newMesh.vertex_normales    = new vec3[newMesh.nbVertices];
		newMesh.vertex_tangentes   = new vec3[newMesh.nbVertices];
		newMesh.vertex_bitangentes = new vec3[newMesh.nbVertices];
		newMesh.vertex_uvs         = new vec2[newMesh.nbVertices];
		newMesh.hasBones 		   = true; //mesh->HasBones();
		newMesh.nodeIndex          = -1;

		newMesh.hasSecondUVSet = false;
		if( newMesh.hasBones )
		{
			newMesh.bonesIndex  = new ivec4[newMesh.nbVertices];
			newMesh.bonesWeight = new vec4[newMesh.nbVertices];
		}
		for( int k = 0; k < mesh->mNumVertices; k++ )
		{
			if( mesh->HasPositions() )
			{
				aiVector3D pos = mesh->mVertices[k];
				newMesh.vertex_positions[k] = vec3( pos.x, pos.y, pos.z );
			}

			if( mesh->HasNormals() )
			{
				aiVector3D norm = mesh->mNormals[k];
				newMesh.vertex_normales[k] = vec3( norm.x, norm.y, norm.z );
			}
			else
			{
				newMesh.vertex_normales[k] = vec3(0,1,0);
			}

			if( mesh->HasTangentsAndBitangents() )
			{
				aiVector3D tang   = mesh->mTangents[k];
				aiVector3D bitang = mesh->mBitangents[k];
				newMesh.vertex_tangentes[k]   = vec3(tang.x, tang.y, tang.z);
				newMesh.vertex_bitangentes[k] = vec3(bitang.x,bitang.y,bitang.z);
			}
			else
			{
				newMesh.vertex_tangentes[k]   = vec3(1.0,0.0,0.0);
				newMesh.vertex_bitangentes[k] = vec3(0.0,1.0,0.0);
			}

			if( mesh->HasTextureCoords(0) )
			{
				aiVector3D uv = mesh->mTextureCoords[0][k]; // [0] -> texture index
				newMesh.vertex_uvs[k] = vec2( uv.x, uv.y );
			}
			else
			{
				newMesh.vertex_uvs[k] = vec2( 0, 0);
			}

			if( newMesh.hasBones )
			{
				vector< pair<int, float> > boneIndexAndWeight;

				// for this vertice, find all weights and keep the 4 biggest ones :
				for( int b = 0; b < mesh->mNumBones; b++ )
				{
					aiBone* bone = mesh->mBones[b];
					for( int w = 0; w < bone->mNumWeights; w++ )
					{
						int vertexIndex = bone->mWeights[w].mVertexId;
						if( vertexIndex == k )
						{
							float weight    = bone->mWeights[w].mWeight;
							// find the bone index :
							int boneIndex   = -1;
							for( int bi=0; bi< newModel->skeleton.size(); bi++ )
							{
								if( strcmp( bone->mName.C_Str() , newModel->skeleton[bi].name.c_str() ) == 0 )
								{
									boneIndex = bi;
									break;
								}
							}
							if( boneIndex == -1 )
							{
								cout<<"couldn't find bone "<<bone->mName.C_Str()<<" when loading mesh "<<mesh->mName.C_Str()<<endl;
								return false;
							}

							boneIndexAndWeight.push_back( pair<int,float>( boneIndex, weight ) );
						}
					}
				}
				if( boneIndexAndWeight.size() == 0 )
				{
					// No bone deforming this mesh. it can be a hard shell (like an armor)
					// never deforming, so just set the root bone.
					//assert( boneIndexAndWeight.size() != 0 );
					boneIndexAndWeight.push_back( pair<int,float>(0,1.0) );
				}
				if( boneIndexAndWeight.size() > 4.0 )
				{
					sort( boneIndexAndWeight.begin(), boneIndexAndWeight.end(), ModelImport_sortWeights );
					assert(boneIndexAndWeight[0].second >= boneIndexAndWeight[1].second );
				}
				
				ivec4 vertexBoneIndexes = ivec4(0,0,0,0);
				vec4 vertexBoneWeights = vec4(0,0,0,0);
				for( int idx = 0; idx < std::min( 4, (int)boneIndexAndWeight.size() ); idx++ )
				{
					vertexBoneIndexes[idx] = boneIndexAndWeight[idx].first;
					vertexBoneWeights[idx] = boneIndexAndWeight[idx].second;
				}
				vertexBoneWeights = normalize(vertexBoneWeights);

				newMesh.bonesIndex[k]  = vertexBoneIndexes;
				//cout<<"bone "<<newMesh.bonesIndex[k].x<<","<<newMesh.bonesIndex[k].y<<"'"<<newMesh.bonesIndex[k].z<<endl;
				newMesh.bonesWeight[k] = vertexBoneWeights;
			}
		}
		// mesh material
		newMesh.materialIndex = mesh->mMaterialIndex;
		cout<<"mesh with "<<mesh->mNumFaces<<" triangles : material "<<newMesh.materialIndex<<endl;

		// Get the name of the mesh. It's more complicated than it should.
		aiNode* thisMeshNode = NULL;
		for( int i=0; i< scene->mRootNode->mNumChildren; i++ )
		{
			for( int j=0; j< scene->mRootNode->mChildren[i]->mNumMeshes; j++ )
			{
				if( scene->mRootNode->mChildren[i]->mMeshes[j] == m )
				{
					// we found it !
					thisMeshNode = scene->mRootNode->mChildren[i];
					break;
				}
			}
		}
		if( thisMeshNode != NULL )
		{
			newMesh.name = string( thisMeshNode->mName.C_Str() );
			cout<<"NEW MESH FOUND :"<<newMesh.name<<endl;
		}

		// Import the shapekeys
		// REMOVING morph animation support
		newMesh.numMeshAnim = 0;
		newMesh.shapekeys = NULL;
/*
		newMesh.numMeshAnim = mesh->mNumAnimMeshes;
		newMesh.shapekeys = new Model::ShapeKey[newMesh.numMeshAnim];
		for( int anim=0; anim< mesh->mNumAnimMeshes; anim++ )
		{
			Model::ShapeKey key;
			key.name = string( mesh->mAnimMeshes[anim]->mName.C_Str() );
			cout<<"shapekey name = "<<key.name<<endl;

			key.nbVertices = newMesh.nbVertices;
			key.hasPositions = mesh->mAnimMeshes[anim]->HasPositions();
			if( key.hasPositions )
			{
				key.vertex_meshanim_positions = new vec3[mesh->mNumVertices];
				for( int i=0; i< mesh->mNumVertices; i++ )
				{
					// I store the difference between the bind pose position and the morph position,
					// so I can simply add it after (instead of interpolating).
					aiVector3D morphpos = mesh->mAnimMeshes[anim]->mVertices[i];
					aiVector3D bindpos  = mesh->mVertices[i];
					key.vertex_meshanim_positions[i] = vec3( morphpos.x - bindpos.x, morphpos.y - bindpos.y, morphpos.z - bindpos.z );
				}
			}

			key.hasNormales = mesh->mAnimMeshes[anim]->HasNormals();
			if( key.hasNormales )
			{
				key.vertex_meshanim_normales = new vec3[mesh->mNumVertices];
				for( int i=0; i< mesh->mNumVertices; i++ )
				{
					aiVector3D normale = mesh->mAnimMeshes[anim]->mNormals[i];
					key.vertex_meshanim_normales[i] = vec3( normale.x, normale.y, normale.z );
				}
			}

			key.hasTexCoords = mesh->mAnimMeshes[anim]->HasTextureCoords(0);
			if( key.hasTexCoords )
			{
				key.vertex_meshanim_texcoords = new vec2[mesh->mNumVertices];
				for( int i=0; i< mesh->mNumVertices; i++ )
				{
					aiVector3D texcoord = mesh->mAnimMeshes[anim]->mTextureCoords[0][i];
					key.vertex_meshanim_texcoords[i] = vec2( texcoord.x, texcoord.y );
				}
			}

			key.hasTangentes   = mesh->mAnimMeshes[anim]->HasTangentsAndBitangents();
			key.hasBitangentes = mesh->mAnimMeshes[anim]->HasTangentsAndBitangents();
			if( key.hasTangentes )
			{
				key.vertex_meshanim_tangentes   = new vec3[mesh->mNumVertices];
				key.vertex_meshanim_bitangentes = new vec3[mesh->mNumVertices];
				for( int i=0; i< mesh->mNumVertices; i++ )
				{
					aiVector3D tangente = mesh->mAnimMeshes[anim]->mTangents[i];
					key.vertex_meshanim_tangentes[i] = vec3( tangente.x, tangente.y, tangente.z );
					aiVector3D bitangente = mesh->mAnimMeshes[anim]->mBitangents[i];
					key.vertex_meshanim_bitangentes[i] = vec3( bitangente.x, bitangente.y, bitangente.z );
				}
			}

			newMesh.shapekeys[anim] = key;
		}
*/

		newMesh.nbTriangles = 0;
		newMesh.indices = new ivec3[ mesh->mNumFaces ];
		for( int k=0; k< mesh->mNumFaces; k++ )
		{
			aiFace face = mesh->mFaces[k];
			// make sure it's a triangle
		    if( face.mNumIndices == 3 )
			{
				newMesh.indices[k] = ivec3( face.mIndices[0], face.mIndices[1], face.mIndices[2] );
				newMesh.nbTriangles++;
			}
			else if( face.mNumIndices == 2 ) // line
			{
				newMesh.lineIndices.push_back( ivec2( face.mIndices[0], face.mIndices[1] ) );
			}
		}

		for( int b=0; b< mesh->mNumBones; b++ )
		{
			aiBone* bone = mesh->mBones[b];

			// trouve le Bone correspondant :
			int boneIndex = -1;
			for( int bi=0; bi< newModel->skeleton.size(); bi++ )
			{
				if( strcmp( bone->mName.C_Str() , newModel->skeleton[bi].name.c_str() ) == 0 )
				{
					boneIndex = bi;
					break;
				}
			}
			if( boneIndex == -1 )
			{
				cout<<"couldn't find bone "<<bone->mName.C_Str()<<" when loading mesh "<<mesh->mName.C_Str()<<endl;
				return false;
			}

			mat4 bindPose;					
			for( int ii=0; ii<4; ii++ )
			{
				for( int j=0; j<4; j++ )
				{
					bindPose[ii][j] = bone->mOffsetMatrix[ii][j];
				}
			}
			newMesh.boneIndexOffset.push_back( pair<int,mat4>( boneIndex, bindPose ) );
		}
		
		newModel->meshes.push_back( newMesh );
	}
	return true;
}


bool ModelImport::ImportBoneAnimations( const aiScene* scene, Model* newModel )
{
	// make sure the bones have been loaded already
	if( !scene->HasAnimations() )
	{
		return true;
	}

	// This is the bones animation
	for( int anim = 0; anim < scene->mNumAnimations; anim++ )
	{
		Model::BoneAnimation newAnimation;
		aiAnimation* animation = scene->mAnimations[anim];
		
		newAnimation.ticksPerSecond = animation->mTicksPerSecond;
		newAnimation.duration       = animation->mDuration;

		for( int i=0; i< animation->mNumChannels; i++ )
		{
			BoneAnimationChannel newChannel;
			aiNodeAnim* nodeAnim = scene->mAnimations[anim]->mChannels[i];

			aiString nodeName = nodeAnim->mNodeName;

			// find the bone index of this channel
			// Tout le monde a un root bone, meme quand j'ai pas cree de rig
			int boneIndex = -1;
			for( int bone=0; bone< newModel->skeleton.size(); bone++ )
			{
				if( strcmp( newModel->skeleton[bone].name.c_str(), nodeAnim->mNodeName.C_Str() ) == 0 )
				{
					boneIndex = bone;
					break;
				}
			}
			if( boneIndex == -1 )
			{
				cout<<"couldn't find bone "<<nodeAnim->mNodeName.C_Str()<<endl;
				return false;
			}
			newChannel.boneIndex = boneIndex;			

			// non-skeletal animation :
			aiNode* mainNode = scene->mRootNode->FindNode(nodeName );
			assert(mainNode != NULL );
			cout<<"node has "<<mainNode->mNumMeshes<<" meshes"<<endl;
			if( mainNode->mNumMeshes > 0 )
			{
				cout<<"mesh index "<<mainNode->mMeshes[0]<<endl;

				for( int m=0; m< mainNode->mNumMeshes; m++ )
				{
					assert( newModel->meshes[ mainNode->mMeshes[m] ].nodeIndex == -1 );
					newModel->meshes[ mainNode->mMeshes[m] ].nodeIndex = boneIndex;
				}
			}

			int nbKeyframes = nodeAnim->mNumPositionKeys;
			if( nbKeyframes != nodeAnim->mNumRotationKeys || nbKeyframes != nodeAnim->mNumScalingKeys )
			{
				cout<<"numPosition keys doesn't match with numRotations keys or numScalingKeys"<<endl;
				assert(false);
				return false;
			}
			for( int key=0; key< nbKeyframes; key++ )
			{
				BoneAnimationChannel::LocRotScale locrotscale;
				
				// write this in our model:
				locrotscale.time = nodeAnim->mPositionKeys[key].mTime;

				aiVector3D pos = nodeAnim->mPositionKeys[key].mValue;
				locrotscale.position = vec3(pos.x, pos.y, pos.z);

			    aiQuaternion rotA = nodeAnim->mRotationKeys[key].mValue;
				locrotscale.rotation = Quaternion( rotA.x, rotA.y, rotA.z, rotA.w );

				aiVector3D scl = nodeAnim->mScalingKeys[key].mValue;
				locrotscale.scale = vec3( scl.x, scl.y, scl.z );

				newChannel.locrotscale.push_back( locrotscale );
			}

			newAnimation.channels.push_back( newChannel );
		}

		newModel->animations.push_back( newAnimation );
	}

	return true;
}

// Do that only after we created the mesh and their shapekeys
bool ModelImport::ImportMorphAnimations( const aiScene* scene, Model* newModel )
{
// Update 01/26/2015 
// Removing Morph anim support for now. It requires modification to the assimp code AND blender source code.
//

/*	
// make sure the bones have been loaded already
	if( !scene->HasAnimations() )
	{
		return true;
	}

	for( int anim = 0; anim < scene->mNumAnimations; anim++ )
	{
		aiAnimation* animation = scene->mAnimations[anim];

		// This is the morph animation (time->weight)
		for( int mesh=0; mesh < animation->mNumMeshChannels; mesh++ )
		{
			aiMeshAnim* animMesh = animation->mMeshChannels[mesh];

			// for each keyframes, store the weight of each of the morph
			for( int morph=0; morph< animMesh->mNumMorph; morph++ )
			{
				aiMorphAnim* morphAnim = animMesh->mMorphs[morph];

				string shapeName = string( morphAnim->mName.C_Str() ); // name of the shapekey

				Model::ShapeKey* shapekey = NULL;
				// look for a shapekey with the same name in all our meshes (how messy is that !) :
				for( int m=0; m< newModel->meshes.size(); m++ )
				{
					for( int s=0; s< newModel->meshes[m].numMeshAnim; s++ )
					{
						if( newModel->meshes[m].shapekeys[s].name == shapeName )
						{
							shapekey = &(newModel->meshes[m].shapekeys[s]);
							break;
						}
					}
				}
				assert(shapekey != NULL);


				for( int i=0; i< morphAnim->mNumKeys; i++ )
				{
					aiWeightKey key = morphAnim->mKeys[i]; // a [time;weight] key

					shapekey->animation.morphTimeline.push_back( pair<float,float>( key.mTime, key.mValue ) );
				}
				shapekey->animation.maxTime = shapekey->animation.morphTimeline[shapekey->animation.morphTimeline.size()-1].first;
			}
		}
	}
*/
	return true;
}

static bool getTextureName( aiMaterial* material, aiTextureType type, string& name)
{
	aiString path;	// filename
	aiReturn texFound;

	texFound = material->GetTexture(type, 0, &path);
	if( texFound != aiReturn_SUCCESS )
	{
		return false;
	}

	string cleanPath = string(path.C_Str());
	for( int i=0; i< cleanPath.length(); i++ )
	{
		// pff vla le merdier
		if( cleanPath[i] == '\\' || cleanPath[i] == '/' )
		{
		//	boost::filesystem::path slash("/");
		//	std::string preferredSlash = slash.make_preferred().native();
		//	cleanPath[i] = preferredSlash[0];
		}
	}

	// Remove directory if present.
	// Do this before extension removal incase directory has a period character.
	const size_t last_slash_idx = cleanPath.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		cleanPath.erase(0, last_slash_idx + 1);
	}

	// Remove extension if present.
	/*const size_t period_idx = cleanPath.rfind('.');
	if (std::string::npos != period_idx)
	{
		cleanPath.erase(period_idx);
	}*/

	//cout<<cleanPath<<endl;
	//boost::filesystem::path p = boost::filesystem::path( cleanPath );
	//name = p.filename().string();
	name = cleanPath;
	cout<<"texture found : "<<name;
	switch( type )
	{
	case aiTextureType_DIFFUSE :
		cout<<"(diffuse)"<<endl;
		break;
	case aiTextureType_NORMALS:
		cout<<"(normale)"<<endl;
		break;
	case aiTextureType_SPECULAR:
		cout<<"(specular)"<<endl;
		break;
	}
	return true;

}

bool ModelImport::ImportMaterials( const aiScene* scene, Model* newModel )
{
	if( !scene->HasMaterials() )
	{
		cout<<"this scene as no material"<<endl;
		return false;
	}

	for(int m=0; m< scene->mNumMaterials; m++ )
	{
		//material->
		Model::Material newMaterial;

		aiMaterial* material = scene->mMaterials[m];

		aiString materialName;
		material->Get( AI_MATKEY_NAME, materialName );
		newMaterial.name = string( materialName.C_Str() );

		int textCount = material->GetTextureCount(aiTextureType_DIFFUSE);
		string basename = "";
		newMaterial.hasDiffuseTexture = getTextureName( material, aiTextureType_DIFFUSE, basename);
		newMaterial.diffuse_texture = basename;
		newMaterial.hasNormalTexture = getTextureName( material, aiTextureType_NORMALS, basename);
		newMaterial.normal_texture = basename;
		// bug ?
		if( !newMaterial.hasNormalTexture )
		{
			newMaterial.hasNormalTexture = getTextureName( material, aiTextureType_HEIGHT, basename);
			newMaterial.normal_texture = basename;
		}

		newMaterial.hasSpecularTexture = getTextureName( material, aiTextureType_SPECULAR, basename);
		newMaterial.specular_texture = basename;


		newModel->materials.push_back( newMaterial );
	}

	return true;
}

#endif
