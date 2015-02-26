// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include "/usr/include/alsa/asoundlib.h"
#include "/usr/include/alsa/pcm.h"
#include <pthread.h>
#include "modEditor.c"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#define MIDIDEVICE		"/dev/snd/midiC2D0"
static char *ALSAdevice = "plughw:1,0";

int MIDIin_d;
int MIDIout_d;
int AUDIOout_d;

pthread_t patchconnector;
	
snd_output_t *output = NULL;
snd_pcm_sframes_t frames;
snd_pcm_t *handle;

int16_t audiobuffer1[1024]; 
int16_t audiobuffer2[1024]; 
volatile unsigned char activebuffer = 0;
volatile unsigned char bufferfull = 0;
snd_pcm_t *handle;

unsigned char globalmididata;

int MIDIin(unsigned char *data) {
	return read(MIDIin_d, data, 1 );
}

void MIDIout(unsigned char outbuf) {
	unsigned char temp;
	
	write(MIDIout_d, &outbuf, 1);
	return;
}

int AudioFIFOfull(void) { // this is not used with ALSA. 
	return 0;
}
void busspy(int bus) {
	static int il;
	static float lastval;
	il++;
	if (il==100) {
		if (lastval!=patchBus[bus][togglerOut]) {
			printf("%03d %03f \n",bus,patchBus[bus][togglerOut]);
			lastval=patchBus[bus][togglerOut];
		}
		il=0;
	}
}

void AudioOut(void) {
	return;/*
	static int samplecount = 0;
	static int buffersamples = sizeof(audiobuffer1)/2;
	snd_pcm_sframes_t frames;
	switch (activebuffer) {
		case 0x0:
			audiobuffer1[samplecount]=(int16_t)(patchBus[OUTL][togglerOut]*0x7FFF);
			samplecount++;
			audiobuffer1[samplecount]=(int16_t)(patchBus[OUTR][togglerOut]*0x7FFF);
			samplecount++;
			break;
		case 0xFF:
			audiobuffer2[samplecount]=(int16_t)(patchBus[OUTL][togglerOut]*0x7FFF);
			samplecount++;
			audiobuffer2[samplecount]=(int16_t)(patchBus[OUTR][togglerOut]*0x7FFF);
			samplecount++;
			break;
		default:
			printf("buffer toggle error\n");
			activebuffer=0;
			break;
	}
	if (samplecount == buffersamples) {
		samplecount=0;
		if (activebuffer==0) {
			while ( frames = snd_pcm_writei(handle, audiobuffer1, sizeof(audiobuffer1)/4) == EAGAIN)
				;
		}
		else {
			while ( (frames = snd_pcm_writei(handle, audiobuffer2, sizeof(audiobuffer2)/4)) == EAGAIN)
				;
		}
		activebuffer = ~activebuffer;
	}
	return;
	*/
}


int main(void)
{
	int err;
	unsigned int i;
	snd_pcm_sframes_t frames;
	int flags;

	printf("ALSA Hardware abstration layer\n  MIDI:  ");
	printf(MIDIDEVICE);printf("\n  Audio: %s\n",ALSAdevice);

	// Init MIDI interface
 	MIDIin_d  = open(MIDIDEVICE,O_RDONLY);
	MIDIout_d = open(MIDIDEVICE,O_WRONLY);

	flags = fcntl(MIDIin_d, F_GETFL, 0);
	fcntl(MIDIin_d, F_SETFL, flags | O_NONBLOCK);

    // Init ALSA sound device
 	snd_pcm_open(&handle, ALSAdevice, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SAMPLERATE, 0, 100000 );

	//printf("Start mainOpenModular\n");
	
	while(1);
	mainOpenModular();

	snd_pcm_close(handle);
	return 0;
}

void editor(void) {
	pthread_create(&patchconnector, NULL, connect, NULL);
	return;
}
