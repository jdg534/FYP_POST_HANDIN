#ifndef _AUDIO_PATTEN_H_
#define _AUDIO_PATTEN_H_

#include <string>

#include <vector>

#include "MathTypes.h"

struct AudioPattern
{
	float startPoint, endPoint;
	std::string symbol;

	unsigned int startIndex, endIndex;

	AudioPattern()
	{
		startIndex = endIndex = 0;
		startPoint = endPoint = 0.0f;
		symbol = "UNDEFINED";
	}

	bool operator < (const AudioPattern & other)
	{
		if (this->startPoint < other.startPoint)
		{
			return true;
		}
		return false;
	}
};

struct SoundDataVector2DListForm
{
	std::vector<Vector2D> samples;
};

enum EN_PEEK_TROTH_FLAT
{
	EN_PT_PEEK = 0,
	EN_PT_TROTH,
	EN_PT_FLAT,
};

struct PeekTroth
{
	// one instance of this per highest peek point / lowest troth point
	unsigned char EnumVal;
	Vector2D position; // X for time point, y for the height
};

struct PeekTrothLog
{
	unsigned int nPeeks, nTroths, nFlats,nSamples;
	std::vector<PeekTroth> peeksAndTroths;
};

#endif