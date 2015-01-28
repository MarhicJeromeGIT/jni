#ifndef _UTF_TRANSLATION_H_
#define _UTF_TRANSLATION_H_

#include <string>
#include <map>
using namespace std;


enum GAME_STRING
{
	END_LEVEL_TIME_SUCCESS,


	MAX_GAME_STRING,
};


struct TranslatedStrings
{
	wstring english;
	wstring translation;

	TranslatedStrings(){};
	TranslatedStrings( const wstring& eng, const wstring& tra )
	{
		english  = eng;
		translation = tra;
	}
};

enum LANGUAGES
{
	ENGLISH,
	FRENCH,
	KOREAN,
	MAX_LANGUAGES,
};

class TranslationDictionary
{
public:
	static LANGUAGES language;
	static std::hash<wstring> hash_fn;
	static std::multimap<size_t,TranslatedStrings> dictionary;
	static void load(LANGUAGES language = ENGLISH);


	static const wstring& translate( const wstring& eng );
};

void initTranslation(LANGUAGES language = ENGLISH);

const wstring& tr(const wstring& eng );

wstring format( const wstring& eng, ... );
wstring format( GAME_STRING gameString, ... );


#endif
