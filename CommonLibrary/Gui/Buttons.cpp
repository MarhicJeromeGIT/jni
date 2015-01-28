#include "Buttons.h"
#include "GLText.h"
#include "Tweaker.h"
#include "TextureGL.h"
#include "Cursor.h"
#include "TTFont.h"
#include "Sound.h"

#include "CommonLibrary.h"


DynamicMesh* Button::mesh = NULL;

Button::Button() : Overlay()
{
	overlayMaterial = new MaterialGUI();

	mesh        = NULL;
	tooltip     = NULL;
	buttonLabel = NULL;
	reloadGLData();

	// default position
	rotation = vec3(0,0.0f,0);
	position = vec3(0.07,0.08,0.0);
	scale = vec3(0.07,1.0,0.07);
	computeBoundingSquare();

	animTime = 0.0f;

	clic = (SoundEffect*) SoundManager::get()->getSoundEffect( "191754__fins__button-5.wav" );

	/*
#define TWEAK_VALUE
#if defined(USE_TWEAKER) && defined(TWEAK_VALUE)
	Tweaker::get()->addTweakable( new FloatTweakable( "button X",
			[this](float f){ setPosRotScale( vec3(f,position.y,position.z), rotation,scale);  }, 0.0f, -10.0f,10.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "button Y",
			[this](float f){ setPosRotScale( vec3(position.x,f,position.z), rotation,scale); }, 0.0f, -10.0f,10.0f ));
#endif
	*/

	ButtonState = button_state::normal;
	hovered  = false;
	toggled  = false;
	pressed  = false;
	released = false;
}

void Button::reloadGLData()
{
	mesh = new DynamicMesh(4,2);
	vec3 vert[4] = { vec3(-0.5,-0.5,0),vec3(0.5,-0.5,0), vec3(0.5,0.5,0), vec3(-0.5,0.5,0) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	mesh->updateGeometry( vert, uv, 4, tri, 2  );
	computeBoundingSquare();

	if( buttonLabel != NULL )
	{
		buttonLabel->reloadGLData();
	}
}

void Button::setScale(const vec3& scl)
{
	scale = scl;
	computeBoundingSquare();
}

void Button::setPosition(const vec3& pos)
{
	position = pos;
	computeBoundingSquare();
}

Button::~Button()
{
	LOGE("DELETING BUTTON");
	delete overlayMaterial;
}

void Button::setPosRotScale( const vec3& pos, const vec3& rot, const vec3& scl )
{
	position = pos;
	rotation = rot;
	scale    = scl;

	// the label scale depends on the button scale :
	if( buttonLabel != NULL )
	{
		buttonLabel->scale = vec3(scale.y * label_fontsize);
		float stringWidth = buttonLabel->sizeX * buttonLabel->scale.x; // la taille de la chaine
		buttonLabel->position.x = position.x + label_offset.x - stringWidth*0.5f;
	}

	computeBoundingSquare();
}

void Button::setLabel( const wstring& label )
{
	float fontSize = 0.4f;
	vec3 offset = vec3(0);
	setLabel(label,offset,fontSize);
}

void Button::setLabel( const wstring& label, vec3 offset, float fontSize )
{
	if( buttonLabel != NULL )
	{
		delete buttonLabel;
		buttonLabel = NULL;
	}

	label_offset = offset;
	label_fontsize = fontSize;

	FontManager::get()->getCurrentFont()->setFontColor( vec4(0,0,1,1) );
	buttonLabel = new DynamicText(label);

	// center the label on the button :
	buttonLabel->scale = vec3(scale.y * fontSize);
	float stringWidth = buttonLabel->sizeX * buttonLabel->scale.x; // la taille de la chaine
	float stringHeight = buttonLabel->sizeY * buttonLabel->scale.y;
	buttonLabel->position.x = position.x + offset.x - stringWidth*0.5f;
	buttonLabel->position.y = position.y + offset.y - stringHeight*0.5f;
	buttonLabel->position.z = position.z + 0.1f;
}

void Button::computeBoundingSquare()
{
	mat4 objectmat =   glm::translate( mat4(1.0), position ) * glm::scale( mat4(1.0), scale )  ;

	vec4 bottomLeftCorner = vec4(-0.5f,-0.5f,0,1);
	vec4 topRightCorner = vec4(0.5f,0.5f,0,1);

	bottomLeft = vec2(objectmat * bottomLeftCorner);
	topRight   = vec2(objectmat * topRightCorner);
}

void Button::update( MouseCursor* cursor, float dt )
{
	(void)dt;

	vec2 mousePosNorm;
	cursor->getNormalizedPosition( mousePosNorm );
	hovered = hoverTest(mousePosNorm);
	if( hovered )
	{
		vec2 hitPosNorm;
		cursor->getNormalizedPressedPosition( hitPosNorm );
		if( cursor->getPressed() && hoverTest(hitPosNorm) )
		{
			pressed = true;
			//LOGE("pressed");
		}
		else
		{
			if( pressed && !cursor->getPressed() )
			{
				LOGE("clicked");
				released = true;
				toggled = !toggled;
				//clic->play();

			}
			pressed = false;
		}
	}
	else
	{
		if( !cursor->getPressed() )
		{
			if( pressed )
			{
				released = true;
			}
			pressed = false;
		}
	}

	// set the button uv:
	if( toggled )
	{
		ButtonState = button_state::toggled;
	}
	else if( pressed )
	{
		ButtonState = button_state::pressed;
	}
	else if( hovered )
	{
		ButtonState = button_state::hovered;
	}
	else
	{
		ButtonState = button_state::normal;
	}
	((MaterialGUI*)overlayMaterial)->setUVScaleAndOffset( scaleAndOffset[ButtonState] );
}

void Button::Draw(MATERIAL_DRAW_PASS Pass)
{
	(void)Pass;

	mat4 objectmat =  glm::translate( mat4(1.0), position ) * glm::scale( mat4(1.0), scale );

	DrawCall drawcall;
	drawcall.Pass     = MATERIAL_DRAW_PASS::GUI;
	drawcall.modelMat = objectmat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = overlayMaterial->getShader(MATERIAL_DRAW_PASS::GUI);
	if( shader != NULL )
	{
		drawcall.material = overlayMaterial;
		drawcall.mesh     = &(mesh->mesh);
		drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency = true;
		Renderer::get()->draw( drawcall );
	}

	if( buttonLabel != NULL )
	{
		buttonLabel->Draw(GUI);
	}

//	if( hovered && tooltip != NULL )
//	{
//		tooltip->Draw(GUI);
//	}
}

bool Button::hoverTest( const vec2& normalizedMousePos )
{
	if( bottomLeft.x < normalizedMousePos.x && normalizedMousePos.x < topRight.x )
	{
		if(bottomLeft.y < normalizedMousePos.y && normalizedMousePos.y < topRight.y )
		{
			return true;
		}
	}
	return false;
}

void Button::setTooltipText( const std::wstring& text )
{
	(void)text;

	if( tooltip != NULL )
		delete tooltip;

	//GLFontManager* fonts = GLFontManager::get();
	//GLFont* arial = fonts->createFont( "arial", "/media/E/projets/build/Data/bitmap_font/Arial/", "Arial.fnt" );
	//tooltip = new DynamicText( arial );
//	tooltip->createGLString( text );
}
