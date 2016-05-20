#include "AudioAnalyser.h"

#include "AudioPattern.h"

#include "MathTypes.h"
#include "Math.h"

#include <algorithm>

//#ifdef _DEBUG
#include <iostream>
//#endif

// #include <aquila.h>


void AudioAnalyser::analyseAudio_singlePhoneme(DialoguePattern * toAnalyse)
{
	// determine the highest sampling rate, that all phonemes & the Dialogue support
	float srToUse = toAnalyse->getSoundData()->getSamplingRate();
	std::vector<std::string> phonemeSymbols;
	phonemeSymbols = m_phonemeMapPtr->getPhonemeSymbols();
	for (int i = 0; i < phonemeSymbols.size(); i++)
	{
		// determine the sampling rate used for each phoneme
		Phoneme * toCheck = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i]);
		float toChkSr = toCheck->soundDataPtr->getSamplingRate();
		
		if (toChkSr < srToUse)
		{
			srToUse = toChkSr;
		}
	}

	// now that determined the sampling rate need to geth the audio data into a comparable form
	// try the peaks and troths approach Bob mentioned


	// find and rule out the silence sections of the dialogue
	std::vector<signed char> dataToAnalyse = toAnalyse->getSoundData()->getSamples();

	

	PeekTrothLog dialoguePeeksTrothLog = determinePeeksTroths(dataToAnalyse);
	

	// do same for each phoneme, then compare the PeekTrothLog(to find the most similar phoneme)

	// just running with TestPhonemeA.wav, for now, don't bother with the silence sections

	
	
	float currentSimilarity = 0.0f; // needed an initial value
	int phonemeIndex = 0;

	for (int i = 0; i < phonemeSymbols.size(); i++)
	{
		std::vector<signed char> phonemeSoundData = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i])->soundDataPtr->getSamples();

		std::vector<signed char> trimedPhonemeData = getWithoutStartAndEndSilence(phonemeSoundData);

		// PeekTrothLog phonemePeekTrothLog = determinePeeksTroths(phonemeSoundData);
		PeekTrothLog phonemePeekTrothLog;
		
		if (trimedPhonemeData.size() > 5)
		{
			phonemePeekTrothLog = determinePeeksTroths(trimedPhonemeData);
		}
		else
		{
			phonemePeekTrothLog = determinePeeksTroths(phonemeSoundData);
		}
		

		// now do the comparision
		std::cout << "Comparing Dialogue to phoneme: " << phonemeSymbols[i] << std::endl;

		

		// pick up here!!!
		unsigned int simlarityToCurrentPhoneme = comparePeekTroth(dialoguePeeksTrothLog, phonemePeekTrothLog);

		std::cout << "Similarity: " << simlarityToCurrentPhoneme << std::endl;

		if (simlarityToCurrentPhoneme > currentSimilarity)
		{
			currentSimilarity = simlarityToCurrentPhoneme;
			phonemeIndex = i;
		}
	}

	std::cout << "best match: " << phonemeSymbols[phonemeIndex] << std::endl;

	DialogueState dsToAdd;
	dsToAdd.stateStart = 0.0f;
	dsToAdd.stateStr = phonemeSymbols[phonemeIndex];
	dsToAdd.stateEnd = toAnalyse->getSoundData()->getDurationInSeconds();
	toAnalyse->addDiaLogueState(dsToAdd);
	toAnalyse->sortDialogueStates();
}

void AudioAnalyser::analyseAudio_seperatePhoneme(DialoguePattern * dpToA)
{
	// determine the highest sampling rate, that all phonemes & the Dialogue support
	float srToUse = dpToA->getSoundData()->getSamplingRate();
	std::vector<std::string> phonemeSymbols;
	phonemeSymbols = m_phonemeMapPtr->getPhonemeSymbols();
	for (int i = 0; i < phonemeSymbols.size(); i++)
	{
		// determine the sampling rate used for each phoneme
		Phoneme * toCheck = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i]);
		float toChkSr = toCheck->soundDataPtr->getSamplingRate();

		if (toChkSr < srToUse)
		{
			srToUse = toChkSr;
		}
	}

	// now that determined the sampling rate need to geth the audio data into a comparable form
	// try the peaks and troths approach Bob mentioned


	// find and rule out the silence sections of the dialogue
	std::vector<signed char> dataToAnalyse = dpToA->getSoundData()->getSamples();

	std::vector<bool> isSound;
	for (int i = 0; i < dataToAnalyse.size(); i++)
	{
		if (dataToAnalyse[i] > m_silenceThreshold)
		{
			isSound.push_back(true);
		}
		else if (dataToAnalyse[i] < -m_silenceThreshold)
		{
			isSound.push_back(true);
		}
		else
		{
			isSound.push_back(false);
		}
	}

	// use isSound to determine the silence points
	std::vector<float> silenceStartPoints, silenceEndPoints;

	// Aquila::Mfcc aquliaMFCC(30);
	
	

		
	bool lastValue = isSound[0];

	if (!lastValue)
	{
		silenceStartPoints.push_back(0.0f);
	}

	float scaleSamplePositionToSeconds = 1.0f / dpToA->getSoundData()->getSamplingRate();
	

	for (int i = 1; i < isSound.size(); i++)
	{
		if (isSound[i])
		{
			// non silent point
			if (!lastValue)
			{
				// was dealing with silence previously
				float silenceEndPoint = i * scaleSamplePositionToSeconds;
				silenceEndPoints.push_back(silenceEndPoint);
			}
		}
		else
		{
			// silent point
			if (lastValue)
			{
				// was dealing with non silent point previously
				float silenceStartPoint = i * scaleSamplePositionToSeconds;
				silenceStartPoints.push_back(silenceStartPoint);
			}
		}
		lastValue = isSound[i];
	}

	if (silenceEndPoints.size() < silenceStartPoints.size())
	{
		silenceEndPoints.push_back(isSound.size() / scaleSamplePositionToSeconds);
	}

	float dialogueSampleRate = dpToA->getSoundData()->getSamplingRate();

	// figure out the silence points
	
	std::string slienceSymbol = "SILENCE";
	std::vector<AudioPattern> soundPaterns;
	for (int i = 0; i < silenceStartPoints.size(); i++)
	{
		AudioPattern ap;
		ap.startPoint = silenceStartPoints[i];
		ap.startIndex = ap.startPoint * dialogueSampleRate;
		ap.endPoint = silenceEndPoints[i];
		ap.endIndex = ap.endPoint * dialogueSampleRate;
		ap.symbol = slienceSymbol;
		soundPaterns.push_back(ap);
	}

	std::sort(soundPaterns.begin(), soundPaterns.end());

	
	// see what can be done with Aquila
	

	// ???
	float a;

	/*
	float smallestPhonemeGap = shortestPhonemeDuration();
	
	for (int i = 1; i < soundPaterns.size(); i++)
	{
		// calculate the gap between the silence points
		float withSoundGap = soundPaterns[i].startPoint - soundPaterns[i - 1].endPoint;
		if (withSoundGap < smallestPhonemeGap)
		{
			soundPaterns.erase(soundPaterns.begin() + i);
			i = 0;
		}
	}
	*/
}

void AudioAnalyser::analyseAudio_fixedStep(DialoguePattern * dp)
{
	// determine the avg phoneme length
	float avgPhonemeDuration = 0.0f;

	std::vector<std::string> phonemeSymbols = m_phonemeMapPtr->getPhonemeSymbols();

	unsigned short nPhonemes = phonemeSymbols.size();

	for (int i = 0; i < nPhonemes; i++)
	{
		Phoneme * p = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i]);
		avgPhonemeDuration += p->soundDataPtr->getDurationInSeconds();
	}

	avgPhonemeDuration = avgPhonemeDuration / nPhonemes;

	// step through the the dialogue with step size of avgPhonemeLength
	float stepStart = 0.0f, stepEnd = avgPhonemeDuration;
	
	SoundData * dialogSD = dp->getSoundData();

	float dialogDuration = dialogSD->getDurationInSeconds();

	std::cout << "Analising the Dialogue using steps of size: " << avgPhonemeDuration << " seconds" << std::endl;
	std::cout << "Dialogue duration: " << dialogDuration << " seconds" << std::endl;


	while (stepStart < dialogDuration)
	{
		std::vector<signed char> dialogSamplesCurrentStep = dialogSD->getSamplesBetween(stepStart, stepEnd);
		SoundDataVector2DListForm dsCurrStep2DVecForm = getAsVec2DForm(dialogSamplesCurrentStep);
		unsigned short mostLikelyPhoneme = 0; // have a silence phoneme that is the first phoneme, in the map
		float differenceToCurrentMostLikelyPhoneme = FLT_MAX; // max value a float can represent

		float xLengthDialogCurrentStep = dsCurrStep2DVecForm.samples[dsCurrStep2DVecForm.samples.size() - 1].x;



		for (unsigned short i = 0; i < nPhonemes; i++)
		{
			Phoneme * p = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i]);
			std::vector<signed char> phonemeSamples = p->soundDataPtr->getSamples();
			SoundDataVector2DListForm phoneme2DVecForm = getAsVec2DForm(phonemeSamples);

			float xLengthPhoneme = phoneme2DVecForm.samples[phoneme2DVecForm.samples.size() - 1].x;
			if (xLengthPhoneme != xLengthDialogCurrentStep)
			{
				// rescale phoneme2DVecForm
				rescaleLength(&phoneme2DVecForm, xLengthDialogCurrentStep);
			}



			std::cout << "comparing dialog section (" << stepStart << " - " << stepEnd << ") with Phoneme: " << p->symbol << std::endl;
			float diff = differenceBetween(&dsCurrStep2DVecForm, &phoneme2DVecForm);

			std::cout << "difference: " << diff << std::endl;

			if (diff < differenceToCurrentMostLikelyPhoneme)
			{
				differenceToCurrentMostLikelyPhoneme = diff;
				mostLikelyPhoneme = i;
				std::cout << "So far, most likely phoneme for section (" << stepStart << " - " << stepEnd << ") is: " << p->symbol << " only " << diff << " difference" << std::endl;

			}
		}

		DialogueState dStateToAdd;
		dStateToAdd.stateStart = stepStart;
		dStateToAdd.stateEnd = stepEnd;
		dStateToAdd.stateStr = phonemeSymbols[mostLikelyPhoneme];

		std::cout << "Identified Phoneme in Audio: " << dStateToAdd.stateStr << std::endl;

		dp->addDiaLogueState(dStateToAdd);

		stepStart = stepEnd;
		stepEnd += avgPhonemeDuration;
		std::cout << dialogDuration - stepStart << " seconds of dialogue to analyse " << std::endl;

	}
}

void AudioAnalyser::analyseAudio(DialoguePattern * dp)
{
	// four steps!
	/*
	Step 1: identify the windows. (window = sound data that isn't silence)
	Step 2: Apply FFT / DFT to samples in the window
	Step 3: Convert to MFCC (or simmilar)
	Step 4: put MFCC vectors through a HMM or DTW
	*/

	// step 1
	SoundData * dpSndData = dp->getSoundData();

	std::vector<signed char> dpSamples = dpSndData->getSamples();
	float dpSndSampleRate = dpSndData->getSamplingRate();
	std::vector<AudioPattern> windows;
	
	bool currentSectionIsAWindow = false;
	bool lastSampleAboveSilenceThreshold = false;
	unsigned int indexOfLastStateChange = 0;
	for (unsigned int i = 1; i < dpSamples.size(); i++)
	{
		if (dpSamples[i - 1] > m_silenceThreshold || dpSamples[i - 1] < -m_silenceThreshold)
		{
			// it's sound
			if (!lastSampleAboveSilenceThreshold)
			{
				// last sample was silence
				indexOfLastStateChange = i;
			}
			lastSampleAboveSilenceThreshold = true;
		}
		else
		{
			// it's silence
			if (lastSampleAboveSilenceThreshold)
			{
				// last sample was sound
				// create the window
				AudioPattern windowToAdd;
				windowToAdd.startIndex = indexOfLastStateChange;
				windowToAdd.endIndex = i;
				windowToAdd.symbol = "UNKNOWN_PHONEME";
				windowToAdd.startPoint = (float) windowToAdd.startIndex / dpSndSampleRate;
				windowToAdd.endPoint = (float) windowToAdd.endIndex / dpSndSampleRate;
				windows.push_back(windowToAdd);
				indexOfLastStateChange = i;
			}
			lastSampleAboveSilenceThreshold = false;
		}
	}

	unsigned int nWindows = windows.size();

	float minWindowLength = shortestPhonemeDurationSimple();

	unsigned int indexOfWindowBeingStreched = 0;

	//Aquila::RectangularWindow rectWindow(windows[0].endIndex - windows[0].startIndex);
	



	for (unsigned int i = indexOfWindowBeingStreched + 1; i < windows.size(); i++)
	{
		float windowDuration = windows[indexOfWindowBeingStreched].endPoint - windows[indexOfWindowBeingStreched].startPoint;
		if (windowDuration < minWindowLength)
		{
			// delete the i+1th window & streach the current window untill its big enough
			if (i < windows.size() - 1)
			{
				// can merge with a later window
				windows[indexOfWindowBeingStreched].endIndex = windows[i + 1].endIndex;
				windows[indexOfWindowBeingStreched].endPoint = windows[i + 1].endPoint;
				windows.erase(windows.begin() + i + 1);
				
				i = indexOfWindowBeingStreched + 1;
			}
			else
			{
				break; // done all posable merges
			}
		}
		else
		{
			indexOfWindowBeingStreched++;
		}
	}

	// update nWindows
	nWindows = windows.size();

	indexOfWindowBeingStreched = 0;
	// now merge the over lapping windows
	for (unsigned int i = indexOfWindowBeingStreched + 1; i < windows.size() - 1; i++)
	{
		if (windows[i].endIndex > windows[i+1].startIndex)
		{
			// merge the windows
			windows[i].endIndex = windows[i+1].endIndex;
			windows.erase(windows.begin() + i + 1);
			i = 0;
		}
	}

	// try the Aquila version of windowing


	for (unsigned int i = 0; i < nWindows; i++)
	{
		// step 2 convert frequency space, FFT of DFT
		unsigned int sizeOfWindowSampleSize = windows[i].endIndex - windows[i].startIndex;
	}
}

PeekTrothLog AudioAnalyser::determinePeeksTroths(const std::vector<signed char> & toAnalyse)
{
	signed char startingValue = toAnalyse[0], secondValue,
		lastValue = startingValue;

	if (toAnalyse.size() < 2)
	{
		secondValue = startingValue;
	}
	else
	{
		secondValue = toAnalyse[1];
	}

	char processing = EN_PT_FLAT; // uses the enum to store what we're currently dealing with

	if (secondValue > startingValue)
	{
		processing = EN_PT_PEEK;
	}
	else if (secondValue < startingValue)
	{
		processing = EN_PT_TROTH;
	}
	else
	{
		processing = EN_PT_FLAT;
	}

	PeekTrothLog rv;
	rv.nSamples = toAnalyse.size();


	for (unsigned int i = 1; i < toAnalyse.size(); i++)
	{
		switch (processing)
		{
		case EN_PT_PEEK:
			// if still increasing, will be >=, else end go to troth mode
			if (toAnalyse[i] >= lastValue)
			{
				// still going up
			}
			else
			{
				// go to troth mode & log i as the top of the peek
				PeekTroth pt;
				pt.EnumVal = EN_PT_PEEK;
				pt.position.x = (float)i;
				pt.position.y = (float)toAnalyse[i];
				processing = EN_PT_TROTH;
				rv.peeksAndTroths.push_back(pt);
			}
			break;
		case EN_PT_TROTH:
			if (toAnalyse[i] <= lastValue)
			{
				// still going down
			}
			else
			{
				// go to peek mode & log i as the bottom of the troth
				PeekTroth pt;
				pt.EnumVal = EN_PT_TROTH;
				pt.position.x = (float)i;
				pt.position.y = (float)toAnalyse[i];
				processing = EN_PT_PEEK;
				rv.peeksAndTroths.push_back(pt);

			}
			break;
		case EN_PT_FLAT:
			if (toAnalyse[i] == lastValue)
			{
				// still flat
			}
			else if (toAnalyse[i] > lastValue)
			{
				// transition to peek
				PeekTroth pt;
				pt.EnumVal = EN_PT_FLAT;
				pt.position.x = (float)i;
				pt.position.y = (float)toAnalyse[i];
				rv.peeksAndTroths.push_back(pt);
				processing = EN_PT_PEEK;
			}
			else if (toAnalyse[i] < lastValue)
			{
				// transition to troth
				PeekTroth pt;
				pt.EnumVal = EN_PT_FLAT;
				pt.position.x = (float)i;
				pt.position.y = (float)toAnalyse[i];
			}
			break;
		}
		lastValue = toAnalyse[i - 1];
	}

	// now that identified peeks the troths, determine the number of each type
	rv.nFlats = 0; // ignoring flats for now
	rv.nTroths = 0;
	rv.nPeeks = 0;
	
	for (int i = 0; i < rv.peeksAndTroths.size(); i++)
	{
		if (rv.peeksAndTroths[i].EnumVal == EN_PT_PEEK)
		{
			rv.nPeeks++;
		}
		else if (rv.peeksAndTroths[i].EnumVal == EN_PT_TROTH)
		{
			rv.nTroths++;
		}
		else if (rv.peeksAndTroths[i].EnumVal == EN_PT_FLAT)
		{
			rv.nFlats++;
		}
	}
	
	return rv;
}

unsigned int AudioAnalyser::comparePeekTroth(const PeekTrothLog & a, const PeekTrothLog & b)
{
	unsigned int uintMax = 4294967296;
	
	unsigned int diffNumSamples;
	if (a.nSamples > b.nSamples)
	{
		diffNumSamples = a.nSamples - b.nSamples;
	}
	else
	{
		diffNumSamples = b.nSamples - a.nSamples;
	}
	
	unsigned int diffNumPeeks;
	if (a.nPeeks > b.nPeeks)
	{
		diffNumPeeks = a.nPeeks - b.nPeeks;
	}
	else
	{
		diffNumPeeks = b.nPeeks - a.nPeeks;
	}

	unsigned int diffNumTroths;
	if (a.nTroths > b.nTroths)
	{
		diffNumTroths = a.nTroths - b.nTroths;
	}
	else
	{
		diffNumTroths = b.nTroths - a.nTroths;
	}

	unsigned int diffNumFlats;
	if (a.nFlats > b.nFlats)
	{
		diffNumFlats = a.nFlats - b.nFlats;
	}
	else
	{
		diffNumFlats = b.nFlats - a.nFlats;
	}

	// compare peek and troth order ()

	int nComparablePeekTroths = 0;
	if (a.peeksAndTroths.size() > b.peeksAndTroths.size())
	{
		nComparablePeekTroths = b.peeksAndTroths.size();
	}
	else
	{
		nComparablePeekTroths = a.peeksAndTroths.size();
	}

	// will be using Vector2 magnitude to calculate difference between samples
	// will use a sum of the difference
	float sumSampleDifference = 0.0f;

	for (unsigned int i = 0; i < nComparablePeekTroths; i++)
	{
		Vector2D aiToBi;
		aiToBi.x = b.peeksAndTroths[i].position.x - a.peeksAndTroths[i].position.x;
		aiToBi.y = b.peeksAndTroths[i].position.y - a.peeksAndTroths[i].position.y;
		
		float aiToBiMag = Math::VectorMath::magnitude(aiToBi);

		sumSampleDifference += aiToBiMag;
	}


	// now try with only comparing when both are using the same enum
	float sumSampleDiffWithSameEnum = 0.0f;
	for (unsigned int i = 0; i < nComparablePeekTroths; i++)
	{
		if (a.peeksAndTroths[i].EnumVal == b.peeksAndTroths[i].EnumVal)
		{
			Vector2D aiToBi;
			aiToBi.x = b.peeksAndTroths[i].position.x - a.peeksAndTroths[i].position.x;
			aiToBi.y = b.peeksAndTroths[i].position.y - a.peeksAndTroths[i].position.y;

			float aiToBiMag = Math::VectorMath::magnitude(aiToBi);
			sumSampleDiffWithSameEnum += aiToBiMag;
		}
	}

	unsigned nPeekTrothInSameOrder = 0
		, nPeekTrothInSameOrderSum = 0;

	for (int i = 0; i < nComparablePeekTroths; i++)
	{
		// amount to add to the sum goes up if there's a match
		// when NO match, sum += amount to add & rest amount to add to 0 

		if (a.peeksAndTroths[i].EnumVal == b.peeksAndTroths[i].EnumVal)
		{
			nPeekTrothInSameOrder++;
		}
		else
		{
			nPeekTrothInSameOrderSum += nPeekTrothInSameOrder;
			nPeekTrothInSameOrder = 0; 
		}
	}

	nPeekTrothInSameOrderSum += nPeekTrothInSameOrder; // could have all been in the same order

	unsigned int diff = 0;

	diff += diffNumSamples;
	diff += diffNumPeeks;
	diff += diffNumTroths;
	diff += diffNumFlats;
	diff += (unsigned int)sumSampleDifference;
	diff += (unsigned int)sumSampleDiffWithSameEnum;

	unsigned int sim = uintMax - diff;
	
	sim += nPeekTrothInSameOrderSum;

	return sim;
	
}

std::vector<signed char> AudioAnalyser::getWithoutStartAndEndSilence(const std::vector<signed char> & samples)
{
	int firstSilenceStart, firstSilenceEnd,
		lastSilenceStart, lastSilenceEnd;

	firstSilenceStart = 0;

	lastSilenceEnd = samples.size() - 1;

	std::vector<bool> isSound;

	for (int i = 0; i < samples.size(); i++)
	{
		bool val = samples[i] > m_silenceThreshold;
		isSound.push_back(val);
	}



	// first loop forward to end of the silence
	for (int i = 0; i < isSound.size(); i++)
	{
		if (isSound[i])
		{
			// hit sound
			firstSilenceEnd = i;
			break;
		}
	}

	// second loop backward to last bit of sound
	for (int i = isSound.size() - 1; i > 0; i--)
	{
		if (isSound[i])
		{
			// hit sound
			lastSilenceStart = i;
			break;
		}
	}

	// now just populate & return rv
	std::vector<signed char> rv;
	for (int i = firstSilenceEnd; i < lastSilenceStart; i++)
	{
		rv.push_back(samples[i]);
	}
	return rv;
}

float AudioAnalyser::shortestPhonemeDuration()
{
	std::vector<std::string> ps = m_phonemeMapPtr->getPhonemeSymbols();
	float rv = 10.0f; // phoneme's arn't likely to last more than 10 seconds 

	for (int i = 0; i < ps.size(); i++)
	{
		Phoneme * p = m_phonemeMapPtr->getPhonemeWithSymbol(ps[i]);
		std::vector<signed char> pd = p->soundDataPtr->getSamples();
		float psr = p->soundDataPtr->getSamplingRate();
		std::vector<signed char> pdTrimed = getWithoutStartAndEndSilence(pd);

		float downScale = 1.0f / psr;
		float phonemeDuration;
		if (pdTrimed.size() > 100) // a phoneme will likely have more than 100 samples
		{
			phonemeDuration = (pdTrimed.size() - 1) * downScale;
		}
		else
		{
			phonemeDuration = (pd.size() - 1) * downScale;
		}
		
		if (rv > phonemeDuration)
		{
			rv = phonemeDuration;
		}
	}
	return rv;
}

float AudioAnalyser::shortestPhonemeDurationSimple()
{
	float shortest = FLT_MAX;

	unsigned int nPhones;
	std::vector<std::string> phonemeSymbols = m_phonemeMapPtr->getPhonemeSymbols();
	nPhones = phonemeSymbols.size();
	for (unsigned int i = 0; i < nPhones; i++)
	{
		Phoneme * pPtr = m_phonemeMapPtr->getPhonemeWithSymbol(phonemeSymbols[i]);
		float currentPhonemeDuration = pPtr->soundDataPtr->getDurationInSeconds();
		if (shortest > currentPhonemeDuration)
		{
			shortest = currentPhonemeDuration;
		}
	}
	return shortest;
}

SoundDataVector2DListForm AudioAnalyser::getAsVec2DForm(std::vector<signed char> & soundData)
{
	SoundDataVector2DListForm rv;

	// down scale heights from 0 - 255 to 0.0 - 1.0

	float downScale = 1.0f / 255.0f;

	for (unsigned int i = 0; i < soundData.size(); i++)
	{
		Vector2D tmp;
		tmp.x = i;
		tmp.y = soundData[i];
		tmp.y *= downScale;
		rv.samples.push_back(tmp);
	}
	return rv;
}

float AudioAnalyser::differenceBetween(SoundDataVector2DListForm * a, SoundDataVector2DListForm * b)
{
	using namespace Math::VectorMath;

	// asumes that a & b have the same X value at start and end

	// see: analyseAudio_fixedStep for way made that asumption

	bool loopThroughA = a->samples.size() < b->samples.size();

	// both have the Same X values at start and end some lerp will be needed

	float diff = 0.0f;

	float maxDiff = 0.0f;


	if (a->samples.size() == b->samples.size()) // is a & b have exactly the same number of samples
	{
		maxDiff = a->samples.size() - 1;
		for (int i = 0; i < a->samples.size(); i++)
		{
			Vector2D aToB = wayToVector(a->samples[i], b->samples[i]);
			diff += magnitude(aToB);
		}
	}

	else if (loopThroughA) 
	{
		maxDiff = a->samples.size();
		// A has less samples than b, loop through A & get lerped values when needed
		for (int i = 0; i < a->samples.size(); i++)
		{
			if (a->samples[i].x == b->samples[i].x)
			{
				Vector2D aToB = wayToVector(a->samples[i], b->samples[i]);
				diff += magnitude(aToB);
			}
			else
			{
				// get the two surrounding data point in b, then get a lerped point
				Vector2D leftPoint, rightPoint;
				
				Vector2D targetPoint = a->samples[i];

				getPointsOnLeftAndRight(b, targetPoint, &leftPoint, &rightPoint);

				// get a lerped vec bweteen leftPoint & right point the would have the same X as targetPoint.x
				Vector2D lerpedVec = lerpedVecFormSample(leftPoint, rightPoint, targetPoint.x);

				if (lerpedVec.x != targetPoint.x)
				{
					// somthing went wrong
					char e;
				}
				
				Vector2D targetToLerpedVec = wayToVector(targetPoint, lerpedVec);
				diff += magnitude(targetToLerpedVec);
			}
		}
	}
	else
	{
		maxDiff = b->samples.size();

		// like above but looping through B instead
		for (int i = 0; i < b->samples.size(); i++)
		{
			if (a->samples[i].x == b->samples[i].x)
			{
				Vector2D bToA = wayToVector(b->samples[i], a->samples[i]);
				diff += magnitude(bToA);
			}
			else
			{
				// get the two surrounding data point in b, then get a lerped point
				Vector2D leftPoint, rightPoint;

				Vector2D targetPoint = b->samples[i];

				getPointsOnLeftAndRight(a, targetPoint, &leftPoint, &rightPoint);

				// get a lerped vec bweteen leftPoint & right point the would have the same X as targetPoint.x
				Vector2D lerpedVec = lerpedVecFormSample(leftPoint, rightPoint, targetPoint.x);

				if (lerpedVec.x != targetPoint.x)
				{
					// somthing went wrong
					
				}

				Vector2D targetToLerpedVec = wayToVector(targetPoint, lerpedVec);
				diff += magnitude(targetToLerpedVec);
			}
 		}
	}

	// rescale diff to range 0.0 - 1.0

	float scaleDiffBy = 1.0f / maxDiff;

	return diff * scaleDiffBy;
}

void AudioAnalyser::rescaleLength(SoundDataVector2DListForm * toScale, float scaleTo)
{
	float startingLength = toScale->samples[toScale->samples.size() - 1].x;

	float scaleXBy = scaleTo / startingLength;

	Matrix2x2 scaleMat = Math::transform::get2x2ScalingMatrix(scaleXBy, 1.0f);

	for (int i = 0; i < toScale->samples.size(); i++)
	{
		Matrix2x1 initial, result;
		initial.r1c1 = toScale->samples[i].x;
		initial.r2c1 = toScale->samples[i].y;
		result = Math::MatrixMath::multiply(scaleMat, initial);
		toScale->samples[i].x = result.r1c1;
		toScale->samples[i].y = result.r2c1;
	}
}

void AudioAnalyser::getPointsOnLeftAndRight(SoundDataVector2DListForm * points, Vector2D targetPoint, Vector2D * leftPoint, Vector2D * rightPoint)
{
	// rember that points in "points" are sorted by x

	
	// float rightPointVal = FLT_MAX;

	// need to make i close to target point, this code needs to be FAST, maybe called frequently at load time
	int i = 0;

	// determine x step up size (will be uniform)
	float xStepSize = points->samples[1].x - points->samples[0].x;

	float StepsIn = targetPoint.x / xStepSize;

	i = (int)floorf(StepsIn);

	// i should now have a starting index that's close to targetPoint


	for (; i < points->samples.size() - 1; i++)
	{
		if (points->samples[i].x < targetPoint.x) 
			// repeatedly overrides the value
			// the final value would the closest value to the target point
		{
			leftPoint->x = points->samples[i].x;
			leftPoint->y = points->samples[i].y;
		}

		if (points->samples[i].x > targetPoint.x)
		{
			/*
			if (points->samples[i].x < rightPointVal)
			{
				rightPointVal = points->samples[i].x;
				rightPoint->x = points->samples[i].x;
				rightPoint->y = points->samples[i].y;
			}
			*/
			// since samples is sorted in order of .x, the first value will do
			rightPoint->x = points->samples[i].x;
			rightPoint->y = points->samples[i].y;
			return;
		}
	}
	
	
	// done, leftPoint & right point will have the values of the left & right sides of the target points
}

Vector2D AudioAnalyser::lerpedVecFormSample(Vector2D a, Vector2D b, float targetX)
{
	// asumes a.x < b.x

	// figue out the lerp value
	float lrDiffInX = b.x - a.x;
	float lerpVal;
	if (lrDiffInX == 0.0f)
	{
		lerpVal = 1.0f;
	}
	else
	{
		float lToTargetDiff = targetX - a.x;
		lerpVal = lToTargetDiff / lrDiffInX;
	}

	// figured out the lerp value

	return Math::interpolation::lerp(a, b, lerpVal);
}