#include "ShaderParams.h"

using namespace glm;

ShaderParams* ShaderParams::thisObject = NULL;

ShaderParams::ShaderParams()
{ 
	// Lights
	for( int i=0; i<MAX_LIGHTS; i++ )
	{
		lights[i].lightPosition = vec4(0,0,0,1);
		lights[i].lightDiffuseColor = vec4(1.0,1.0,1.0,1.0);
	}
	// ambient color:
	ambient = vec4(0.4,0.4,0.4,1.0);

	win_x = 1024.0f;
	win_y = 768.0f;

	time = 0.0f;
}

ShaderParams* ShaderParams::get()
{
	if( thisObject == NULL )
	{
		thisObject = new ShaderParams();
	}
	return thisObject;
}
