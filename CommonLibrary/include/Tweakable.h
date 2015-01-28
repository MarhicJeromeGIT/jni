#ifndef TWEAKABLE_H
#define TWEAKABLE_H

#ifndef __ANDROID__

#include <string>
#include <iostream>
#include <vector>
#include <boost/any.hpp>
#include <thread>

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <boost/function.hpp>
#include "tinyxml.h"

enum TWEAKER_TYPE
{
	TWEAKER_UNKNOW, // unsupported type
	TWEAKER_FLOAT,
	TWEAKER_INTEGER,
	TWEAKER_COLOR,
	TWEAKER_BOOL,
	TWEAKER_BUTTON,
	TWEAKER_ENUM,
	TWEAKER_FILECHOOSER,
	TWEAKER_FILESAVER,

	TWEAKER_TYPE_MAX,
};

enum TAB_TYPE
{
	SHADERS_TYPE,
	GAMEPLAY_TYPE,
	LEVELEDITOR_TYPE,
	TAB_TYPE_MAX,
};

//*****************************************************
// Tweakable base-class (abstract)
//
//*****************************************************
typedef boost::function<void(float)>       callbackFloatArg;
typedef boost::function<void(glm::vec4)>        callbackFloatVec4;
typedef boost::function<void(void)>        callbackVoid;
typedef boost::function<void(bool)>        callbackBoolArg;
typedef boost::function<void(int)>         callbackIntArg;
typedef boost::function<void(std::string)> callbackStringArg;

class ITweakable
{

public:
	int TWEAK_ID;
	TAB_TYPE tabType;
	TWEAKER_TYPE type;
	std::string name;
public:

	// Create a tweakable from XML data
	static ITweakable* deserialize( TiXmlElement* Tweak );

public:
	ITweakable();
	virtual ~ITweakable(){}

	virtual void serialize( TiXmlElement* Tweak );
	virtual void copyValue( ITweakable* tweak ){};
	virtual void do_callback(){};

	void setTabType( TAB_TYPE t ){ tabType = t; }
};


//*****************************************************
// Boolean Tweakable
//
//*****************************************************

struct BooleanTweakable : public ITweakable
{
	bool bValue;
	callbackBoolArg callback;

	BooleanTweakable( TiXmlElement* Tweak );
	BooleanTweakable( const std::string& n, callbackBoolArg cb, bool val = false );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );
	void do_callback();
};

//*****************************************************
// Float Tweakable
//
//*****************************************************

struct FloatTweakable : public ITweakable
{
	float fValue;
	float fMin;
	float fMax;
	callbackFloatArg callback;

	FloatTweakable( TiXmlElement* Tweak );
	FloatTweakable( const std::string& n, callbackFloatArg cb, float val = 0.0, float min=0.0, float max=0.0 );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );
	void do_callback();
};

//*****************************************************
// Color Tweakable
//
//*****************************************************


struct ColorTweakable : public ITweakable
{
	glm::vec4 color;
	callbackFloatVec4 callback;

	ColorTweakable( TiXmlElement* Tweak );
	ColorTweakable( const std::string& n, callbackFloatVec4 cb, const glm::vec4& val );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );
	void do_callback();
};

//*****************************************************
// A Button... yep.
//
//*****************************************************

struct ButtonTweakable : public ITweakable
{
	callbackVoid callback;

	ButtonTweakable( TiXmlElement* Tweak );
	ButtonTweakable( const std::string& n, callbackVoid cb );

	void do_callback();
};

//*****************************************************
// A choice selection tweakable
//
//*****************************************************

struct EnumTweakable : public ITweakable
{
	callbackIntArg callback;
	std::vector<std::string> strings;
	int currentChoice;

	EnumTweakable( TiXmlElement* Tweak );
	EnumTweakable( const std::string& n, callbackIntArg cb, std::vector<std::string> strings );
	EnumTweakable( const std::string& n, callbackIntArg cb, int nbStrings, const std::string* strings );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );

	void do_callback();
};

//*****************************************************
// A file chooser (opener) tweakable
//
//*****************************************************

struct FileChooserTweakable : public ITweakable
{
	callbackStringArg callback;
	std::string filename;

	FileChooserTweakable( TiXmlElement* Tweak );
	FileChooserTweakable( const std::string& name, callbackStringArg cb, std::string filename );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );

	void do_callback();
};


//*****************************************************
// A file saver tweakable
//
//*****************************************************

struct FileSaverTweakable : public ITweakable
{
	callbackStringArg callback;
	std::string filename;

	FileSaverTweakable( TiXmlElement* Tweak );
	FileSaverTweakable( const std::string& name, callbackStringArg cb, std::string filename );

	void serialize( TiXmlElement* Tweak );
	void copyValue( ITweakable* tweak );

	void do_callback();
};


#endif

#endif
