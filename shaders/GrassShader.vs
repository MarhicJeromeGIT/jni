uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 lightPosition;
  
uniform vec2 windDirection;
uniform float time;

uniform sampler2D grassHeightSampler;

attribute vec3  vertexPosition;
attribute vec2  iTexCoord;
attribute vec3  normale;
attribute vec2  grassCoord;

varying vec2 texCoord;
varying float diffuse;
varying vec2 vGrassCoord;

void main(void)
{	
  texCoord = iTexCoord;
  vGrassCoord = grassCoord;
  
  // grass animation
  vec3 vertexPos = vertexPosition;
  vec2 offset = vec2(0.0);
  if( iTexCoord.y > 0.05 ) // top part of the grass
  {
	  float h = texture2D( grassHeightSampler, grassCoord ).r;
	  
	  const float M_PI_2 = 6.2831;
	  offset = windDirection * sin( time * M_PI_2 / 6.0 ) * h * 0.2;
	  
	  vertexPos += vec3( offset.x, h * 2.0, offset.y );
	  texCoord.y = h * 0.95;
  }
  
  gl_Position      = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPos,1.0);
  
  // lighting :
  vec3 L = normalize( lightPosition - vertexPos);
  vec3 N = normalize( normale + vec3( offset.x, 0.0, offset.y ) );
  diffuse = max( 0.0, dot( L, N ) );
  
}
