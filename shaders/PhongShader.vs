uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform vec4 lightPosition;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;

varying vec3 lightDir, eyeVec, normal; 

void main(void)
{	
	// Transform the normal into eye space
	normal = NormalMatrix * normalOrientation;
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
  eyeVec   = -eyeCoords.xyz;
  lightDir = lightPosition.xyz;
 
  gl_Position =  ProjectionMatrix * eyeCoords;
}

