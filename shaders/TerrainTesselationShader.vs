uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D heightmap;
uniform float intensity;

in vec3  vertexPosition;
in vec2  texCoord;

void main(void)
{	

  vec3 pos = vertexPosition;
  pos.y += texture2D( heightmap, texCoord ).r * intensity;

  gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(pos, 1.0);
} 
 
