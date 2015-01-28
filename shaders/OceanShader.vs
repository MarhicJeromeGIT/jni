uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform vec4 lightPosition;
uniform float lambda; // displacement

uniform sampler2D heightMapSampler;
uniform sampler2D normalMapSampler;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;
attribute vec2  iTexCoord;

varying vec3 lightDir, eyeVec, normal; 
varying vec2 texCoord;
varying vec3 pos_eye;
varying vec3 n_eye;

void main(void)
{	
	vec3 norm = normalOrientation * 0.0001 + texture2D( normalMapSampler, iTexCoord).xyz;
	vec3 displacement = texture2D( heightMapSampler, iTexCoord ).xyz;
	vec3 pos_displaced = vertexPosition.xyz + vec3( lambda, 1.0, lambda ) * displacement;
	
	pos_eye = vec3( ViewMatrix * ModelMatrix * vec4(pos_displaced,1.0) );
	n_eye = vec3( NormalMatrix * norm );

	// Transform the normal into eye space
	normal = NormalMatrix * norm;
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(pos_displaced,1.0);
  eyeVec   = -eyeCoords.xyz;
  lightDir = lightPosition.xyz;
 
	texCoord         = iTexCoord;

  gl_Position =  ProjectionMatrix * eyeCoords;
}

