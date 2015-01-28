
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;

attribute vec3 vertexPosition;

varying vec3 oTexCoord;

void main(void)
{	
	oTexCoord = vertexPosition;

  gl_Position =  (ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0) ).xyww;

}
