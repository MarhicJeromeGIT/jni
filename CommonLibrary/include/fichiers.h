#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef __ANDROID__
	#include <jni.h>
	#include <android/log.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <android/asset_manager.h>
	#include <android/asset_manager_jni.h>

class MyAssetManager 
{
private:
	static MyAssetManager* thisObject;
	MyAssetManager();

public:
	AAssetManager* mgr;
	static MyAssetManager* get();
	void setManager( AAssetManager* mgr );
};

#endif

unsigned char* readBuffer( const char* filename, long& size );
std::string  readFile(const char* filename);
std::string  readFile_string( const std::string& filename );
std::wstring readFileUnicode(const char* filename);

