uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

attribute vec3  vertexPosition;


void main(void)
{	
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);


}
