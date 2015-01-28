uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 LightMat;

attribute vec3  vertexPosition;

varying vec4 PosFromLightPOV;

void main(void)
{	
	PosFromLightPOV = LightMat * vec4(vertexPosition,1.0);
	
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
}
