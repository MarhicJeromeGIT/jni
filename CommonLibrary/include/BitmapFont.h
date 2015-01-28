#ifndef _BITMAP_FONT_H_
	#define _BITMAP_FONT_H_

#include <string>
#include <vector>
using namespace std;



struct CharacterFont
{
	float bitmap_x, bitmap_y;
	float bitmap_w, bitmap_h;

	float x,y;
	float w,h;
	float xoffset,yoffset;
	float xadvance;
	int pageNum;
	int channel;

	int charId; 
};


class BitmapFont
{
public:
	string fontName;

	static const int charMax = 64000;
	int utfKey[charMax]; // UTFkey -> index dans mon characterMap

	vector<string> texName; // texture id lookup
	int FirstCharacterIndex;
	int LastCharacterIndex;
	vector<CharacterFont> characterMap;
	
public:
	BitmapFont( const char* directory, const char* filename );
	void load(const string& fontData );

	void drawWString( const wstring& sentence ); 

	int nbPages;
private:
	void extractFontInfo( const vector<string>& fontInfo );
};

#endif
