#version 140

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in mat4  ModelMatrix;
in vec3  vertexPosition;

void main(void)
{	
  vec3 position = vertexPosition;
  gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
} 
 
