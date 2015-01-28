#include "GameState.h"
#include  <iostream>
#include "assert.h"

using std::cout;
using std::endl;

GameState::GameState(GameStateStack* StateStack) : stateStack(StateStack)
{
}

ExitGameGameState::ExitGameGameState(GameStateStack* StateStack) : GameState(StateStack)
{

}

void ExitGameGameState::update(float dt)
{
	cout<<"ExitGameGameState exiting game"<<endl;
	//exit(0);
}

void ExitGameGameState::onResume()
{
}

GameStateStack::GameStateStack()
{
#ifdef __ANDROID__

#else
	states.push( new ExitGameGameState(this) );
#endif
}

void GameStateStack::pushState( GameState* state )
{
	state->start();
	states.push( state );
}

void GameStateStack::popState()
{
	GameState* top = states.top();
	if( top != NULL )
	{
		top->onPop();
	}

	states.pop();

	if( !states.empty() )
		states.top()->resume();
}

void GameStateStack::clear()
{
	while( !states.empty() )
	{
		states.pop();
	}
}

