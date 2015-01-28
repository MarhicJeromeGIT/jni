#version 150

uniform sampler2D waterTex;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform vec3 LightPos;
uniform float time;

uniform mat4 LightMat;
varying vec4 PosFromLightPOV;

in vec3  iNormale,iTangente;
in vec3  vertexPosition;
in vec2  iTexCoord;

varying vec2 oTexCoord;
varying vec4 interpolatedVertexObject;

out vec3 lightDir, eyeVec;

uniform sampler2D noiseMap;

void main(void)
{
  const float speed = 0.10f;
	oTexCoord = iTexCoord + vec2(time*speed,time*speed*speed);
 
	vec3 vertexModified = vertexPosition;

  //float d = texture2D( waterTex, oTexCoord ).r;
  vertexModified.x += cos( oTexCoord.x ) * 20.0;
vertexModified.x += sin( oTexCoord.y ) * 20.0;
 	//vertexModified.z += cos( oTexCoord.y ) * 20.0;
	

  // eyepos
	vec3 vVertex = vec3(ViewMatrix * ModelMatrix * vec4(vertexModified,1.0));
	
	vec3 t = normalize(NormalMatrix * iTangente );
	vec3 n = normalize(NormalMatrix * iNormale );
	vec3 b = cross(n,t);

	vec3 lightVec = normalize(LightPos);
	lightDir.x = dot(lightVec, t);
	lightDir.y = dot(lightVec, b);
	lightDir.z = dot(lightVec, n);

	eyeVec.x = dot(-vVertex, t);
	eyeVec.y = dot(-vVertex, b);
	eyeVec.z = dot(-vVertex, n);



	interpolatedVertexObject = vec4( vertexModified, 1.0 );
  gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexModified,1.0);

	PosFromLightPOV  = LightMat * ModelMatrix * vec4(vertexModified,1.0);
}
