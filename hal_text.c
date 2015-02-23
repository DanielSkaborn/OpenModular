// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#include <stdio.h>

#define MIDIDEVICE		"/dev/snd/midiC1D0"
int MIDIin_d;
int MIDIout_d;


int MIDIinit(void) {
	MIDIin_d  = open(MIDIDEVICE,O_RDONLY, 0);
	MIDIout_d = open(MIDIDEVICE,O_WRONLY,0);
}

unsigned char globalmididata;
int MIDIdataavail(void) { // pollcheck of MIDI input
	
	return read(MIDIin_d, &globmididata, 1 );
//	return 0;
}
unsigned char MIDIrcv(void) {
	return globmididata;
}
int MIDIin(unsigned char &data) {
//	int ret;
	
	return read(MIDIin_d, &data, 1 );
//	if (ret) printf("%02x ",data);
//	return ret;
}


void MIDIout(unsigned char outbuf) {
	unsigned char temp;
	
	write(wd, &outbuf, 1);
	
	if (outbuf&0x80) printf("\n");
	printf("%02X ",outbuf);
	return;
}

int AudioFIFOfull(void) {
	return 0;
}
void busspy(int bus) {
	printf("%03d:%03d ",bus,ctrlPatchBus[bus]);
}

void AudioOut(void) {
	busspy(144);
	printf("%03d-%03d %03d-%03d  ",note[0],gate[0],note[1],gate[1]);
	printf("%f\t%f\n", audioPatchBus[0], audioPatchBus[1]);
	return;
}

int main(void) {
	mainOpenModular();
}
