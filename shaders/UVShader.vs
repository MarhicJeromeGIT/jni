uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec3  vertexPosition;
in vec3  texCoord;

varying vec3 vTexCoord;

void main(void)
{	
	vTexCoord = texCoord;
	gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition, 1.0);
} 
 
