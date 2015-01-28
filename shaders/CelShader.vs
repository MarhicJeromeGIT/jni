uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform bool bUseScreenCoord;
uniform vec4 lightPosition;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;
attribute vec2  iTexCoord;

varying vec3 lightDir, eyeVec, normal; 
varying vec2 texCoord;
varying vec4 eyepos;

void main(void)
{	

	// Transform the normal into eye space
	normal = NormalMatrix * normalOrientation;
  eyepos = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
  eyeVec   = -eyepos.xyz;
  lightDir = lightPosition.xyz;
 	gl_Position =  ProjectionMatrix * eyepos;

	if( bUseScreenCoord )
	{
		texCoord = vec2( gl_Position.x, gl_Position.y );
	}
	else
	{
		texCoord         = iTexCoord;
	}
  
}

