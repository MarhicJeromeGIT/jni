#ifndef JAVASTUFF_H
#define JAVASTUFF_H

#include "tinyxml.h"

#ifdef __ANDROID__
// for native audio
#include "jni.h"
#endif

#include <map>
#include <string>

#ifndef __ANDROID__

class PreferencesManager
{
private:
	TiXmlDocument doc;

public:
	PreferencesManager();

	float getFloatPreference( const std::string& name );
	void  setFloatPreference( const std::string& name, float value );

	bool  getBoolPreference( const std::string& name );
	void  setBoolPreference( const std::string& name, bool value );
};

#endif

class JavaStuff
{
public:
#ifdef __ANDROID__
	JNIEnv* JavaEnvironment;
	jobject JavaSoundManager;
	jobject JavaPreferencesManager;
	jobject JavaMainActivity;
#else
	PreferencesManager* preferences;
#endif

	static JavaStuff* get();

	float getFloatPreference( const std::string& name );
	void  setFloatPreference( const std::string& name, float value );

	bool  getBoolPreference( const std::string& name );
	void  setBoolPreference( const std::string& name, bool value );

	void  resetPreferences();

	void enableAds(bool enable);
private:
	static JavaStuff* thisObject;
	JavaStuff();
};


#endif
