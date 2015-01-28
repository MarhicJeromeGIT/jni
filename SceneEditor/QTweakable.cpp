#include "QTweakable.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using std::vector;

//*************
// Helper
//*************

static string SERIALIZE_TWEAKER_TYPE[] =
{
	"unknown",        //TWEAKER_UNKNOW, // unsupported type
	"float",          //TWEAKER_FLOAT,
	"int",            //TWEAKER_INTEGER,
	"color",          //TWEAKER_COLOR,
	"bool",           //TWEAKER_BOOL,
	"button",         //TWEAKER_BUTTON,
	"enum",           //TWEAKER_ENUM
	"file_chooser",   //TWEAKER_FILECHOOSER
	"file_saver",     // TWEAKER_FILESAVER
};


//*****************************************************
// Tweakable base-class (abstract)
//
//*****************************************************

QTweakable::QTweakable() : QObject(NULL)
{
	static unsigned int counter = 0;
	TWEAK_ID = counter;
	counter++;
}

//*****************************************************
// Boolean Tweakable
//
//*****************************************************


QBooleanTweakable::QBooleanTweakable( const std::string& n, callbackBoolArg cb, bool val ) : QTweakable()
{
	type     = TWEAKER_BOOL;
	name     = n;
	bValue   = val;
	callback = cb;
}

void QBooleanTweakable::doCallback( int b )
{
	if( callback != NULL )
	{
		callback( (bool)b );
	}
}
//*****************************************************
// Float Tweakable
//
//*****************************************************

QFloatTweakable::QFloatTweakable( const string& n, callbackFloatArg cb, float val, float min, float max ) : QTweakable()
{
	name     = n;
	type     = TWEAKER_FLOAT;
	fValue   = val;
	fMin     = min;
	fMax     = max;
	callback = cb;
}

void QFloatTweakable::doCallback( double f )
{
	if( callback != NULL )
	{
		callback( (float)f );
	}
}

//*****************************************************
// Int Tweakable
//
//*****************************************************

QIntTweakable::QIntTweakable( const string& n, callbackFloatArg cb, int val, int min, int max ) : QTweakable()
{
	name     = n;
	type     = TWEAKER_INTEGER;
	iValue   = val;
	iMin     = min;
	iMax     = max;
	callback = cb;
}

void QIntTweakable::doCallback(int value)
{
	callback(value);
}

//*****************************************************
// Color Tweakable
//
//*****************************************************

QColorTweakable::QColorTweakable( const string& n, callbackFloatVec4 cb, const glm::vec4& col ) : QTweakable()
{
	name     = n;
	type     = TWEAKER_COLOR;
	color    = col;
	callback = cb;
}

void QColorTweakable::doCallback( const QColor & color)
{
	callback( glm::vec4( color.redF(), color.greenF(), color.blueF(), color.alphaF() ) );
}

//*****************************************************
// A Button... yep.
//
//*****************************************************

QButtonTweakable::QButtonTweakable( const string& n, callbackVoid cb ) : QTweakable()
{
	name      = n;
	type      = TWEAKER_BUTTON;
	callback  = cb;
}


//*****************************************************
// A choice selection tweakable
//
//*****************************************************

QEnumTweakable::QEnumTweakable( const std::string& n, callbackIntArg cb, vector<std::string> strs ) : QTweakable()
{
	type          = TWEAKER_ENUM;
	name          = n;
	strings       = strs;
	currentChoice = 0;
	callback      = cb;
}

QEnumTweakable::QEnumTweakable( const std::string& n, callbackIntArg cb, int nbStrings, const std::string* strs ) : QTweakable()
{
	type          = TWEAKER_ENUM;
	name          = n;
	currentChoice = 0;
	callback      = cb;

	for( int i=0; i< nbStrings; i++ )
	{
		strings.push_back( strs[i] );
	}
}
