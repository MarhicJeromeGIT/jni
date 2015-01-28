uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 LightMat;
uniform mat3 NormalMatrix;
uniform bool bUseScreenCoord;

attribute vec3  normal;
attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

varying vec2 texCoord;
varying vec4 PosFromLightPOV;
varying vec3 interpolatedVertexEye;

uniform vec3 lightPosition;
varying float diffuseAmount;

void main(void)
{	
	// Transform the normal into eye space
	vec3 normalDir = normalize( NormalMatrix * normal );
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
  // lightPosition est passee en eyeSpace
  vec3 lightDir = normalize(lightPosition.xyz);
	diffuseAmount = max(0.0, dot(normalDir, lightDir) );

 
	PosFromLightPOV  = LightMat * ModelMatrix * vec4(vertexPosition,1.0);
    gl_Position      = ProjectionMatrix * eyeCoords;

	interpolatedVertexEye = vec3( ModelMatrix * vec4(vertexPosition,1.0) );

	if( bUseScreenCoord )
	{
		texCoord = vec2( gl_Position.x, gl_Position.y );
	}
	else
	{
		  texCoord         = iTexCoord;
	}

}

