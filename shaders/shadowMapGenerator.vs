uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec3  vertexPosition;

varying vec4 position;

void main(void)
{
	 position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
   gl_Position =  position;
}
