// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include "/usr/include/alsa/asoundlib.h"
#include "/usr/include/alsa/pcm.h"
#include <pthread.h>

/*
#define OVERSAMPLE			4
#define SAMPLERATEOUT		44100
#define SAMPLERATEOUTF		44100.0f
*/
#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MIDIDEVICE		"/dev/snd/midiC1D0"

int MIDIin_d;
int MIDIout_d;
int AUDIOout_d;

static char *ALSAdevice = "plughw:1,0"; /* playback device */
snd_output_t *output = NULL;
snd_pcm_sframes_t frames;
snd_pcm_t *handle;

int16_t buffer[16*1024]; /* some random data */
int16_t audiobuffer1[64*1024]; 
int16_t audiobuffer2[64*1024]; 
volatile unsigned char activebuffer = 0;
volatile unsigned char bufferfull = 0;
snd_pcm_t *handle;

unsigned char globalmididata;

int MIDIinit(void) {

	int flags;
 	MIDIin_d  = open(MIDIDEVICE,O_RDONLY, 0);
	MIDIout_d = open(MIDIDEVICE,O_WRONLY,0);

	flags = fcntl(MIDIin_d, F_GETFL, 0);
	fcntl(MIDIin_d, F_SETFL, flags | O_NONBLOCK);

}

int MIDIdataavail(void) {
	int ret;
//	ret = read(MIDIin_d, &globalmididata, 1 );
//	if (ret==1) printf("%d\n",globalmididata);
	return 0;
}
unsigned char MIDIrcv(void) {
	return globalmididata;
}

int MIDIin(unsigned char *data) {
	return 0;
//	return read(MIDIin_d, data, 1 );
}

void MIDIout(unsigned char outbuf) {
	unsigned char temp;
	
//	write(MIDIout_d, &outbuf, 1);
/*	
	if (outbuf&0x80) printf("\n");
	printf("%02X ",outbuf);
*/	return;
}

int AudioFIFOfull(void) {
	return bufferfull;
}
void busspy(int bus) {
	printf("%03d %03f ",bus,patchBus[bus][togglerOut]);
}

void AudioInit(void) {

}

void AudioOut(void) {
	static int samplecount = 0;
	static int buffersamples = sizeof(audiobuffer1)/4;
	static 
	snd_pcm_sframes_t frames;

		
	switch (activebuffer) {
		case 0x0:
			audiobuffer1[samplecount]=(int16_t)(patchBus[0][togglerOut]*0x7FFF);
			samplecount++;
			audiobuffer1[samplecount]=(int16_t)(patchBus[1][togglerOut]*0x7FFF);
			samplecount++;
			break;
		case 0xFF:
			audiobuffer2[samplecount]=(int16_t)(patchBus[0][togglerOut]*0x7FFF);
			samplecount++;
			audiobuffer2[samplecount]=(int16_t)(patchBus[1][togglerOut]*0x7FFF);
			samplecount++;
			break;
		default:
			printf("buffer toggle error\n");
			activebuffer=0;
			break;
	}
	if (samplecount == buffersamples) {
		bufferfull=1;
		samplecount=0;
		if (activebuffer==0) {
			while ( frames = snd_pcm_writei(handle, audiobuffer1, sizeof(audiobuffer1)/4) == EAGAIN)
				printf("pending\n");
			if (frames<0) printf("error\n");
		}
		else {
			while ( (frames = snd_pcm_writei(handle, audiobuffer2, sizeof(audiobuffer2)/4)) == EAGAIN)
				;
			printf(".\n");
		}
		bufferfull = 0;
		activebuffer = ~activebuffer;
	}
	return;
}


int main(void)
{
	pthread_t audiotoggle;
	int err;
	unsigned int i;
	snd_pcm_sframes_t frames;

	MIDIinit();

	snd_pcm_open(&handle, ALSAdevice, SND_PCM_STREAM_PLAYBACK, SND_PCM_STREAM_PLAYBACK);//) < 0);
	snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SAMPLERATE, 0, 100000 );

    mainOpenModular();

	snd_pcm_close(handle);
	return 0;
}

