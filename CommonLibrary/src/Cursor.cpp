#include "Cursor.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Material.h"
#include "RayPicking.h"
#include "ShaderParams.h"
#include "Model.h"
#include "Renderer.h"
#include "TextureGL.h"
#include "OpenGLModel.h"

#include <iostream>
using namespace std;
#include "Camera.h"
#include "TextureGL.h"

using namespace glm;

MouseCursor::MouseCursor()
{
/*	position = vec2(-1,-1);
	pressedPosition = position;

	ray = new RayPicking();
	buttonPressed = false;

	mesh = NULL;
	reloadGLData();

	material = new MaterialGUI();
	TextureGL* tex = TextureManager::get()->loadTexture( DATA_PATH "slick_arrow-arrow.png", DATA_PATH "slick_arrow-arrow.png" );
	material->setTexture( tex );

	mouseMoved = false;
	mouseWheelOffset = 0;*/
}

void MouseCursor::reloadGLData()
{
	if( mesh != NULL )
		delete mesh;

	mesh = new DynamicMesh(4,2);
	vec3 vert[4] = { vec3(0,-1,0),vec3(1,-1,0), vec3(1,0,0), vec3(0,0,0) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	mesh->updateGeometry( vert, uv, 4, tri, 2  );
}

void MouseCursor::Draw()
{
	vec3 scaleVec = vec3(0.02, 0.02, 1.0);
	vec2 posnorm;
	getNormalizedPosition(posnorm);
	vec3 pos = vec3(posnorm.x, posnorm.y, 0.0f );
	mat4 objectmat =  glm::translate( mat4(1.0), pos ) *  glm::scale( mat4(1.0), scaleVec );

	DrawCall drawcall;
	drawcall.Pass     = MATERIAL_DRAW_PASS::GUI;
	drawcall.modelMat = objectmat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = material->getShader(MATERIAL_DRAW_PASS::GUI);
	if( shader != NULL )
	{
		drawcall.material = material;
		drawcall.mesh     = &(mesh->mesh);
		drawcall.hasTransparency = true;
		Renderer::get()->draw( drawcall );
	}
}

void MouseCursor::update(float dt)
{
	(void)dt;

	static vec2 previousPosition = vec2(0.0f,0.0f);

	if( previousPosition != position )
	{
		previousPosition = position;
		mouseMoved = true;
	}
	else
	{
		mouseMoved = false;
	}

	// release just last one frame :
	static bool wasReleased = false;
	if( wasReleased )
	{
		buttonReleased = false;
		wasReleased    = false;
	}
	if( buttonReleased )
	{
		wasReleased = true;
	}

	static int mouseWheel = 0;
	if( mouseWheel != mouseWheelOffset )
	{
		mouseWheel = mouseWheelOffset;
	}
	else
	{
		mouseWheelOffset = 0;
		mouseWheel = 0;
	}

}

void MouseCursor::getPosition( vec2& pos)
{
	pos = vec2(position.x, position.y);
	return;
}

void MouseCursor::getNormalizedPosition( vec2& pos)
{
	const float rapport = ShaderParams::get()->win_x/ShaderParams::get()->win_y;
	pos = vec2(rapport*position.x/ShaderParams::get()->win_x, position.y/ShaderParams::get()->win_y);
	return;
}

void MouseCursor::getNormalizedPressedPosition( vec2& pos)
{
	const float rapport = ShaderParams::get()->win_x/ShaderParams::get()->win_y;
	pos = vec2(rapport*pressedPosition.x/ShaderParams::get()->win_x, pressedPosition.y/ShaderParams::get()->win_y);
	return;
}

void MouseCursor::setPressed(bool pressed)
{
	if( pressed )
	{
		pressedPosition = position;
		buttonReleased  = false;
	//	cout<<"pressed"<<endl;
	}
	else if( buttonPressed == true && pressed == false)
	{
		buttonReleased = true;
		releasedPosition = position;
	//	cout<<"released"<<endl;
	}
	buttonPressed = pressed;
}

void MouseCursor::setPosition( float x, float y )
{
	position = vec2(x,ShaderParams::get()->win_y - y);
}

vec3 GetOGLPos(float x, float y)
{
	//Camera* camera = getCamera();
    vec4 viewport;
    mat4 modelview;
    mat4 projection;
    GLfloat winX, winY;
    vec3 pos;


    modelview   = ShaderParams::get()->viewMatrix; //glm::lookAt( camera->Position, camera->Direction + camera->Position, camera->Up );
    float rapport = ShaderParams::get()->win_x/ShaderParams::get()->win_y;
    projection  =  ShaderParams::get()->projectionMatrix;
	viewport = vec4(0,0,ShaderParams::get()->win_x,ShaderParams::get()->win_y);

    winX = x;
    winY = ShaderParams::get()->win_y-y;

    pos = unProject( vec3(winX, winY, 0), modelview, projection, viewport );

    return pos;
}

RayPicking* MouseCursor::getRay()
{
	vec2 pos;
	getPosition(pos);
	vec3 result = GetOGLPos( pos.x, pos.y );

	ray->rayStart = vec3(0); // aaaaah ! getCamera()->Position;
	ray->rayEnd   = result;

	return ray;
}

//****************************************************************//
// TouchScreen interface
//
//****************************************************************//

TouchScreen::TouchScreen()
{
	currentNbTouch = -1;
}

void TouchScreen::setPressed(bool pressed, float x, float y)
{
	if( currentNbTouch >= NbMultiTouch-1 )
		return;

	if( pressed )
	{
		currentNbTouch++;
		cout<<"touch "<<currentNbTouch<<endl;
		touchPosition[currentNbTouch] = vec2(x,y);
		cursorPosition[currentNbTouch] = vec2(x,y);
	}
	else
	{

	}
}

void TouchScreen::setPosition( float x, float y )
{

}

