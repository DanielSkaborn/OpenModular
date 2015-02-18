// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#include <stdio.h>
int MIDIdataavail(void) { // pollcheck of MIDI input
		return 0;
}
unsigned char MIDIrcv(void) {
	return 0;
}
void MIDIout(unsigned char outbuf) {
	return;
}

int AudioFIFOfull(void) {
	return 0;
}
void busspy(int bus) {
	printf("%03d:%03d ",bus,ctrlPatchBus[bus]);
}

void AudioOut(void) {
	//busspy(144);
	printf("%f\t%f\n", audioPatchBus[0], audioPatchBus[1]);
	return;
}

int main(void) {
	mainOpenModular();
}
