
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform vec4 lightPosition;

attribute vec3  vertexPosition;
attribute vec3  normalOrientation;

#ifdef USE_TEXTURE
	attribute vec2 texCoord;
	varying vec2 uv;
#endif

#ifdef USE_NORMALMAP
attribute vec3 tangente;
varying mat3 invNormalMat;
#endif

varying vec3 pos_eye;
varying vec3 n_eye;
varying vec3 lightDir, eyeVec, normal; 

void main(void)
{
#ifdef USE_TEXTURE
	uv = texCoord;
#endif

	normal = NormalMatrix * normalOrientation;
  vec4 eyeCoords = ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);

	pos_eye = vec3( ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0) );
	n_eye = vec3( NormalMatrix * normalOrientation );
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0);

#ifdef USE_NORMALMAP
	vec3 t = normalize(NormalMatrix * tangente);
	vec3 b = cross(normal, t);
	

	mat3 RM = mat3( ModelMatrix[0].xyz,
									ModelMatrix[1].xyz,
									ModelMatrix[2].xyz );

	invNormalMat = RM * ( mat3(t,b,normal) );

  vec3 lightVec = normalize( lightPosition.xyz );
	lightDir.x = dot(lightVec, t);
	lightDir.y = dot(lightVec, b);
	lightDir.z = dot(lightVec, normal);
	
	vec3 vVertex = eyeCoords.xyz;
	eyeVec.x = dot(-vVertex, t);
	eyeVec.y = dot(-vVertex, b);
	eyeVec.z = dot(-vVertex, normal);
#else
  eyeVec   = -eyeCoords.xyz;
  lightDir = lightPosition.xyz;
#endif
}
