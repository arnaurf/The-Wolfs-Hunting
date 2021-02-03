#include "Audio.h"

Audio::Audio(HSAMPLE sample)
{
	this->sample = sample;
}

Audio::~Audio() 
{
}

HCHANNEL Audio::play(float volume) 

{
	BASS_SetVolume(volume);

	channel = BASS_SampleGetChannel(sample, false);
	//Lanzamos un sample
	BASS_ChannelPlay(channel, true);
	return channel;
}

void Audio::Stop(HCHANNEL channel) 
{
	//BASS_SampleStop(this);
}
