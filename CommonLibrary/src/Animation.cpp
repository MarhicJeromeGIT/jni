#include "Animation.h"

#include "assert.h"
#include "Model.h"

//***************************************//
// Animation
//
// A single, simple animation
// read the model channels
//***************************************//

Animation::Animation( const Model* m, int index, bool loop /*=true*/ )
{
	model = m;
	assert( model != NULL );
	animationIndex     = index;
	nextAnimationIndex = -1;
	playing     = false;
	looped      = loop;
	speed       = 1.0;
	currentTime = 0.0f;

	if( index >= 0 )
	{
		maxTime = model->animations[animationIndex].duration;
	}
	else
	{
		maxTime = 0.0f;
	}
}

void Animation::setAnimationIndex( int index )
{
	if( index != animationIndex )
	{
		animationIndex = index;
		if( index >= 0 )
		{
			maxTime = model->animations[animationIndex].duration;
		}
		else
		{
			maxTime = 0.0f;
		}
		currentTime = 0.0f;
	}
}

void Animation::start()
{
	playing = true;
}

void Animation::pause()
{
	playing = false;
}

void Animation::reset()
{
	playing = false;
	currentTime = 0.0f;
}

void Animation::blendNextAnimation( int nextAnim, bool immediate )
{
	nextAnimationIndex = nextAnim;
}

void Animation::update(float dt)
{
	if( !playing )
		return;

	currentTime += dt * speed;
	if( currentTime >= maxTime )
	{
		if( !looped )
		{
			setAnimationIndex( nextAnimationIndex );
			looped = true;
			nextAnimationIndex = -1;
			return;
		}
		currentTime = 0.0f;
	}

}

