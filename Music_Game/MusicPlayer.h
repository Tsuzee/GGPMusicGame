#pragma once

#include "fmod.hpp"

class MusicPlayer
{
public:
	MusicPlayer();
	~MusicPlayer();

	void setSound(char* fileName);
	void play();
	void stop();
	void update();

private:

	FMOD::System     *system;
	FMOD::Sound      *sound = 0;
	FMOD::Channel    *channel = 0;
	FMOD_RESULT       result;
	unsigned int version;

	FMOD::DSP *spectrumDSP;
	FMOD_DSP_PARAMETER_FFT *fftparameter;

	unsigned int len = 1024;

	bool beamMark=false;
	float largestFreq=0;
	float lastFreq = 0;
	bool detected=false;

	void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
	#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)
public:
	bool IsDetected();
};



