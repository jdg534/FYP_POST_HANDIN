#ifndef _DIALOGUE_H_
#define _DIALOGUE_H_

#include <string>
#include <vector>
#include <ostream>

#include "SoundData.h"

// this class to has to purpose of representing the dialoue

// to store a break down on the audio states, wit durations

// 

struct DialogueState
{
	float stateStart, stateEnd;
	std::string stateStr; // "SILENCE" or a phoneme symbol

	bool operator<(const DialogueState & comp) // for sorting
	{
		return stateStart < comp.stateStart;
	}
};

class DialoguePattern
{
public:
	DialoguePattern();
	~DialoguePattern();

	bool loadFromFile(std::string soundFile);

	bool loadStatesFromFile(std::string stateFile);

	void addDiaLogueState(DialogueState ds);

	void sortDialogueStates();

	SoundData * getSoundData();

	void writeDialogueStates(std::ostream & out);

	std::vector<DialogueState> getDialogueStates();
private:
	std::vector<DialogueState> m_dialogueStates; // this is to populated for the pattern recognition class
	// or one of its potential sub classes
	SoundData * m_soundData;
};


#endif