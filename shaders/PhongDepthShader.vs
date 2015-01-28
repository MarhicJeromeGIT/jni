uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform vec4 lightPosition;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;
attribute vec2  iTexCoord;

varying vec3 lightDir, eyeVec, normal; 
varying vec2 texCoord;

void main(void)
{	
	// Transform the normal into eye space
	normal = NormalMatrix * normalOrientation;
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
  eyeVec   = -eyeCoords.xyz;
  lightDir = lightPosition.xyz;
 
	
  gl_Position =  ProjectionMatrix * eyeCoords;
  
  texCoord         = iTexCoord;

}

