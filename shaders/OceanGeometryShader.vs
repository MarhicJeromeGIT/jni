
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

varying vec2 texCoord;

void main()
{
	texCoord = iTexCoord;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);

}