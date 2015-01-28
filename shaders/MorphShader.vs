uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 lightPosition;
uniform mat3 NormalMatrix;
uniform float morphCoeff;

attribute vec3  vertexPosition;
attribute vec3  vertexMorphPosition;
attribute vec3  normalOrientation;
attribute vec3  normalMorphOrientation;
attribute vec2  iTexCoord;

varying vec3 lightDir, eyeVec, normal; 
varying vec2 texCoord;

void main(void)
{	
	float morphCoeffClamped = clamp(morphCoeff,0.0,1.0);
	vec3 vertex = mix( vertexPosition, vertexMorphPosition, morphCoeffClamped );
	normal = NormalMatrix * mix( normalOrientation, normalMorphOrientation, morphCoeffClamped );

  texCoord         = iTexCoord;

	// Transform the normal into eye space
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(vertex,1.0);
  eyeVec   = -eyeCoords.xyz;
  lightDir = lightPosition.xyz;
 
  gl_Position =  ProjectionMatrix * eyeCoords;
} 
 
