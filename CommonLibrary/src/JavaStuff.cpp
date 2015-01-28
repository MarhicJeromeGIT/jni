#include "CommonLibrary.h"
#include "JavaStuff.h"
#include "Sound.h"
#include "assert.h"
#include <iostream>
using namespace std;
#include "fichiers.h"
#include "math.h"
//#include "AntigravityConfig.h"
#include "ShaderParams.h"

//*************************************
// Java Stuff
//
//*************************************

JavaStuff* JavaStuff::thisObject = NULL;

JavaStuff* JavaStuff::get()
{
	if( thisObject == NULL )
	{
		thisObject = new JavaStuff();
	}
	return thisObject;
}

#ifdef __ANDROID__

JavaStuff::JavaStuff()
{
	JavaEnvironment = NULL;
	JavaPreferencesManager = NULL;
	JavaSoundManager = NULL;
}

float JavaStuff::getFloatPreference( const std::string& name )
{
	if( JavaEnvironment == NULL ) return 0.0f;

	//LOGE("JavaStuff::getFloatPreference");

	jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/PreferencesManager");
	if( clazz == NULL )
	{
		LOGE("couldn't find PreferencesManager");
		return 0.0f;
	}

	jmethodID getPreferenceID = JavaEnvironment->GetMethodID(clazz, "getFloatPreference", "(Ljava/lang/String;)F");
	if( getPreferenceID == NULL )
	{
		LOGE(" getPreferenceID == NULL " );
		return 0.0f;
	}

	jstring jstr = JavaEnvironment->NewStringUTF(name.c_str());
	float returnValue = JavaEnvironment->CallFloatMethod(JavaPreferencesManager, getPreferenceID, jstr);

	return returnValue;
}

void JavaStuff::setFloatPreference( const std::string& name, float value )
{
	if( JavaEnvironment == NULL ) return;

	//LOGE("JavaStuff::setFloatPreference");

	jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/PreferencesManager");
	if( clazz == NULL )
	{
		LOGE("couldn't find PreferencesManager");
		return;
	}

	jmethodID setPreferenceID = JavaEnvironment->GetMethodID(clazz, "setFloatPreference", "(Ljava/lang/String;F)V");
	if( setPreferenceID == NULL )
	{
		LOGE("setPreferenceID == NUL " );
		return;
	}

	jstring jstr = JavaEnvironment->NewStringUTF(name.c_str());
	JavaEnvironment->CallVoidMethod(JavaPreferencesManager, setPreferenceID, jstr, value);
}

bool  JavaStuff::getBoolPreference( const std::string& name )
{
	if( JavaEnvironment == NULL ) return false;

	//LOGE("JavaStuff::getBoolPreference");

	jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/PreferencesManager");
	if( clazz == NULL )
	{
		LOGE("couldn't find PreferencesManager");
		return false;
	}

	jmethodID getPreferenceID = JavaEnvironment->GetMethodID(clazz, "getBoolPreference", "(Ljava/lang/String;)Z");
	if( getPreferenceID == NULL )
	{
		LOGE(" getPreferenceID == NULL " );
		return false;
	}

	jstring jstr = JavaEnvironment->NewStringUTF(name.c_str());
	bool returnValue = JavaEnvironment->CallBooleanMethod(JavaPreferencesManager, getPreferenceID, jstr);

	return returnValue;
}

void  JavaStuff::setBoolPreference( const std::string& name, bool value )
{
	if( JavaEnvironment == NULL ) return;

	//LOGE("JavaStuff::setBoolPreference");

	jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/PreferencesManager");
	if( clazz == NULL )
	{
		LOGE("couldn't find PreferencesManager");
		return;
	}

	jmethodID setPreferenceID = JavaEnvironment->GetMethodID(clazz, "setBoolPreference", "(Ljava/lang/String;Z)V");
	if( setPreferenceID == NULL )
	{
		LOGE("setPreferenceID == NUL " );
		return;
	}

	jstring jstr = JavaEnvironment->NewStringUTF(name.c_str());
	JavaEnvironment->CallVoidMethod(JavaPreferencesManager, setPreferenceID, jstr, value);

	//LOGE("JavaStuff::setBoolPreference Done");
}

void JavaStuff::resetPreferences()
{
#ifndef NDEBUG
	if( JavaEnvironment == NULL ) return;
	//LOGE("JavaStuff::resetPreferences");

	jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/PreferencesManager");
	if( clazz == NULL )
	{
		LOGE("couldn't find PreferencesManager");
		return;
	}

	jmethodID resetPreferencesID = JavaEnvironment->GetMethodID(clazz, "resetPreferences", "()V");
	if( resetPreferencesID == NULL )
	{
		LOGE("resetPreferencesID == NUL " );
		return;
	}

	JavaEnvironment->CallVoidMethod(JavaPreferencesManager, resetPreferencesID );
#endif
}

void JavaStuff::enableAds( bool enable )
{
/*	LOGE("JavaStuff::enableAds");

    jclass clazz = JavaEnvironment->FindClass("com/BandanaTech/BunnyRun/MainActivity");
    if( clazz == NULL )
    {
    	LOGE("MainActivity == NUL " );
    	return;
    }

    jmethodID enablesAds = JavaEnvironment->GetMethodID(clazz, "enablesAds","(Z)V");
    if( enablesAds == NULL )
    {
    	LOGE("enablesAds == NUL " );
    	return;
    }
    JavaEnvironment->CallBooleanMethod(JavaMainActivity, enablesAds, enable );

    LOGE("JavaStuff::enableAds Done");
*/
}


#else


PreferencesManager::PreferencesManager()
{
	std::string path = ShaderParams::get()->currentDir + string("preferences.xml");
	if( !doc.LoadFile( path.c_str() ) )
	{
		cout<<"New file"<<endl;

	    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "");
	    doc.LinkEndChild( decl );

	    TiXmlElement* Preferences = new TiXmlElement( "preferences" );
	    doc.LinkEndChild( Preferences );
	}
}

float PreferencesManager::getFloatPreference( const std::string& name )
{
	TiXmlHandle hdl(&doc);
	TiXmlElement* Preferences = hdl.FirstChildElement("preferences").Element();
	assert( Preferences );

	float result = 0.0f;

	TiXmlElement* PreferenceElement = Preferences->FirstChildElement("preference");
	while( PreferenceElement != NULL )
	{
		if( strcmp( PreferenceElement->Attribute( "name" ), name.c_str() ) == 0 )
		{
			double d = 0.0f;
			PreferenceElement->Attribute( "value", &d );
			result = d;
			break;
		}

		PreferenceElement = PreferenceElement->NextSiblingElement();
	}

	return result;
}

void PreferencesManager::setFloatPreference( const std::string& name, float value )
{
	TiXmlHandle hdl(&doc);
	TiXmlElement* Preferences = hdl.FirstChildElement("preferences").Element();
	assert( Preferences );

	// look for this element, if it's not there we will create it
	bool foundIt = false;
	TiXmlElement* PreferenceElement = Preferences->FirstChildElement("preference");
	while( PreferenceElement != NULL )
	{
		if( strcmp( PreferenceElement->Attribute( "name" ), name.c_str() ) == 0 )
		{
			PreferenceElement->SetDoubleAttribute( "value", (double) value );
			foundIt = true;
			break;
		}

		PreferenceElement = PreferenceElement->NextSiblingElement();
	}

	if( !foundIt )
	{
	    TiXmlElement* Preference = new TiXmlElement( "preference" );
	    Preference->SetAttribute( "name", name );
		Preference->SetDoubleAttribute( "value", (double)value );
	    Preferences->LinkEndChild( Preference );
	}

	doc.SaveFile();
}

bool PreferencesManager::getBoolPreference( const std::string& name )
{
	TiXmlHandle hdl(&doc);
	TiXmlElement* Preferences = hdl.FirstChildElement("preferences").Element();
	assert( Preferences );

	bool result = false;

	TiXmlElement* PreferenceElement = Preferences->FirstChildElement("preference");
	while( PreferenceElement != NULL )
	{
		if( strcmp( PreferenceElement->Attribute( "name" ), name.c_str() ) == 0 )
		{
			const char* resultString = PreferenceElement->Attribute( "value" ); // "true" or "false"
			assert( resultString != NULL );
			result = strcmp( resultString, "true" ) == 0;
			break;
		}

		PreferenceElement = PreferenceElement->NextSiblingElement();
	}

	return result;
}

void PreferencesManager::setBoolPreference( const std::string& name, bool value )
{
	TiXmlHandle hdl(&doc);
	TiXmlElement* Preferences = hdl.FirstChildElement("preferences").Element();
	assert( Preferences );

	// look for this element, if it's not there we will create it
	bool foundIt = false;
	TiXmlElement* PreferenceElement = Preferences->FirstChildElement("preference");
	while( PreferenceElement != NULL )
	{
		if( strcmp( PreferenceElement->Attribute( "name" ), name.c_str() ) == 0 )
		{
			PreferenceElement->SetAttribute( "value", value ? "true" : "false" );
			foundIt = true;
			break;
		}

		PreferenceElement = PreferenceElement->NextSiblingElement();
	}

	if( !foundIt )
	{
	    TiXmlElement* Preference = new TiXmlElement( "preference" );
	    Preference->SetAttribute( "name", name );
	    Preference->SetAttribute( "value", value ? "true" : "false" );
	    Preferences->LinkEndChild( Preference );
	}

	doc.SaveFile();
}


JavaStuff::JavaStuff()
{
	preferences = new PreferencesManager();

}

float JavaStuff::getFloatPreference( const std::string& name )
{
	return preferences->getFloatPreference(name);
}

void  JavaStuff::setFloatPreference( const std::string& name, float value )
{
	return preferences->setFloatPreference(name,value);
}

bool  JavaStuff::getBoolPreference( const std::string& name )
{
	return preferences->getBoolPreference(name);
}

void  JavaStuff::setBoolPreference( const std::string& name, bool value )
{
	return preferences->setBoolPreference(name,value);
}

void JavaStuff::enableAds( bool enable )
{

}

void JavaStuff::resetPreferences()
{

}


#endif
