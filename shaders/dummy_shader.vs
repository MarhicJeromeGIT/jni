uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec3  vertexPosition;
in vec2  iTexCoord;

varying vec2 uv;

void main()
{
	uv = iTexCoord;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4( vertexPosition, 1.0 );
}
