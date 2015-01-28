#define MAX_MESH_BONES 30

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 uBonesMatrix[MAX_MESH_BONES];

attribute vec3  vertexPosition;
attribute vec4  bonesIndex;
attribute vec4  bonesWeight;

void main(void)
{	
	mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
	int x = int(bonesIndex.x);
	int y = int(bonesIndex.y);
	int z = int(bonesIndex.z);
	int w = int(bonesIndex.w);
	mat4 BoneTransform = uBonesMatrix[ x ] * bonesWeight.x;
	BoneTransform += uBonesMatrix[ y ] * bonesWeight.y;
	BoneTransform += uBonesMatrix[ z ] * bonesWeight.z;
	BoneTransform += uBonesMatrix[ w ] * bonesWeight.w;

	vec4 vertex_transform = BoneTransform *  vec4(vertexPosition,1.0 );
	vec4 vertex_pos = ModelViewMatrix * vertex_transform;
	
  gl_Position =  ProjectionMatrix * vertex_pos;
}

