#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#ifdef __ANDROID__
// for native audio
#include "jni.h"
#else
#include "SFML/Audio.hpp"
#endif

#include <map>
#include <string>

class Sound;
class SoundEffect;
class Music;

class SoundManager
{
private:
	std::map<std::string, Sound*> SoundMap;

	SoundManager();
	static SoundManager* thisObject;
public:
	static SoundManager* get();
	void reload();
	Sound* getSoundEffect( const std::string& bufferName );
	Sound* getMusic( const std::string& bufferName );

	void setSoundEffectVolume( float volume ); // [0;1]
	void setMusicVolume( float volume );       // [0;1]
 
#ifdef __ANDROID__

#endif
};

// sound effect or music
class Sound
{
public:
	virtual ~Sound(){}

	virtual void load() = 0;
	virtual void reload() = 0;
	virtual void play() = 0;
	virtual void setVolume( float volume ) = 0;
	virtual void stop() = 0;

	float volume;
};

class SoundEffect : public Sound
{
	friend class SoundManager;
public:
	void load();
	void reload();
	void play();
	void stop();
	// volume in [0;1]
	void setVolume(float volume);

protected:
	bool isPlaying;
	std::string bufferName;

	SoundEffect(const std::string& bufferName);
	~SoundEffect(){}

#ifdef __ANDROID__
	static jclass    clazz;
	static jmethodID loadSoundID;
	static jmethodID playSoundID;
	static jmethodID stopSoundID;
	static jmethodID setVolumeID;
	static bool      loadedJavaStuff;

	int streamID;
#else
	sf::SoundBuffer buffer;
	sf::Sound sound;
#endif
};

class Music : public Sound
{
public:

	Music(const std::string& bufferName);
	~Music(){}

	void load();
	void reload();
	void play();
	void stop();
	void setVolume( float volume );

#ifdef __ANDROID__
	static jclass    clazz;
	static jmethodID loadMusic;
	static jmethodID playMusic;
	static jmethodID setMusicVolume;
	static bool      loadedJavaStuff;
#else
	sf::Music music;
#endif

	std::string bufferName;
};



#endif
