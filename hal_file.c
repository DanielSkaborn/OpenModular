// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn
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
	ret = read(MIDIin_d, &globalmididata, 1 );
//	if (ret==1) printf("%d\n",globalmididata);
	return 0;
}
unsigned char MIDIrcv(void) {
	return globalmididata;
}

int MIDIin(unsigned char *data) {
	return read(MIDIin_d, data, 1 );
}


void MIDIout(unsigned char outbuf) {
	unsigned char temp;
	
	write(MIDIout_d, &outbuf, 1);
/*	
	if (outbuf&0x80) printf("\n");
	printf("%02X ",outbuf);
*/	return;
}

int AudioFIFOfull(void) {
	return 0;
}
void busspy(int bus) {
	printf("%03d %03f ",bus,patchBus[bus][togglerOut]);
}

void AudioOut(void) {
	
	volatile static int c=0;
	static FILE* f=NULL;
	int16_t temp=0;
	
	if (f==NULL) {
		f = fopen("OpenModularAudio.bin", "wb");
		fseek(f, 0, SEEK_SET);
		printf("Writing file\n");
	}

	temp = (int16_t)(patchBus[0][togglerOut]*32767);
	fwrite(&temp, sizeof(temp), 1, f);
	
	temp = (int16_t)(patchBus[1][togglerOut]*32767);
	fwrite(&temp, sizeof(temp), 1, f);
    c++;
    
	if (c==SAMPLERATE*90) {
		fclose(f);
		printf("Closed file OpenModularAudio.bin\n");
	}
	
	return;
}

int main(void) {
	MIDIinit();
	mainOpenModular();
}
