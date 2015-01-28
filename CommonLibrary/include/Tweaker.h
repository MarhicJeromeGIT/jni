#ifndef TWEAKER_H
#define TWEAKER_H

#ifndef __ANDROID__

#ifndef NDEBUG
#define USE_TWEAKER
#endif

#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include "Tweakable.h"
#include "TCPIP.h"

class Tweaker
{
public:
	static Tweaker* thisObject;
	Tweaker();

public:
	std::vector<ITweakable*> tweaks;
	static Tweaker* get();
	void addTweakable( ITweakable* tweakable);
};
 
#endif

#endif
