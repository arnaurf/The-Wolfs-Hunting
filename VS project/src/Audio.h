#include "extra/bass.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef AUDIO_H
#define AUDIO_H

class Audio
{
public:
	HSAMPLE sample; //aqui guardamos el handler del sample que retorna BASS_SampleLoad
	HCHANNEL channel;

	Audio(HSAMPLE sample); //importante poner sample a cero aqui
	~Audio(); //aqui deberiamos liberar el sample con BASS_SampleFree

	HCHANNEL play(float volume); //lanza el audio y retorna el channel donde suena

	static void Stop(HCHANNEL channel); //para parar un audio necesitamos su channel
};
#endif