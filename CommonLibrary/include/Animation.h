#ifndef ANIMATION_H
#define ANIMATION_H

#include <stack>
class Model;

//***************************************//
// Animation
//
// A single, simple animation
// read the model channels
//***************************************//

class Animation
{
public:
	Animation( const Model* model, int animationIndex, bool looped = true );
	void start();
	void pause();
	void reset();
	void update(float dt);

	int getAnimationIndex(){ return animationIndex; }
	void setAnimationIndex( int animationIndex );

	float getCurrentTime(){ return currentTime; }

	void setLooped( bool loop ){ looped = loop; }
	void setSpeed( float spd ){ speed = spd; }

	int nextAnimationIndex;
	void blendNextAnimation( int animationIndex, bool immediate );

private:
	const Model* model;
	int   animationIndex;
	float currentTime;
	float maxTime;
	bool  playing;
	bool  looped;
	float speed;

};


#endif
