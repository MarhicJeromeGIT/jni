#include "MeshAnimation.h"
#include "OpenGLModel.h"

//*********************************
// Node based Animation
// it can be a bone or not
//
//*********************************

using namespace glm;

MeshAnimation::MeshAnimation( const Model* m ) : model(m)
{
	assert( model != NULL );

	skeletonLocal  = new mat4[ model->skeleton.size() ];
	skeletonGlobal = new mat4[ model->skeleton.size() ];
	skeletonLocalWithoutScale  = new mat4[ model->skeleton.size() ];
	skeletonGlobalWithoutScale = new mat4[ model->skeleton.size() ];

	for( unsigned int i=0; i< model->skeleton.size(); i++ )
	{
		skeletonLocal[i]  = model->skeleton[i].localTransform;
		skeletonGlobal[i] = model->skeleton[i].worldTransform;

		skeletonLocalWithoutScale[i]  = model->skeleton[i].localTransform;
		skeletonGlobalWithoutScale[i] = model->skeleton[i].worldTransform;
	}
}

MeshAnimation::~MeshAnimation()
{
	delete[] skeletonLocal;
	delete[] skeletonGlobal;
	delete[] skeletonLocalWithoutScale;
	delete[] skeletonGlobalWithoutScale;
}

void MeshAnimation::ComputeAnimation( int animationIndex, float time )
{
	if( animationIndex == -1 )
	{
		return;
	}

	// TODO : I can optimize this
	for( unsigned int i=0; i< model->animations[animationIndex].channels.size() ; i++ )
	{
		BoneAnimationChannel channel = model->animations[animationIndex].channels[i];

		// find the frame corresponding to the time:
		int frameA = 0;
		int frameB = 0;
		float t = 1.0; // range [0;1], time = frameA * t + (1-t)*frameB;

		if( channel.locrotscale.size() > 1 )
		{
			if( time >= channel.locrotscale[channel.locrotscale.size()-1].time )
			{
				frameA = channel.locrotscale.size()-1;
				frameB = frameA;
			}
			for( unsigned int numFrame=0; numFrame< channel.locrotscale.size()-1; numFrame++ )
			{
				if( channel.locrotscale[numFrame].time <= time && channel.locrotscale[numFrame+1].time >= time )
				{
					frameA = numFrame;
					frameB = numFrame+1;
					t = (time - channel.locrotscale[frameB].time ) / (channel.locrotscale[frameA].time - channel.locrotscale[frameB].time );
					assert( t>= 0.0f && t<= 1.0f );
					break;
				}
			}
		}

		mat4 localTransformMat;
		getLocalTransform( channel, frameA, frameB, t, localTransformMat);
		skeletonLocal[ channel.boneIndex ] =  localTransformMat;

		//getLocalTransform( channel, frameA, frameB, t, localTransformMat, false );
		//skeletonLocalWithoutScale[ channel.boneIndex ] =  localTransformMat;
	}
}

void MeshAnimation::ComputeGlobalSkeleton()
{
	// recompute the global transform of each bone (I know that the index of the parent is always smaller than the index of the child)
	// no need to recompute the root
	skeletonGlobal[0] = skeletonLocal[0];
	skeletonGlobalWithoutScale[0] = skeletonLocal[0];

	for( unsigned int i=1; i < model->skeleton.size(); i++ )
	{
		skeletonGlobal[i] = skeletonGlobal[ model->skeleton[i].parentIndex ] * skeletonLocal[i];

		//skeletonGlobalWithoutScale[i] = skeletonGlobalWithoutScale[ model->skeleton[i].parentIndex ] * skeletonLocalWithoutScale[i];
		//skeletonGlobal[i]             = skeletonGlobalWithoutScale[ model->skeleton[i].parentIndex ] * skeletonLocal[i];
	}
}

void MeshAnimation::getLocalTransform( const BoneAnimationChannel& channel, int frameA, int frameB, float t, mat4& localTransform, bool applyScale /*=false*/ )
{
	vec3 posA  = channel.locrotscale[frameA].position;
	vec3 posB = channel.locrotscale[frameB].position;

	vec3 sclA = channel.locrotscale[frameA].scale;
	vec3 sclB = channel.locrotscale[frameB].scale;
	vec3 scl = sclA * t + sclB * (1.0f-t);

	Quaternion rotA = channel.locrotscale[frameA].rotation;
	Quaternion rotB = channel.locrotscale[frameB].rotation;

	Quaternion qA = rotA.normalize();
	Quaternion qB = rotB.normalize();
	Quaternion q = slerp( t, qA, qB ).normalize();

	mat4 rotation = q.RotationMatrix();

	vec3 pos = vec3(posA.x, posA.y, posA.z ) * t + vec3(posB.x, posB.y, posB.z ) * (1.0f-t);
	mat4 translation = glm::translate( mat4(1.0),pos );
	mat4 scale = glm::scale( mat4(1.0), scl );
	localTransform = applyScale ? translation * rotation * scale : translation * rotation;
}



//*******************************
// Shapekey animation
//
//*******************************

MorphKeyAnimation::MorphKeyAnimation()
{
	initialized = false;
}

void MorphKeyAnimation::init( const Model::Mesh* m, InternalMesh* glmesh )
{
	initialized = true;
	mesh = m;
	glMesh = glmesh;
	positionBuffer = new vec3[ mesh->nbVertices ];
}

void MorphKeyAnimation::update( float animationTime )
{
	if( !initialized )
	{
		return;
	}

	vector<float> shapeBlendValue = vector<float>( mesh->numMeshAnim );

	for( int shape = 0; shape < mesh->numMeshAnim; shape++ )
	{
		const Model::ShapeKey& shapekey = mesh->shapekeys[shape];
		const Model::MorphAnimation& morphAnim = shapekey.animation;
		if( morphAnim.morphTimeline.size() == 0 )
		{
			shapeBlendValue[shape] = 0.0f;
			continue; // no animation for that blendshape
		}
		else if( morphAnim.morphTimeline.size() == 1 )
		{
			shapeBlendValue[shape] = morphAnim.morphTimeline[0].second;
			continue; // no animation for that blendshape, just a default value
		}

		// compute the current blend value ([0.0;1.0])
		shapeBlendValue[shape] = morphAnim.morphTimeline[0].second;

		for( unsigned int i= 0; i< morphAnim.morphTimeline.size()-1; i++ )
		{
			if( morphAnim.morphTimeline[i].first <= animationTime && animationTime < morphAnim.morphTimeline[i+1].first )
			{
				pair<float,float> frameA = morphAnim.morphTimeline[i];
				pair<float,float> frameB = morphAnim.morphTimeline[i+1];
				float t = std::max(0.0f, std::min(1.0f, (animationTime-frameA.first)/(frameB.first-frameA.first) ));
				shapeBlendValue[shape] = frameA.second * (1.0 - t ) + frameB.second * t;
				break;
			}
		}
	}

	for( int i=0; i< mesh->nbVertices; i++ )
	{
		vec3 position = mesh->vertex_positions[i];
		for( int shape = 0; shape< mesh->numMeshAnim; shape++ )
		{
			position += mesh->shapekeys[shape].vertex_meshanim_positions[i] * shapeBlendValue[shape];
		}
		positionBuffer[i] = position;
	}
	DynamicMesh::updateBuffer( positionBuffer, mesh->nbVertices, glMesh->vertexBuffer );
}

