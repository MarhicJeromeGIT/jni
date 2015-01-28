#include "UTFTranslation.h"
#include <iostream>
using namespace std;
#include <stdarg.h>
#include <vector>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <sstream>
#include "CommonLibrary.h"
#include "TTFont.h"

std::hash<wstring> TranslationDictionary::hash_fn;
std::multimap<size_t,TranslatedStrings> TranslationDictionary::dictionary;
LANGUAGES TranslationDictionary::language;

struct dico
{
	wstring english;
	wstring french;
	wstring korean;
};

static map<GAME_STRING,dico> gameStrings;

void TranslationDictionary::load(LANGUAGES lang /*= ENGLISH*/)
{
	// Change the font family depending on the language :
	if( lang == ENGLISH || lang == FRENCH )
	{
		FontManager::get()->getCurrentFont()->setDefaultFontFamily( FONTFAMILY::FRANKY );
	}
	else
	{
		FontManager::get()->getCurrentFont()->setDefaultFontFamily( FONTFAMILY::NANUM );
	}

	language = lang;
	dictionary.clear();

	if( language == ENGLISH )
	{
		cout<<"Loading English"<<endl;
		return;
	}

	vector<dico> strings;
/*	strings.push_back( dico{ L"Start Game", L"Nouvelle Partie", L"시작" } );
	strings.push_back( dico{ L"Costumes", L"Costumes", L"스킨" } );
	strings.push_back( dico{ L"NEW RECORD : |YELLOW|{0:d}|END| meters !",
							 L"Nouveau record : |YELLOW|{0:d}|END| mètres !",
							 L"신기록 : |YELLOW|{0:d}|END| 미터 !" });
	strings.push_back( dico{ L"(Previously {0:d} meters)",
							 L"(Record précédent {0:d} mètres)",
							 L"(이전 기록 {0:d} 미터)" });
	strings.push_back( dico{ L"New Costume Unlocked !",
							 L"Nouveau Costume Débloqué !",
							 L"새로운 스킨 얻었습니다 !" });
	strings.push_back( dico{ L"Music Volume", L"Volume Musique", L"음악 볼륨" });
	strings.push_back( dico{ L"Sound Effects Volume", L"Volume Effets", L"효과음 볼륨" });
	strings.push_back( dico{ L"Settings", L"Options", L"설정" });
	strings.push_back( dico{ L"Credits", L"Crédits", L"크레디트" });
	strings.push_back( dico{ L"Help", L"Aide", L"도움" });
	strings.push_back( dico{ L"Costume {0:d}/4 : {1:s}",
							 L"Costume {0:d}/4 : {1:s}",
							 L"스킨 {0:d}/4 : {1:s}" });
	strings.push_back( dico{ L"Dodge the obstacles to run as far as possible !",
							 L"Evite les obstacles pour aller le plus loin possible !",
							 L"상자 피하고 날아라 멀리 뛰어고라 !" });
	strings.push_back( dico{ L"Beat records to unlock costumes !",
							 L"Débloque des nouveaux costumes !",
							 L"멀리 가서 스킨 언락 !" });
	strings.push_back( dico{ L"Tap twice to Double Jump !",
							 L"Réappuie pour un double saut !",
							 L"2번 누르고 더블점프 !" });
	strings.push_back( dico{ L"Tap to Jump",
							 L"     Saut",
							 L"누르고 점프" });
	strings.push_back( dico{ L"Tap to Crouch",
							 L"     Esquive",
							 L"누르고 쭈그리다" });
	strings.push_back( dico{ L"Personal Best : {0:d} meters",
							 L"Record actuel : {0:d} mètres",
							 L"최고 기록 : {0:d} 미터" });
*/
	for( unsigned int i=0; i< strings.size(); i++ )
	{
		TranslatedStrings tr = TranslatedStrings( strings[i].english, language == FRENCH ? strings[i].french : strings[i].korean );
		if( tr.translation.size() == 0 )
		{
			tr.translation = tr.english;
		}

		dictionary.insert( pair<size_t, TranslatedStrings>(hash_fn(strings[i].english), tr ) );
	}

}


const wstring& TranslationDictionary::translate( const wstring& eng )
{
	if( language == ENGLISH )
	{
		return eng;
	}

	size_t hash = hash_fn(eng);
	int count = dictionary.count(hash);

	if( count == 1 )
	{
		std::multimap<size_t, TranslatedStrings>::iterator it = dictionary.equal_range(hash).first;
		return (*it).second.translation;
	}
	else if( count > 1 )
	{
		std::multimap<size_t, TranslatedStrings>::iterator it;
		for (it=dictionary.equal_range(hash).first; it!=dictionary.equal_range(hash).second; ++it)
		{
			TranslatedStrings& translation = (*it).second;
			// compare character par character...
			if( translation.english == eng )
			{
				return translation.translation;
			}
		}
	}

	cout<<"No traduction found for "<<eng.c_str()<<endl;
	return eng;
}

void initTranslation(LANGUAGES lang /*= ENGLISH*/)
{
	TranslationDictionary::load(lang);


/*	gameStrings.insert( pair<GAME_STRING,dico>(  END_LEVEL_TIME_SUCCESS,
												 dico{ L"Finished in less than {0:d}:{1:d} ! (|GREEN|{2:d}:{3:d}|END|)",
													   L"Finished in less than {0:d}:{1:d} ! (|GREEN|{2:d}:{3:d}|END|)",
													   L"Finished in less than {0:d}:{1:d} ! (|GREEN|{2:d}:{3:d}|END|)" } ) );
*/
}

const wstring& tr(const wstring& eng )
{
	return TranslationDictionary::translate( eng );
}

wstring format( GAME_STRING gameString, ... )
{
	auto it = gameStrings.find( gameString );
	assert( it != gameStrings.end() );
	const wstring& eng = it->second.english;

	va_list va;
	va_start(va,gameString);
	return format( eng, va );
}

wstring format( const wstring& eng, ... )
{
	const wstring& translated = tr(eng);
	wstring formated = L"";

    const int maxargs = 10;
    wstring argsType[maxargs]; // store the argument type, eg 'd' (integer), 's' (string)...

    // count how many arguments we have, get their type, and format them into wstrings :
    size_t pos = 0;

    while(pos != eng.npos )
    {
    	pos = eng.find( '{', pos );
    	if( pos != translated.npos )
    	{
    		// format is : {0:d}
    		wstring wargNum = eng.substr( pos+1, 1 );
    		int argNum = wcstol( wargNum.c_str(), NULL, 10 ); // wcstol converts wstring to int
    		wstring wvalue = eng.substr( pos + 3, 1 );
    		argsType[argNum] = wvalue;
    		pos++;
    	}
    }

    // ok so we know what kind of arguments to expect, fetch them from the _va_args list :
    wstring args[maxargs];
    va_list listPointer;
    va_start( listPointer, eng );
    for( int i=0; i< maxargs; i++ )
    {
    	if( argsType[i].empty() )
    	{
    		// we don't use this arg but I have to read it anyway :
    		int intarg = va_arg( listPointer, int );
    		(void)intarg;
    	}
    	else
    	{
    		switch( argsType[i][0] )
    		{
    			case 'd':
    			{
    				int intarg = va_arg( listPointer, int );

#if 1 // no swprintf support on android
    				std::wstringstream wss;
    				wss << intarg;
    				args[i] = wss.str();
#else
    				wchar_t buffer[32];
    				int cx = swprintf ( buffer, 32, L"%d", intarg );
    				LOGE("%d caracteres ecrits pour %d \n", cx, intarg );
    				args[i] = wstring(buffer);
#endif
    			//	LOGE("NOUVEL ARGUMENT %d \n", intarg);
    			}
    			break;

    			case 's':
    			{
    				wchar_t* warg = va_arg( listPointer, wchar_t* );
    				args[i] = wstring(warg);
    			}
    			break;

    			default:
    			{
    				cout<<"format parameter "<<args[i][0]<<" unrecognized"<<endl;
    			}
    			break;
    		}
    	}
    }
    va_end( listPointer );

    for( int i=0; i< maxargs; i++ )
    {
    //	LOGE("arg %d %zu %ls \n",i,args[i].size(),args[i].c_str() );
    }
    // copy the translated string and replaces the parameters by their values
    for( unsigned int i=0; i< translated.size(); i++ )
    {
        if( translated[i] == '{' )
        {
        	i++;
    		wstring wargNum = translated.substr( i, 1 );
    		int argNum = wcstol( wargNum.c_str(), NULL, 10 ); // wcstol converts wstring to int
    		assert( 0 <= argNum && argNum < maxargs );
    		formated += args[argNum];

    		//LOGE("ARGUMENT  %d \n", argNum);
    		//LOGE("SIZE = %zu", args[argNum].size() );
    		i = translated.find( '}', i);
        }
        else
        {
        	formated += translated[i];
        }
    }

	return formated;
}




