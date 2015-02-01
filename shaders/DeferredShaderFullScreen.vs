uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

varying vec2 texCoord; 
varying vec3 texCoordScreen;

void main(void)
{	
  texCoord = iTexCoord;

  gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
  
  texCoordScreen = gl_Position.xyw;
}

