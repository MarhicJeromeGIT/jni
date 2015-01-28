uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

attribute vec3  vertexPosition;
attribute vec3  iNormal;

varying vec3 normal;

#if defined( TEXTURE_2D )

varying vec3 position;

#else if defined( TEXTURE_CUBE )

uniform vec3 WorldSpaceCameraPos;
varying vec3 viewDirection;

#endif

void main(void)
{	
  normal = NormalMatrix * iNormal;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);

#if defined( TEXTURE_2D )

position = gl_Position.xyz;

#else if defined( TEXTURE_CUBE )
  normal = normal*0.00001 +  vec3( ModelMatrix * vec4(iNormal,0.0) );

  viewDirection = vec3(ModelMatrix * vec4(vertexPosition,1.0) - vec4(WorldSpaceCameraPos, 1.0));
//	viewDirection = - (ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0)).xyz;

#endif
}
