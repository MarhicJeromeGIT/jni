uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform float scale;

attribute vec4  vertexPosition; // position + age
attribute vec4  speed; // speed + rotation 
attribute vec2  iTexCoord;

varying vec2 texCoord;
varying float alpha;

void main()                                                
{             
	//rotated UVs :
  float sa = sin( speed.w );
  float ca = cos( speed.w );
	mat2 R = mat2(ca,sa,-sa,ca);

	float fadein = smoothstep(0.0, 1.0, 5.0*vertexPosition.w  ); // fade in in 0.2 secs
	alpha = clamp( (1.2 - vertexPosition.w) * fadein, 0.0, 1.0); // age (fade-in, fade-out)
		

  texCoord = iTexCoord;
  texCoord += vec2( -0.5,-0.5 );
	texCoord = R * texCoord;
  texCoord += vec2( 0.5, 0.5 );

	vec3 pos = vec3(ViewMatrix * ModelMatrix * vec4(vertexPosition.xyz,1.0) );
	vec3 corner_position = pos + vec3( (iTexCoord.x -0.5) * scale, (iTexCoord.y-0.5) * scale, 0 );
                   
	gl_Position = ProjectionMatrix * vec4(corner_position,1.0);  // eye space   	
}                                                            
 	

			
