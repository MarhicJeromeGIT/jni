#ifndef GAME_BUTTONS_H
#define GAME_BUTTONS_H

#include "Overlay.h"
#include <functional>

enum button_state
{
	normal,
	toggled,
	pressed,
	hovered,
	num_states
};

//******************************************************
// Generic Button class
//
//******************************************************
class DynamicText;
class MouseCursor;
class DynamicMesh;
class SoundEffect;

class Button : public Overlay
{
public:
	vec4 scaleAndOffset[button_state::num_states];

	static DynamicMesh* mesh;

	Material* overlayMaterial;

	// bounding Square
	vec2 bottomLeft;
	vec2 topRight;

	TextureGL* towerLogoTex;

	float animTime;
	int currentSprite;

	DynamicText* buttonLabel;
	vec3  label_offset;
	float label_fontsize;

	void setLabel( const wstring& label, vec3 offset, float fontSize );
	void setLabel( const wstring& label );

	button_state ButtonState;

	bool released;
	bool hovered;
	bool pressed;
	bool toggled;

	SoundEffect* clic;

	void setPosRotScale( const vec3& position, const vec3& rotation, const vec3& scale );

	bool hoverTest( const vec2& normalizedMousePos );

	virtual void update(MouseCursor* cursor, float dt);
	virtual void Draw(MATERIAL_DRAW_PASS Pass);

	virtual void reloadGLData();
public:
	Button();
	virtual ~Button();

	virtual void setScale(const vec3& scl);
	void setPosition(const vec3& pos);
protected:
	vec3 rotation;
	vec3 position;
	vec3 scale;
	void computeBoundingSquare();

	DynamicText* tooltip;
	void setTooltipText( const std::wstring& text );
};

#endif
