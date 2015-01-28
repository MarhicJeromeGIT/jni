#include "Tweaker.h"

#ifndef __ANDROID__

#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <boost/bind.hpp>

Tweaker* Tweaker::thisObject = NULL;

Tweaker::Tweaker()
{
	cout<<"creating tweaker"<<endl;
}

Tweaker* Tweaker::get()
{
	if( thisObject == NULL )
	{
		thisObject = new Tweaker();
	}
	return thisObject;
}

void Tweaker::addTweakable( ITweakable* tweakable )
{
	if( tweakable->type == TWEAKER_UNKNOW )
	{
		return;
	}

	tweaks.push_back( tweakable );
}

#endif
