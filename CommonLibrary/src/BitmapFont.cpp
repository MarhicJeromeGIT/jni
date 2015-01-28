#include "BitmapFont.h"
#include <fstream>
#include <stdio.h>
#include <iostream>
#include "fichiers.h"
#include <algorithm>
#include <map>
#include "assert.h"

void splitString( const string& data, vector<string>& output, vector<char> delimiter )
{
	string currentSubStr = "";
	for( unsigned int i=0; i< data.size(); i++ )
	{
		bool isDelimiter = (std::find(delimiter.begin(), delimiter.end(), data[i]) != delimiter.end());
		if( isDelimiter )
		{
			if( currentSubStr.size() > 0 ) // don't push empty strings if there are several successive delimiters
			{
				output.push_back(currentSubStr);
			}
			currentSubStr.clear();
			continue;
		}
		else
		{
			currentSubStr += data[i];
		}
	}
	if( currentSubStr.size() > 0 )
	{
		output.push_back(currentSubStr);
	}
}

void extractIntoMap( const vector<string>& data, map<string,string>& output, const vector<char> delimiters )
{
	for( unsigned int i=0; i< data.size(); i++ )
	{
		vector<string> keyvalue;
		splitString(data[i], keyvalue, delimiters );
		if( keyvalue.size() == 2 )
		{
			//cout<<keyvalue[0]<<"=>"<<keyvalue[1]<<endl;
			output.insert( pair<string,string>( keyvalue[0], keyvalue[1] ) );
		}
	}
}

void BitmapFont::extractFontInfo( const vector<string>& fontInfo )
{
	// put everything in a map key-value, separated by the '=':
	map<string,string> infoMap;
	vector<char> delimiters;
	delimiters.push_back('=');
	extractIntoMap( fontInfo, infoMap, delimiters );
 
	// look for the useful infos :
	auto it = infoMap.find( "pages" );
	assert( it != infoMap.end() );

	nbPages = atoi((it->second).c_str());
	fontName = infoMap.find( "face" )->second;
}

void BitmapFont::load(const string& fontData )
{
	for( int i=0; i< charMax; i++ )
	{
		utfKey[i] = -1;
	}

	vector<string> splits;
	vector<char> delimiters;
	delimiters.push_back('\n');
	delimiters.push_back('\r');
	splitString(fontData, splits, delimiters );

	delimiters.push_back(' ');
	// 2 first lines : informations about the font
	vector<string> fontInfo;
	splitString(splits[0], fontInfo, delimiters );
	splitString(splits[1], fontInfo, delimiters );
	extractFontInfo( fontInfo );

	unsigned int currentLineIndex = 2;
	vector<char> equaldelimiter;
	equaldelimiter.push_back('=');

	for( int page=0; page<nbPages; page++ )
	{
		// page id=0 file="bitmapfontascii_0.tga"
		vector<string> pageInfo;
		splitString(splits[currentLineIndex], pageInfo, delimiters );
		currentLineIndex++;

		map<string,string> pageMap;
		extractIntoMap( pageInfo, pageMap, equaldelimiter );
		auto itp = pageMap.find( "file" );
		assert( itp != pageMap.end() );
		string tex = itp->second;
		texName.push_back( tex.substr(1, tex.size()-2) ); // remove the '"' in beginning and end of texname
		cout<<texName[page]<<endl;
	}

	//chars count=191
	vector<string> charCountInfo;
	splitString(splits[currentLineIndex], charCountInfo, delimiters );
	currentLineIndex++;

	map<string,string> charCountMap;
	extractIntoMap( charCountInfo, charCountMap, equaldelimiter );
	auto itc = charCountMap.find( "count" );
	assert( itc != charCountMap.end() );
	unsigned int nbChar = atoi((itc->second).c_str());
	cout<<"charcount = "<<nbChar<<endl;

	assert( nbChar + currentLineIndex <= splits.size() );
	for( unsigned int c=0; c< nbChar; c++ )
	{
		vector<string> charInfo;
		splitString(splits[currentLineIndex], charInfo, delimiters );
		currentLineIndex++;

		//char id=32   x=67    y=41    width=1     height=1     xoffset=0     yoffset=0     xadvance=14    page=0  chnl=15
		map<string,string> charInfoMap;
		extractIntoMap( charInfo, charInfoMap, equaldelimiter );

		CharacterFont character;
		character.charId   = atoi( charInfoMap.find( "id" )->second.c_str() );
		character.x        = atoi( charInfoMap.find( "x" )->second.c_str() );
		character.y        = atoi( charInfoMap.find( "y" )->second.c_str() );
		character.w        = atoi( charInfoMap.find( "width" )->second.c_str() );
		character.h        = atoi( charInfoMap.find( "height" )->second.c_str() );
		character.xoffset  = atoi( charInfoMap.find( "xoffset" )->second.c_str() );
		character.yoffset  = atoi( charInfoMap.find( "yoffset" )->second.c_str() );
		character.xadvance = atoi( charInfoMap.find( "xadvance" )->second.c_str() );
		character.pageNum  = atoi( charInfoMap.find( "page" )->second.c_str() );
		character.channel  = atoi( charInfoMap.find( "chnl" )->second.c_str() );

		// normalize
		character.x /= 256.0f;
		character.y /= 256.0f;
		character.y  = 1.0f - character.y;
		character.w /= 256.0f;
		character.h /= 256.0f;

		cout<<"char id : "<<character.charId<<" x :"<<character.x<<endl;
		characterMap.push_back(character);

		// Add this character UTF value to our global map:
		utfKey[ character.charId ] = c;
	}

	FirstCharacterIndex = characterMap[0].charId;
	LastCharacterIndex  = characterMap.back().charId;
}

#ifdef __ANDROID__

BitmapFont::BitmapFont( const char* directory, const char* filename )
{
	string pathname = string(directory) + string(filename);
	string fontData = readFile(pathname.c_str());

	load(fontData);
}

void BitmapFont::drawWString( const wstring& sentence )
{
}

#else
	#include <boost/regex.hpp>

BitmapFont::BitmapFont( const char* directory, const char* filename )
{
	string pathname = string(directory) + string(filename);
	string fontData = readFile(pathname.c_str());

	load(fontData);
}

void BitmapFont::drawWString( const wstring& sentence )
{
	for( unsigned int i=0; i< sentence.length(); i++ )
	{
		int c = (int)sentence[i];
		cout<<c<<endl;
	}
}


#endif





