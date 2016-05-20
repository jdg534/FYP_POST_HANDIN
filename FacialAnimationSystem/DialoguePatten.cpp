#include "DialoguePattern.h"
#include <algorithm>
#include "ErrorLog.h"

#include <fstream> // for DialoguePattern::loadStatesFromFile

DialoguePattern::DialoguePattern()
{
	m_soundData = nullptr;
	m_dialogueStates.clear();
}

DialoguePattern::~DialoguePattern()
{
	delete m_soundData;
}

bool DialoguePattern::loadFromFile(std::string soundFile)
{
	m_soundData = new SoundData();
	if (!m_soundData->loadFromFile(soundFile))
	{
		// it didn't work
		ErrorLog * elInst = ErrorLog::getInstance();
		elInst->logMinorError("Couldn't load the dialogue file");
		delete m_soundData;
		return false;
	}
	return true;
}

bool DialoguePattern::loadStatesFromFile(std::string stateFile)
{
	std::ifstream sf(stateFile);
	if (!sf.good())
	{
		return false;
	}

	/* strusture:
	symbol startTimePointAsFloat endTimePointAsFloat
	...
	END
	*/
	std::string cb; // cb = Current Bit
	while (!sf.eof())
	{
		sf >> cb;
		if (cb != "END")
		{
			float start, end;
			sf >> start >> end;
			DialogueState ds;
			ds.stateStr = cb;
			ds.stateStart = start;
			ds.stateEnd = end;
			addDiaLogueState(ds);
			
		}
		else if (cb == "END")
		{
			break;
		}
	}

	sortDialogueStates();

	sf.close();
	return true;
}

void DialoguePattern::addDiaLogueState(DialogueState ds)
{
	m_dialogueStates.push_back(ds);
}

void DialoguePattern::sortDialogueStates()
{
	std::sort(m_dialogueStates.begin(), m_dialogueStates.end());
}

SoundData * DialoguePattern::getSoundData()
{
	return m_soundData;
}

void DialoguePattern::writeDialogueStates(std::ostream & out)
{
	out << "information for dialog file: " << m_dialogueStates.size() << " dislogue states identified" <<std::endl;
	for (int i = 0; i < m_dialogueStates.size(); i++)
	{
		out << "State " << i << ": " << m_dialogueStates[i].stateStr;
		out << " start " << m_dialogueStates[i].stateStart << " end " << m_dialogueStates[i].stateEnd << std::endl;
	}
}

std::vector<DialogueState> DialoguePattern::getDialogueStates()
{
	return m_dialogueStates;
}