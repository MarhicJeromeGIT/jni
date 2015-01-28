#include "fichiers.h"
#include "macros.h"
using namespace std;
#include "assert.h"
#include "CommonLibrary.h"
#include "CustomResource.h"

#ifdef __ANDROID__

MyAssetManager* MyAssetManager::thisObject = NULL;

MyAssetManager::MyAssetManager()
{

}

MyAssetManager* MyAssetManager::get()
{
	if( thisObject == NULL )
	{
		thisObject = new MyAssetManager();
	}
	return thisObject;
}

void MyAssetManager::setManager( AAssetManager* m )
{
	mgr = m;
}


unsigned char* readBuffer( const char* filename, long& size )
{
	AAssetDir* assetDir = AAssetManager_openDir(MyAssetManager::get()->mgr, "");
	const char* dirfilename = (const char*)NULL;
	while ((dirfilename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		if( strcmp(filename, dirfilename) != 0 )
			continue;

	    AAsset* asset = AAssetManager_open(MyAssetManager::get()->mgr, dirfilename, AASSET_MODE_UNKNOWN);
	    if( asset == NULL )
	    {
	    	LOGE("Can't find asset");
	    	continue;
	    }
	    size = AAsset_getLength(asset);
	    unsigned char* buffer = new unsigned char[size+1];
	    AAsset_read (asset,buffer,size);
	    AAsset_close(asset);
	    AAssetDir_close(assetDir);
	    buffer[size] = 0; // mmh mmh.

	    return buffer;

	}
	LOGE("COULDN't find file %s",filename);
	return NULL;
}

std::string readFile_string( const std::string& filename )
{
	return readFile( filename.c_str() );
}

string readFile(const char* filename)
{
	AAssetDir* assetDir = AAssetManager_openDir(MyAssetManager::get()->mgr, "");
	const char* dirfilename = (const char*)NULL;
	while ((dirfilename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		if( strcmp(filename, dirfilename) != 0 )
			continue;

	    AAsset* asset = AAssetManager_open(MyAssetManager::get()->mgr, dirfilename, AASSET_MODE_UNKNOWN);
	    if( asset == NULL )
	    {
	    	LOGE("Can't find asset");
	    	continue;
	    }
	    long size = AAsset_getLength(asset);
	    char* buffer = new char[size+1];
	    AAsset_read (asset,buffer,size);
	    AAsset_close(asset);
	    AAssetDir_close(assetDir);
	    buffer[size] = 0; // mmh mmh.

	    string toReturn = string(buffer);
	    delete[] buffer;
	    return toReturn;

	}
	LOGE("COULDN't find file %s",filename);
}

std::wstring readFileUnicode(const char* filename)
{
	//long size = 0;
	//unsigned char* bytes = readBuffer(filename, size);
	//wstring wstr(reinterpret_cast<wchar_t*>(bytes), size/sizeof(wchar_t));

	// actually just ansi for now
	string f = readFile(filename);
	//LOGE("%s", f.c_str());
	wstring ws;
	ws.assign(f.begin(), f.end());

	return ws;
}

#else

unsigned char* readBuffer( const char* filename, long& size )
{
#ifndef __BUILD_DATA__
	unsigned char* custom = CustomResource_ReadFile( filename, size );
	if( custom != NULL )
	{
		return custom;
	}
#endif

	FILE* f = fopen(filename, "r");
	if( f == NULL )
	{
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *str = new unsigned char[size+1];
	fread(str, size, 1, f);
	fclose(f);

	str[size] = 0;
	return str;
}

std::string readFile_string( const std::string& filename )
{
	return readFile( filename.c_str() );
}

string readFile(const char* filename)
{
	string custom = CustomResource_ReadFile(filename);
	if( custom.size() > 0 )
	{
		return custom;
	}

	std::ifstream fichier( filename , ios_base::in);

	if( fichier.fail() )
	{
		cout<<"couldn't open "<<filename<<endl;
		assert(false);
		return "";
	}
	// need error report here
	assert(fichier);

    std::stringstream buffer; 
    buffer << fichier.rdbuf();
    fichier.close();

    return buffer.str();
}

wstring readFileUnicode(const char* filename)
{
	wstring custom = CustomResource_ReadFileW(filename);
	if( custom.size() > 0 )
	{
		return custom;
	}

	std::wifstream fichier( filename , ios_base::in);

	if( fichier.fail() )
	{
		cout<<"couldn't open "<<filename<<endl;
		assert(false);
		return L"";
	}
	// need error report here
	assert(fichier);

    std::wstringstream buffer;
    buffer << fichier.rdbuf();
    fichier.close();

    return buffer.str();
}


#endif
