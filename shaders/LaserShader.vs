uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

varying vec2 uv;

void main(void)
{	
    uv    = iTexCoord;

	float size = 2.0;

	vec3 pos = vec3(ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0) );

	vec3 corner_position = pos + vec3( 0, size * (uv.y-0.5) , 0 );
    
	gl_Position = ProjectionMatrix * vec4(corner_position,1.0);  // eye space

} 
 
