#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <stack>

#ifdef __ANDROID__
#else
#include <SFML/Window.hpp>
#endif

//#include "AntigravityConfig.h"

class GameStateStack;
class GameState
{
protected:
	GameStateStack* stateStack;
public:
	GameState(GameStateStack* StateStack);
	virtual ~GameState(){}

	virtual void update(float dt){ (void)dt; };
	virtual void draw(){};
	virtual void resume(){};
	virtual void start(){}; // when the state is first pushed on the stack
	virtual void onPop(){}; // when the state is removed from the stack

	virtual void onResume(){};
	virtual void onPause(){};

	virtual void MouseFunc( int button, int state, int x, int y ){ (void)button; (void)state; (void)x; (void)y; }
	virtual void PassiveMotionFunc(int x,int y){ (void)x; (void)y; }

	virtual void reloadGLData(){};

	virtual void onSurfaceChanged(float x, float y){ (void)x; (void)y; }

#ifdef __ANDROID__
	virtual void TouchScreenFunc( int pointer, bool pressed, int x, int y ){ (void)pointer; (void)pressed; (void)x; (void)y; };
#else
	virtual void KeyboardFunc( sf::Event::KeyEvent& keyEvent ){}
	virtual void KeyboardFuncReleased( sf::Event::KeyEvent& keyEvent ){}
#endif
};

class ExitGameGameState : public GameState
{

public:
	ExitGameGameState(GameStateStack* StateStack);
	void update(float dt);
	void onResume();
};

class GameStateStack
{
public:
#ifndef __ANDROID__
	sf::Window* window;
#endif
	std::stack<GameState*> states;

	// creates a state with the "exit game" game state pushed so the stack isn't empty.
	GameStateStack();

	void pushState( GameState* state );
	void popState();

	void clear();
	GameState* getCurrentState()
	{ 
		if( !states.empty() )
			return states.top(); 
		return 0;
	}
};

#endif
