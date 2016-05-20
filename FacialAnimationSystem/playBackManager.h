#ifndef _PLAYBACK_MANAGER_H_
#define _PLAYBACK_MANAGER_H_

#include <fmod.hpp>


#include <vector>

class PlayBackManager // manages audio playback
{
public:
	
	~PlayBackManager();

	bool isInitialised()
	{
		return m_initialised;
	}

	bool init();
	void shutdown();

	FMOD::System * getSoundSystemPtr()
	{
		return m_soundSystem;
	}

	static PlayBackManager * getInstance();

	void playSound(FMOD::Sound * s, bool looped);

	void update(float dt);


private:
	PlayBackManager(); // singleton
	FMOD::System * m_soundSystem;
	FMOD::Channel * m_playChannel;
	unsigned int m_numChannels;

	FMOD::Sound * m_toPlay;

	bool m_initialised;
};


#endif