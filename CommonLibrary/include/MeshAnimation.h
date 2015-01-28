#ifndef MESH_ANIMATION_H
#define MESH_ANIMATION_H

#include "glm/glm.hpp"

#include "Model.h"

class InternalMesh;

// it can be a bone or not
// Channel animation (hierarchical)
class MeshAnimation
{
public:
	// This class computes the global model matrices for a given animation time
	const Model* model;

	glm::mat4* skeletonLocal;
	glm::mat4* skeletonLocalWithoutScale;
	glm::mat4* skeletonGlobal;
	glm::mat4* skeletonGlobalWithoutScale;

	MeshAnimation( const Model* model );
	~MeshAnimation();

	void ComputeAnimation( int animationIndex, float time );
	void ComputeGlobalSkeleton();

private:
	void getLocalTransform( const BoneAnimationChannel& channel, int frameA, int frameB, float t, glm::mat4& localTransform, bool applyScale = true );
};


class MorphKeyAnimation
{
public:
	MorphKeyAnimation();
	void init(const Model::Mesh* m, InternalMesh* glmesh);

	bool initialized;
	const Model::Mesh* mesh;
	InternalMesh* glMesh; // the buffers we are modifying

	glm::vec3* positionBuffer;

	void update( float animationTime );
};

#endif
