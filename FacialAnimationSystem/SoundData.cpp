#include "SoundData.h"

#include <fstream>

SoundData::SoundData()
{

}

SoundData::~SoundData()
{

}

bool SoundData::loadFromFile(std::string fileName)
{
	std::ifstream in(fileName, std::ios::binary);

	if (!in.good())
	{
		return false;
	}

	m_fileName = fileName;

	in.read(reinterpret_cast<char *>(&m_fileInfo), sizeof(WavHeader));
	
	if (m_fileInfo.numChannels > 1)
	{
		return false;
	}

	if (m_fileInfo.bitsPerSample == 8)
	{
		load8BitSamples(in);
	}
	else if (m_fileInfo.bitsPerSample == 16)
	{
		load16BitSamples(in);
	}
	else
	{
		return false;
	}
	in.close();

	// set the class's member variables
	m_samplingRate = (float)m_fileInfo.samplingRate;
	return true;
}

void SoundData::writeAsText(std::ostream & out)
{
	out << "Stats for: " << m_fileName.c_str() << std::endl << std::endl;
	m_fileInfo.writeToStream(out);
	out << "Samples (scaled to 8 bit signed values):" << std::endl;
	for (int i = 0; i < m_samples.size(); i++)
	{
		out << static_cast<short>( m_samples[i]) << std::endl;
	}
}

std::vector<signed char> SoundData::getSamples()
{
	std::vector<signed char> rv;
	for (int i = 0; i < m_samples.size(); i++)
	{
		rv.push_back(m_samples[i]);
	}
	return rv;
}

float SoundData::getDurationInSeconds()
{
	// result = (number of samples / number of channels) / sampling rate
	unsigned int totalSamplesPerChannel = m_samples.size() / m_fileInfo.numChannels;

	return (float) totalSamplesPerChannel / m_samplingRate;
}

std::vector<signed char> SoundData::getSamplesBetween(float startSeconds, float endSeconds)
{
	std::vector<signed char> rv;
	unsigned int startIndex = indexForNumberOfSecondsFromStart(startSeconds);
	unsigned int endIndex = indexForNumberOfSecondsFromStart(endSeconds);

	for (int i = startIndex; i <= endIndex; i++)
	{
		rv.push_back(m_samples[i]);
	}

	return rv;
}

void SoundData::load8BitSamples(std::ifstream & in)
{
	m_samples.resize(m_fileInfo.dataChunkSize);
	in.read(reinterpret_cast<char *>(&m_samples[0]), m_fileInfo.dataChunkSize);
}

void SoundData::load16BitSamples(std::ifstream & in)
{
	std::vector<signed short> toConvert;
	int nSamples = m_fileInfo.dataChunkSize / 2;
	toConvert.resize(nSamples);
	in.read(reinterpret_cast<char *>(&toConvert[0]), m_fileInfo.dataChunkSize);

	for (int i = 0; i < nSamples; i++)
	{
		char downScaled = ScaleShortToByte(toConvert[i]);
		m_samples.push_back(downScaled);
	}
}

unsigned int SoundData::indexForNumberOfSecondsFromStart(float secondsFromStart)
{
	if (secondsFromStart > getDurationInSeconds())
	{
		return m_samples.size() - 1;
	}
	float nSamplesCovered = secondsFromStart * m_samplingRate;
	return (unsigned int)nSamplesCovered;
}

char SoundData::ScaleShortToByte(short sample)
{
	// signed short value range: –32, 768 to 32, 767
	// signed char (byte) value range: –128 to 127

	float scaleBy = 1.0f / 32767.0f;
	float downScaled = static_cast<float>(sample)* scaleBy;

	float scaleUpBy = 127.0f;

	char rv = downScaled * scaleUpBy;

	return rv;
}