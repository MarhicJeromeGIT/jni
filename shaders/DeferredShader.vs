uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;
attribute vec2  iTexCoord;

varying vec3 normal; 
varying vec3 vertexPos;
varying vec2 texCoord;

void main(void)
{	
  texCoord = iTexCoord;
  
  // Transform the normal into eye space
  normal = NormalMatrix * normalOrientation;
  vertexPos = vec3(ModelMatrix * vec4(vertexPosition,1.0) );
  
  gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
}

