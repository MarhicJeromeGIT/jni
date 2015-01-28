#include "Tweakable.h"
#include <iostream>

#ifndef __ANDROID__
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

static_assert( sizeof(SERIALIZE_TWEAKER_TYPE)/ sizeof(SERIALIZE_TWEAKER_TYPE[0]) == (TWEAKER_TYPE_MAX), "oops" );

static TWEAKER_TYPE Tweakable_StringToTweakerType( const std::string& str )
{
	TWEAKER_TYPE type = TWEAKER_UNKNOW;
	for( int i = 0; i < TWEAKER_TYPE_MAX; i++ )
	{
		if( str == SERIALIZE_TWEAKER_TYPE[i] )
		{
			type = static_cast<TWEAKER_TYPE>(i);
			break;
		}
	}
	return type;
}

string float2string(float f)
{
	cout<<"before " <<f<<endl;
  std::ostringstream os;
  os << f;

  cout<<"after "<<os.str()<<endl;
  return os.str();
}

float string2float( string s)
{
	std::stringstream ss;
	float f;
	ss.str(s);
	ss >> f;
	return f;
}

//*****************************************************
// Tweakable base-class (abstract)
//
//*****************************************************

ITweakable::ITweakable()
{
	static unsigned int counter = 0;
	TWEAK_ID = counter;
	tabType  = TAB_TYPE::SHADERS_TYPE;
	counter++;
}

void ITweakable::serialize( TiXmlElement* Tweak )
{
    // Write the Ball data :
	Tweak->SetAttribute( "type", SERIALIZE_TWEAKER_TYPE[type] );
	Tweak->SetAttribute( "id", TWEAK_ID );
	Tweak->SetAttribute( "name", name.c_str() );
	Tweak->SetAttribute( "tabType", (int)tabType );
}

ITweakable* ITweakable::deserialize( TiXmlElement* Tweak )
{
    // read the tweak data :
	const char* strType = Tweak->Attribute("type");
	assert( strType != NULL );
	TWEAKER_TYPE type = Tweakable_StringToTweakerType(strType);

	ITweakable* tweakable = NULL;
	switch( type )
	{
		case TWEAKER_FLOAT:
		{
			tweakable = new FloatTweakable(Tweak);
		}
		break;

		case TWEAKER_COLOR:
		{
			tweakable = new ColorTweakable(Tweak);
		}
		break;

		case TWEAKER_BUTTON:
		{
			tweakable = new ButtonTweakable(Tweak);
		}
		break;

		case TWEAKER_BOOL:
		{
			tweakable = new BooleanTweakable(Tweak);
		}
		break;

		case TWEAKER_ENUM:
		{
			tweakable = new EnumTweakable(Tweak);
		}
		break;

		case TWEAKER_FILECHOOSER:
		{
			tweakable = new FileChooserTweakable(Tweak);
		}
		break;

		case TWEAKER_FILESAVER:
		{
			tweakable = new FileSaverTweakable(Tweak);
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	}

	if( tweakable != NULL )
	{
		int TWEAK_ID = -1;
		assert( Tweak->Attribute( "id", &TWEAK_ID ) != NULL );
		string name = string( Tweak->Attribute( "name" ) );
		int tabType = 0;
		assert( Tweak->Attribute( "tabType", &tabType ) != NULL );

		tweakable->TWEAK_ID = TWEAK_ID;
		tweakable->tabType  = static_cast<TAB_TYPE>(tabType);
		tweakable->name     = name;
	}

	return tweakable;
}

//*****************************************************
// Boolean Tweakable
//
//*****************************************************

BooleanTweakable::BooleanTweakable( TiXmlElement* Tweak )
{
	type     = TWEAKER_BOOL;
	const char* value = Tweak->Attribute( "value" );
	bValue = value != NULL && strcmp(value,"true") == 0;
}

BooleanTweakable::BooleanTweakable( const std::string& n, callbackBoolArg cb, bool val ) : ITweakable()
{
	type     = TWEAKER_BOOL;
	name     = n;
	bValue   = val;
	callback = cb;
}

void BooleanTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetAttribute("value", bValue ? "true" : "false" );
}

void BooleanTweakable::copyValue( ITweakable* tweak )
{
	BooleanTweakable* bTweak = dynamic_cast<BooleanTweakable*>(tweak);
	assert(bTweak);
	bValue = bTweak->bValue;
}

void BooleanTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(bValue);
	}
}

//*****************************************************
// Float Tweakable
//
//*****************************************************

FloatTweakable::FloatTweakable( TiXmlElement* Tweak ) : ITweakable()
{
	type     = TWEAKER_FLOAT;
	callback = 0;
	fValue = string2float(Tweak->Attribute( "value" ));
	fMin   = string2float(Tweak->Attribute( "min" ));
	fMax   = string2float(Tweak->Attribute( "max" ));
}

FloatTweakable::FloatTweakable( const string& n, callbackFloatArg cb, float val, float min, float max ) : ITweakable()
{
	name     = n;
	type     = TWEAKER_FLOAT;
	fValue   = val;
	fMin     = min;
	fMax     = max;
	callback = cb;
}

void FloatTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetAttribute("value",float2string(fValue));
	Tweak->SetAttribute("min",  float2string(fMin));
	Tweak->SetAttribute("max",  float2string(fMax));
}

void FloatTweakable::copyValue( ITweakable* tweak )
{
	FloatTweakable* fTweak = dynamic_cast<FloatTweakable*>(tweak);
	assert(fTweak);
	fValue = fTweak->fValue;
}

void FloatTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(fValue);
	}
}

//*****************************************************
// Color Tweakable
//
//*****************************************************

ColorTweakable::ColorTweakable( TiXmlElement* Tweak )
{
	double r = 0.0f;
	Tweak->Attribute( "colorR", &r );
	double g = 0.0f;
	Tweak->Attribute( "colorR", &g );
	double b = 0.0f;
	Tweak->Attribute( "colorR", &b );
	double a = 0.0f;
	Tweak->Attribute( "colorR", &a );

	color    = glm::vec4(r,g,b,a);
	type     = TWEAKER_COLOR;
	callback = 0;
}

ColorTweakable::ColorTweakable( const string& n, callbackFloatVec4 cb, const glm::vec4& col ) : ITweakable()
{
	name     = n;
	type     = TWEAKER_COLOR;
	color    = col;
	callback = cb;
}

void ColorTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetDoubleAttribute("colorR",color.r);
	Tweak->SetDoubleAttribute("colorG",color.g);
	Tweak->SetDoubleAttribute("colorB",color.b);
	Tweak->SetDoubleAttribute("colorA",color.a);
}

void ColorTweakable::copyValue( ITweakable* tweak )
{
	ColorTweakable* cTweak = dynamic_cast<ColorTweakable*>(tweak);
	assert(cTweak);
	color = cTweak->color;
}

void ColorTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(color);
	}
}

//*****************************************************
// A Button... yep.
//
//*****************************************************

ButtonTweakable::ButtonTweakable( TiXmlElement* Tweak )
{
	type      = TWEAKER_BUTTON;
	callback  = 0;
}

ButtonTweakable::ButtonTweakable( const string& n, callbackVoid cb ) : ITweakable()
{
	name      = n;
	type      = TWEAKER_BUTTON;
	callback  = cb;
}

void ButtonTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback();
	}
}


//*****************************************************
// A choice selection tweakable
//
//*****************************************************

EnumTweakable::EnumTweakable( TiXmlElement* Tweak )
{
	type = TWEAKER_ENUM;
	callback = 0;

	int nbStrings = 0;
	Tweak->QueryIntAttribute( "enumSize", &nbStrings );

	for( int i=0; i< nbStrings; i++ )
	{
		std::ostringstream out;
		out << i;
		string key = "string_" + out.str();

		const std::string* value = Tweak->Attribute( key );
		assert( value != NULL );
		strings.push_back( value->c_str() );
	}

	Tweak->QueryIntAttribute( "currentChoice", &currentChoice );
}

EnumTweakable::EnumTweakable( const std::string& n, callbackIntArg cb, vector<std::string> strs )
{
	type          = TWEAKER_ENUM;
	name          = n;
	strings       = strs;
	currentChoice = 0;
	callback      = cb;
}

EnumTweakable::EnumTweakable( const std::string& n, callbackIntArg cb, int nbStrings, const std::string* strs )
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

void EnumTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(currentChoice);
	}
}

void EnumTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetAttribute( "currentChoice", currentChoice );
	Tweak->SetAttribute("enumSize", strings.size() );
	for( int i=0; i< strings.size(); i++ )
	{
		std::ostringstream out;
		out << i;
		string key = "string_" + out.str();

		Tweak->SetAttribute( key, strings[i] );
	}
}

void EnumTweakable::copyValue( ITweakable* tweak )
{
	EnumTweakable* eTweak = dynamic_cast<EnumTweakable*>(tweak);
	assert( eTweak != NULL );
	currentChoice = eTweak->currentChoice;
}

//*****************************************************
//  A file chooser tweakable
//
//*****************************************************

FileChooserTweakable::FileChooserTweakable( TiXmlElement* Tweak ) : ITweakable()
{
	type = TWEAKER_FILECHOOSER;
	callback = 0;

	const char* n = Tweak->Attribute( "filename" );
	filename = std::string(n);
}

FileChooserTweakable::FileChooserTweakable( const std::string& n, callbackStringArg cb, std::string file )  : ITweakable()
{
	type          = TWEAKER_FILECHOOSER;
	name          = n;
	filename      = file;
	callback      = cb;
}

void FileChooserTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(filename);
	}
}

void FileChooserTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetAttribute( "filename", filename );
}

void FileChooserTweakable::copyValue( ITweakable* tweak )
{
	FileChooserTweakable* eTweak = dynamic_cast<FileChooserTweakable*>(tweak);
	assert( eTweak != NULL );
	filename = eTweak->filename;
}


//*****************************************************
//  A file saver tweakable (marrant c'est tout pareil)
//
//*****************************************************

FileSaverTweakable::FileSaverTweakable( TiXmlElement* Tweak ) : ITweakable()
{
	type = TWEAKER_FILESAVER;
	callback = 0;

	const char* n = Tweak->Attribute( "filename" );
	filename = std::string(n);
}

FileSaverTweakable::FileSaverTweakable( const std::string& n, callbackStringArg cb, std::string file )  : ITweakable()
{
	type          = TWEAKER_FILESAVER;
	name          = n;
	filename      = file;
	callback      = cb;
}

void FileSaverTweakable::do_callback()
{
	if( callback != 0 )
	{
		callback(filename);
	}
}

void FileSaverTweakable::serialize( TiXmlElement* Tweak )
{
	ITweakable::serialize(Tweak);

	Tweak->SetAttribute( "filename", filename );
}

void FileSaverTweakable::copyValue( ITweakable* tweak )
{
	FileSaverTweakable* eTweak = dynamic_cast<FileSaverTweakable*>(tweak);
	assert( eTweak != NULL );
	filename = eTweak->filename;
}

#endif
