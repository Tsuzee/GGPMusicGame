#include "MusicPlayer.h"
#include "fmod_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

MusicPlayer::MusicPlayer()
{
	result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
	}

	result = system->init(32, FMOD_INIT_NORMAL, NULL);
	ERRCHECK(result);
}

MusicPlayer::~MusicPlayer()
{
	if (sound) {
		result = sound->release();
		ERRCHECK(result);
	}

	result = system->close();
	ERRCHECK(result);

	result = system->release();
	ERRCHECK(result);
}

void MusicPlayer::setSound(char *fileName)
{
	char *filePath = (char *)calloc(256, sizeof(char));

	strcat(filePath, "Assets/musics/");
	strcat(filePath, fileName);

	result = system->createSound(filePath, FMOD_DEFAULT, 0, &sound);
	ERRCHECK(result);

	result = sound->setMode(FMOD_LOOP_OFF);    /* prevent embedded loop points which automatically makes looping turn on, */
	ERRCHECK(result);      

	result = system->createDSPByType(FMOD_DSP_TYPE_FFT, &spectrumDSP);
	ERRCHECK(result);

	result = spectrumDSP->setParameterInt((int)FMOD_DSP_FFT_WINDOWSIZE, 1024);
	ERRCHECK(result);

	result = spectrumDSP->setParameterInt((int)FMOD_DSP_FFT_WINDOWTYPE, (int)FMOD_DSP_FFT_WINDOW_HAMMING);
	ERRCHECK(result);
}

void MusicPlayer::play() {
	result = system->playSound(sound, 0, false, &channel);
	ERRCHECK(result);

	result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, spectrumDSP);
	ERRCHECK(result);

	result = channel->setVolume(0.1);
	ERRCHECK(result);
}

void MusicPlayer::update() {
	if (!sound)
		return;

	result = system->update();
	ERRCHECK(result);

	result = spectrumDSP->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fftparameter, &len, 0, 0);
	ERRCHECK(result);
}

void MusicPlayer::ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	if (result != FMOD_OK)
	{
		printf("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}


// TODO:: get frequency

////for (int channel = 0; channel < fftparameter->numchannels; channel++) {
//for (int channel = 0; channel < (fftparameter->numchannels == 0 ? 0 : 1); channel++) {
//	for (int bin = 1; bin < 10; ++bin) {
//		float freqVal = fftparameter->spectrum[channel][bin];
//
//		Common_Draw("%f", freqVal);
//	}
//}

	 // http://support.ircam.fr/docs/AudioSculpt/3.0/co/FFT%20Size.html
	 // http://www.decibelcar.com/articles/43-theory-and-physics/141-treble-hertz.html
	 // https://katyscode.wordpress.com/2013/01/16/cutting-your-teeth-on-fmod-part-4-frequency-analysis-graphic-equalizer-beat-detection-and-bpm-estimation/