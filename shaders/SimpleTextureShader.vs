uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec4 UVScaleAndOffset;
        
attribute vec3  vertexPosition;
attribute vec2  iTexCoord;

#ifdef VERTEX_TRANSPARENCY
attribute float vertexTransparency;
 
varying float alpha;
#endif

varying vec2 texCoord;

void main(void)
{	
	#ifdef VERTEX_TRANSPARENCY
	alpha = vertexTransparency;
 	#endif

  texCoord         = iTexCoord * UVScaleAndOffset.xy + UVScaleAndOffset.zw;
	gl_Position      = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
}
