// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include "/usr/include/alsa/asoundlib.h"
#include "/usr/include/alsa/pcm.h"
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#define MIDIDEVICE		"/dev/snd/midiC1D0"
#define ALSADEVICE      	"plughw:0,0"

int MIDIin_d;
int MIDIout_d;
int AUDIOout_d;

pthread_t patchconnector;

snd_output_t *output = NULL;
snd_pcm_sframes_t frames;

int16_t audiobuffer1[256]; 
int16_t audiobuffer2[256]; 
volatile unsigned char activebuffer = 0;
volatile unsigned char bufferfull = 0;
snd_pcm_t *handle;

unsigned char globalmididata;

int MIDIin(unsigned char *data) {
	return read(MIDIin_d, data, 1 );
}

void MIDIout(unsigned char outbuf) {
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
			while ( (frames = snd_pcm_writei(handle, audiobuffer1, sizeof(audiobuffer1)/4)) == EAGAIN)
				;
		}
		else {
			while ( (frames = (snd_pcm_writei(handle, audiobuffer2, sizeof(audiobuffer2)/4))) == EAGAIN)
				;
		}
		activebuffer = ~activebuffer;
	}
	return;
}


int main(void)
{
	int flags;

	printf("ALSA Hardware abstration layer\n  MIDI:  ");
	printf(MIDIDEVICE);
	printf("\n  Audio: ");
	printf(ALSADEVICE);
	printf("\n");

	// Init MIDI interface
 	MIDIin_d  = open(MIDIDEVICE,O_RDONLY);
	MIDIout_d = open(MIDIDEVICE,O_WRONLY);
	if (MIDIin_d==-1) { printf("could not open MIDI in %s\n", MIDIDEVICE); exit(0); }
	if (MIDIout_d==-1) { printf("could not open MIDI out %s\n", MIDIDEVICE); exit(0); }

	flags = fcntl(MIDIin_d, F_GETFL, 0);
	fcntl(MIDIin_d, F_SETFL, flags | O_NONBLOCK);

    // Init ALSA sound device

 	if ( snd_pcm_open(&handle, ALSADEVICE, SND_PCM_STREAM_PLAYBACK, 0)<0 ) {
		printf("PCM stream playback init failed!\n");
		exit(0);
	}
	snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SAMPLERATE, 0, 20000 );

	mainOpenModular();

	snd_pcm_close(handle);

	printf("ALSI init done\n");
	return 0;
}

void editor(void) {
	pthread_create(&patchconnector, NULL, patchtexteditor, NULL);
	return;
}

