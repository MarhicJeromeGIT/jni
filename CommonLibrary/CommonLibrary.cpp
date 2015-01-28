#include "CommonLibrary.h"

#define _STLP_HAS_INCLUDE_NEXT  1
#define _STLP_USE_MALLOC   1
#define _STLP_USE_NO_IOSTREAMS  1
/*
#include <stl/config/_android.h>
#include <string>
*/
#include <map>

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#include <glm/glm.hpp>
using namespace glm;

int test()
{
/*	// do something...

	std::map<std::string,int> mymap;
	mymap.insert( std::pair<string,int>("test", 25) );

	std::map<std::string,int>::iterator it = mymap.find( "test" );
	int a = -1;
	if( it != mymap.end() )
	{
		a = it->second;
	}
	return a;*/

	return 0;
}
