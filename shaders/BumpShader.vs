				 
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform vec3 lightPos;
uniform mat4 LightMat;

attribute vec3 vertexPosition;
attribute vec2 iTexcoord;
attribute vec3 normale,tangente;

varying vec3 lightVec; 
varying vec3 eyeVec;
varying vec2 texCoord;

varying vec3 n;
varying vec4 eyepos;
varying vec4 PosFromLightPOV;

void main(void)
{
	eyepos = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);
	gl_Position = ProjectionMatrix * eyepos; 
	texCoord = iTexcoord;
	
	PosFromLightPOV  = LightMat * ModelMatrix * vec4(vertexPosition,1.0);

	n = normalize(NormalMatrix * normale);
	vec3 t = normalize(NormalMatrix * tangente);
	vec3 b = cross(n, t);
	
	vec3 vVertex = eyepos.xyz;
	
    vec3 lightDir  = normalize( lightPos );
	lightVec.x = dot(lightDir, t);
	lightVec.y = dot(lightDir, b);
	lightVec.z = dot(lightDir, n);

	eyeVec.x = dot(-vVertex, t);
	eyeVec.y = dot(-vVertex, b);
	eyeVec.z = dot(-vVertex, n);
}
