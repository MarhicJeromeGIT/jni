#ifndef CURSOR
#define CURSOR

#include <glm/glm.hpp>
#include <vector>
using namespace std;

class RayPicking;
class DynamicMesh;
class MaterialGUI;

enum MOUSE_BUTTON
{
	LEFT,
	MIDDLE,
	RIGHT,
};

class MouseCursor
{
public:
	MouseCursor();
	void reloadGLData();
	DynamicMesh* mesh;

	RayPicking* ray;
public:
	MaterialGUI* material;

	glm::vec2 position; // screen coord
	glm::vec2 pressedPosition;
	glm::vec2 releasedPosition;

	void Draw();

	void update(float dt);

	// (x,y) en pixels
	void getPosition( glm::vec2& pos);

	MOUSE_BUTTON mouseButton;

	// a one button mouse
	bool buttonPressed;
	bool buttonReleased;
	void setPressed(bool pressed);
	bool getPressed(){ return buttonPressed; }
	bool getReleased(){ return buttonReleased; }
	bool release(){ buttonPressed = false; buttonReleased = false; return true; }

	bool mouseMoved;

	// (x,y) entre 0 et 1
	void getNormalizedPosition( glm::vec2& pos);
	void getNormalizedPressedPosition( glm::vec2& pos);

	void setPosition( float x, float y );


	RayPicking* getRay();

	int mouseWheelOffset; // +1 up, -1 down
};



// a little like a cursor...
class TouchScreen
{
	static const int NbMultiTouch = 3;
	int currentNbTouch;
	glm::vec2 cursorPosition[NbMultiTouch];
	glm::vec2 touchPosition[NbMultiTouch];

public:
	TouchScreen();


	void setPressed(bool pressed, float x, float y);
	void setPosition( float x, float y );
};


#endif
