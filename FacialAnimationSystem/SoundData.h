#ifndef _SOUND_DATA_H_
#define _SOUND_DATA_H_

#include <ostream>
#include <vector>

struct WavHeader
{
	char riffMagicNum[4]; // "RIFF"
	unsigned int riffChunkSize;
	
	
	char formatMagicNum[4]; // "WAVE"
	char formatChunkMagicNum[4]; // "fmt "
	unsigned int formatChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned int samplingRate;
	unsigned int bytesPerSec;
	unsigned short bytesPerBlock;
	unsigned short bitsPerSample;

	char dataChunkMagicNum[4]; // "data"
	unsigned int dataChunkSize;
	// rest of the file is the raw PCM data

	std::ostream & operator<<(std::ostream & out)
	{
		out << ".Wav file header data:" << std::endl;

		out << "riffMagicNumber: " << riffMagicNum << std::endl;
		out << "riffChunkSize: " << riffChunkSize << std::endl << std::endl;

		out << "formatMagicNum: " << formatMagicNum << std::endl;
		out << "formatChunkMagicNum: " << formatChunkMagicNum << std::endl;
		out << "formatChunkSize: " << formatChunkSize << std::endl;
		out << "audioFormat (1 = PCM): " << audioFormat << std::endl;
		out << "numChannels: " << numChannels << std::endl;
		out << "samplingRate: " << samplingRate << std::endl;
		out << "bytesPerSec: " << bytesPerSec << std::endl;
		out << "bytesPerBlock: " << bytesPerBlock << std::endl;
		out << "bitsPerSample: " << bitsPerSample << std::endl << std::endl;

		out << "dataChunkMagicNum: " << dataChunkMagicNum << std::endl;
		out << "dataChunkSize: " << dataChunkSize << std::endl;

		return out;
	}

	void writeToStream(std::ostream & out)
	{
		out << ".Wav file header data:" << std::endl;

		out << "riffMagicNumber: " << riffMagicNum << std::endl;
		out << "riffChunkSize: " << riffChunkSize << std::endl << std::endl;

		out << "formatMagicNum: " << formatMagicNum << std::endl;
		out << "formatChunkMagicNum: " << formatChunkMagicNum << std::endl;
		out << "formatChunkSize: " << formatChunkSize << std::endl;
		out << "audioFormat (1 = PCM): " << audioFormat << std::endl;
		out << "numChannels: " << numChannels << std::endl;
		out << "samplingRate: " << samplingRate << std::endl;
		out << "bytesPerSec: " << bytesPerSec << std::endl;
		out << "bytesPerBlock: " << bytesPerBlock << std::endl;
		out << "bitsPerSample: " << bitsPerSample << std::endl << std::endl;

		out << "dataChunkMagicNum: " << dataChunkMagicNum << std::endl;
		out << "dataChunkSize: " << dataChunkSize << std::endl;
	}
};

class SoundData
{
public:
	SoundData();
	~SoundData();
	bool loadFromFile(std::string fileName);
	void writeAsText(std::ostream & out);

	std::vector<signed char> getSamples();
	float getDurationInSeconds();
	std::vector<signed char> getSamplesBetween(float startSeconds, float endSeconds);

	float getSamplingRate(){ return m_samplingRate; }

private:
	void load8BitSamples(std::ifstream & in);
	void load16BitSamples(std::ifstream & in);

	unsigned int indexForNumberOfSecondsFromStart(float secondsFromStart);

	char ScaleShortToByte(short sample);

	float m_samplingRate;
	float m_durationSeconds;
	std::vector<signed char> m_samples; // will down scale the audio data
	WavHeader m_fileInfo;
	std::string m_fileName;
};


#endif