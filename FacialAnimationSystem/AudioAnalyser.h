#ifndef _AUDIO_ANALYSER_H_
#define _AUDIO_ANALYSER_H_

#include "PhonemeMap.h"

#include "DialoguePattern.h"

#include "AudioPattern.h"


/*
algorithums to for feature extraction:
// according to: https://www.youtube.com/watch?v=i9Gn2QYrYpo

Perceptual Linear Prediction (PLP)
Relative spectra filtering of log domain coefficients PLP (RASTA-PLP)
Linear predictive coding (LPC)
Predictive cepstral coefficients (LPCC)
Mel scale cepstral analysis (MEL)
Mel-frequency cepstral coefficients (MFCC)
Outdated
Power spectral analysis (FFT)
First order derivative (DELTA)

// can skip the "compare & select word selection" stage
HMM at this stage, does need written about in the report
"Deep Neural Network", is a type of Neural Network

// can skip the "Compare & select sentence level match" stage


according to video (order of simplisity, taking into account word & sentence recognition)
LPC or LPCC with a HMM
PLP or MFCC with a HMM
MFCC with a HMM
MFCC with a HMM & trainning algorithum
RASTA-PLP with a HMM and Training Algorithum
DNN

in pratic will be able to drop sections


*/




class AudioAnalyser
{
public:
	void analyseAudio_singlePhoneme(DialoguePattern * toAnalyse);
	void analyseAudio_seperatePhoneme(DialoguePattern * dpToA);

	void analyseAudio_fixedStep(DialoguePattern * dp);

	void analyseAudio(DialoguePattern * dp);

	void setSilenceThreshold(signed char silenceThreshold){ m_silenceThreshold = silenceThreshold; }

	void setPhonemeMap(PhonemeMap * pm){ m_phonemeMapPtr = pm; }
private:
	PhonemeMap * m_phonemeMapPtr; // not deleted in this class

	signed char m_silenceThreshold;

	PeekTrothLog determinePeeksTroths(const std::vector<signed char> & toAnalyse);
	unsigned int comparePeekTroth(const PeekTrothLog & a, const PeekTrothLog & b);

	std::vector<signed char> getWithoutStartAndEndSilence(const std::vector<signed char> & samples);

	float shortestPhonemeDuration();

	float shortestPhonemeDurationSimple();

	static SoundDataVector2DListForm getAsVec2DForm(std::vector<signed char> & soundData);

	static float differenceBetween(SoundDataVector2DListForm * a, SoundDataVector2DListForm * b);

	static void rescaleLength(SoundDataVector2DListForm * toScale, float scaleTo);

	static void getPointsOnLeftAndRight(SoundDataVector2DListForm * points, Vector2D targetPoint ,Vector2D * leftPoint, Vector2D * rightPoint);

	static Vector2D lerpedVecFormSample(Vector2D a, Vector2D b, float targetX);

};


#endif