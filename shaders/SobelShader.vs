
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec3 iVertex;
attribute vec2 iTexCoord;

varying vec2 vTexCoord;

void main(void)
{
	vTexCoord = iTexCoord;

  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(iVertex,1.0);
}
