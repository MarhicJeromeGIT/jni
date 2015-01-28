uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
        
attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

varying vec3 texCoord;

void main(void)
{	
    texCoord    = vec3( iTexCoord.x, iTexCoord.y, 0.0 );
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
}
