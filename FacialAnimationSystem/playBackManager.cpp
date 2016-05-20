#include "playBackManager.h"

#include <fmod_errors.h>
#include <iostream>

PlayBackManager * f_ptr_instance = nullptr; // file scope instance (only code in this file can see this)

PlayBackManager::PlayBackManager()
{
	m_initialised = false;
	m_playChannel = nullptr;
	m_toPlay = nullptr;
}

PlayBackManager::~PlayBackManager()
{
	f_ptr_instance = nullptr;
}

bool PlayBackManager::init()
{
	FMOD_RESULT initResult;


	initResult = FMOD::System_Create(&m_soundSystem);

	if (initResult != FMOD_OK)
	{
		std::cout << "Error the couldn't create FMOD SoundSystem" << std::endl;
		std::cout << FMOD_ErrorString(initResult) << std::endl;
		return false;
	}

	m_numChannels = 5;

	initResult = m_soundSystem->init(m_numChannels, FMOD_INIT_NORMAL, 0);
	if (initResult != FMOD_OK)
	{
		std::cout << "Error couldn't initialise the FMOD SoundSystem" << std::endl;
		std::cout << FMOD_ErrorString(initResult) << std::endl;
		return false;
	}
	m_soundSystem->update();

	 

	m_initialised = true;
	return m_initialised;
}

void PlayBackManager::shutdown()
{
	m_initialised = false;
	m_soundSystem->release();
}

PlayBackManager * PlayBackManager::getInstance()
{
	if (f_ptr_instance == nullptr)
	{
		f_ptr_instance = new PlayBackManager();
	}
	return f_ptr_instance;
}

void PlayBackManager::playSound(FMOD::Sound * s, bool looped)
{
	/*
	if (m_playChannel == nullptr)
	{
		m_soundSystem->getChannel(1, &m_playChannel);
	}
	*/

	m_toPlay = s;
	m_soundSystem->playSound(m_toPlay, nullptr, false, &m_playChannel);
}

void PlayBackManager::update(float dt)
{
	bool soundPlaying = true;
	m_playChannel->isPlaying(&soundPlaying);
	if (!soundPlaying)
	{
		m_soundSystem->playSound(m_toPlay, nullptr, false, &m_playChannel);
	}
}