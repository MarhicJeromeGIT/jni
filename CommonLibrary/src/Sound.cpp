#include "Sound.h"
#include "assert.h"
#include <iostream>
using namespace std;
#include "fichiers.h"
#include "math.h"
#include "JavaStuff.h"
#include "CommonLibrary.h"

//*************************************
// Sound Manager ( PC & android )
//
//*************************************

SoundManager* SoundManager::thisObject = NULL;

SoundManager::SoundManager()
{
}

SoundManager* SoundManager::get()
{
	if( thisObject == NULL )
	{
		thisObject = new SoundManager();
	}
	return thisObject;
}

void SoundManager::reload()
{
	for( auto it = SoundMap.begin(); it != SoundMap.end(); it++ )
	{
		(it->second)->reload();
	}
}

Sound* SoundManager::getSoundEffect( const std::string& bufferName )
{
	auto it = SoundMap.find( bufferName );
	if( it != SoundMap.end() )
	{
		return it->second;
	}
	else
	{
		Sound* newSound = new SoundEffect( bufferName );
		SoundMap.insert( std::pair< std::string,Sound*>( bufferName, newSound) );
		return newSound;
	}
}

Sound* SoundManager::getMusic( const std::string& bufferName )
{
	auto it = SoundMap.find( bufferName );
	if( it != SoundMap.end() )
	{
		return it->second;
	}
	else
	{
		Sound* newSound = new Music( bufferName );
		SoundMap.insert( std::pair< std::string,Sound*>( bufferName, newSound) );
		return newSound;
	}
}

void SoundManager::setSoundEffectVolume( float volume )
{
	for( auto it = SoundMap.begin(); it != SoundMap.end(); it++ )
	{
		//SoundEffect* sound = dynamic_cast<SoundEffect*>( (it->second) );
		SoundEffect* sound = (SoundEffect*)(it->second);
		if( sound != NULL )
		{
			sound->setVolume( volume );
		}
	}
}

void SoundManager::setMusicVolume( float volume )
{
	for( auto it = SoundMap.begin(); it != SoundMap.end(); it++ )
	{
		//Music* sound = dynamic_cast<Music*>( (it->second) );
		Music* sound = (Music*)(it->second);
		if( sound != NULL )
		{
			sound->setVolume( volume );
		}
	}
}

//*************************************
// Sound class
//
//*************************************

#ifdef __ANDROID__

jclass    SoundEffect::clazz;
jmethodID SoundEffect::loadSoundID;
jmethodID SoundEffect::playSoundID;
jmethodID SoundEffect::stopSoundID;
jmethodID SoundEffect::setVolumeID;
bool      SoundEffect::loadedJavaStuff = false;

SoundEffect::SoundEffect( const std::string& buf ) : Sound()
{
	LOGE("loading soundeffect %s",bufferName.c_str());
	JavaStuff* java = JavaStuff::get();
	if( !loadedJavaStuff )
	{
		loadedJavaStuff = true;

		clazz = java->JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/SoundManager");
		if( clazz == NULL ) return;
		clazz = reinterpret_cast<jclass>(java->JavaEnvironment->NewGlobalRef(clazz));

		loadSoundID = java->JavaEnvironment->GetMethodID(clazz, "loadSound", "(Ljava/lang/String;)I");
		if( loadSoundID == NULL ) return;

		playSoundID = java->JavaEnvironment->GetMethodID(clazz, "playSound", "(I)V");
		if( playSoundID == NULL ) return;

		setVolumeID = java->JavaEnvironment->GetMethodID(clazz, "setSoundVolume", "(F)V");
		if( setVolumeID == NULL ) return;

		stopSoundID = java->JavaEnvironment->GetMethodID(clazz, "stopSound", "(I)V");
		if( stopSoundID == NULL ) return;

		LOGE("loaded Sound Java stuff");
	}

	bufferName = buf;
	load();
}

void SoundEffect::load()
{
	JavaStuff* java = JavaStuff::get();
    jstring jstr = java->JavaEnvironment->NewStringUTF(bufferName.c_str());
    streamID = java->JavaEnvironment->CallIntMethod(java->JavaSoundManager, loadSoundID, jstr);

	LOGE("OK");
}

void SoundEffect::reload()
{
	load();
}

void SoundEffect::play()
{
	JavaStuff* java = JavaStuff::get();
	jint ji = streamID;
	java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager,playSoundID,ji );
}

void SoundEffect::stop()
{
	JavaStuff* java = JavaStuff::get();
	jint ji = streamID;
	java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager,stopSoundID,ji );
}

void SoundEffect::setVolume(float v)
{
	volume = v;
	JavaStuff* java = JavaStuff::get();
	java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager,setVolumeID,volume );
}


#else

SoundEffect::SoundEffect( const std::string& bufferName ) : Sound()
{
	cout<<"SoundEffect"<<endl;
	//system("pause");
	long size = 0;
	unsigned char* content = readBuffer( bufferName.c_str(), size );
	assert(content != NULL);
	cout<<size<<" bites of sound"<<endl;
	//system("pause");

	if (!buffer.loadFromMemory( content, size ) )
	{
		cout<<"couldn't find file "<<bufferName<<endl;
		assert( false );
	}
	
	sound = sf::Sound(buffer);
	sound.setLoop(false);
	sound.setVolume( 100.0f );
	isPlaying = false;

    // Display sound informations
    std::cout << bufferName<<" : " << std::endl;
    std::cout << " " << buffer.getDuration().asSeconds() << " seconds"       << std::endl;
    std::cout << " " << buffer.getSampleRate()           << " samples / sec" << std::endl;
    std::cout << " " << buffer.getChannelCount()         << " channels"      << std::endl;

	delete[] content;
	//system("pause");
}

void SoundEffect::load()
{
}

void SoundEffect::reload()
{
}

void SoundEffect::play()
{
//	system("pause");
	isPlaying = true;
	sound.play();
}

void SoundEffect::stop()
{
}

// volume in [0;1]
void SoundEffect::setVolume(float volume)
{
	// volume is not linear
	float v = 100.0f * (float) (1.0f - (log(100.0f - volume*100.0f) / log(100.0f)));
	sound.setVolume(v);
}

#endif


//*************************************
// Music class
//
//*************************************

#ifdef __ANDROID__

jclass    Music::clazz;
jmethodID Music::loadMusic;
jmethodID Music::playMusic;
jmethodID Music::setMusicVolume;
bool      Music::loadedJavaStuff = false;

Music::Music(const std::string& buf)
{
	JavaStuff* java = JavaStuff::get();
	if( !loadedJavaStuff )
	{
		loadedJavaStuff = true;

		clazz = java->JavaEnvironment->FindClass("com/BandanaTech/AlienSurf/SoundManager");
		if( clazz == NULL ) return;
		clazz = reinterpret_cast<jclass>(java->JavaEnvironment->NewGlobalRef(clazz));

		loadMusic = java->JavaEnvironment->GetMethodID(clazz, "loadMusic", "(Ljava/lang/String;)V");
		if( loadMusic == NULL ) return;

		playMusic = java->JavaEnvironment->GetMethodID(clazz, "playMusic", "()V");
		if( playMusic == NULL ) return;

		setMusicVolume = java->JavaEnvironment->GetMethodID(clazz, "setMusicVolume", "(F)V");
		if( setMusicVolume == NULL ) return;

		LOGE("loaded Music Java stuff");
	}

	bufferName = buf;
	load();
}

void Music::load()
{
	JavaStuff* java = JavaStuff::get();
    jstring jstr = java->JavaEnvironment->NewStringUTF(bufferName.c_str());
    java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager, loadMusic, jstr);
}

void Music::reload()
{
	load();
	setVolume(volume);
}

void Music::play()
{
	JavaStuff* java = JavaStuff::get();
	java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager,playMusic );
}

void Music::setVolume( float v )
{
	volume = v;
	JavaStuff* java = JavaStuff::get();
	java->JavaEnvironment->CallVoidMethod(java->JavaSoundManager,setMusicVolume, volume );
}

void Music::stop()
{

}

#else

Music::Music(const std::string& bufferName)
{
	long size = 0;
	unsigned char* content = readBuffer( bufferName.c_str(), size );
	assert(content != NULL);
	
	cout<<size<<" bites of music"<<endl;
	
	if (!music.openFromMemory(content,size) )
	{
		cout<<"couldn't find file "<<bufferName<<endl;
		assert(false);
	}
	music.setLoop(true);
	
}

void Music::load()
{
}

void Music::reload()
{
}

void Music::play()
{
	music.play();
}

void Music::stop()
{
	music.stop();
}

void Music::setVolume( float volume )
{
	// volume is not linear
	float v = 100.0f * (float) (1.0f - (log(100.0f - volume*100.0f) / log(100.0f)));
	music.setVolume( v);
}
#endif
