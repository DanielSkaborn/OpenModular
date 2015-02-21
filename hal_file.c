// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#define SAMPLERATE		96000
#define SAMPLERATEF		96000.0f

#include <stdio.h>
int MIDIdataavail(void) { // pollcheck of MIDI input
		return 0;
}
unsigned char MIDIrcv(void) {
	return 0;
}
void MIDIout(unsigned char outbuf) {
	unsigned char temp;
	temp = outbuf;
	if (temp&0x80) printf("\n");
	printf("%02X ",outbuf);
	return;
}

int AudioFIFOfull(void) {
	return 0;
}
void busspy(int bus) {
	printf("%03d:%03f ",bus,patchBus[bus][togglerOut]);
}

void AudioOut(void) {
	static int c=0;
		static FILE* f=NULL;
	
	int16_t temp;
	
	if (f==NULL) {
		f = fopen("OpenModularAudio.bin", "wb");
		fseek(f, 0, SEEK_SET);
		printf("Writing file\n");
	}
	
	c++;
	temp = (int16_t)(patchBus[0][togglerOut]*32767);
    fwrite(&temp, sizeof(temp), 1, f);
    temp = (int16_t)(patchBus[1][togglerOut]*32767);
    fwrite(&temp, sizeof(temp), 1, f);
    
	if (c==SAMPLERATE*30) {
		fclose(f);
		printf("Closed file OpenModularAudio.bin\n");
	}
		
	return;
}

int main(void) {
	mainOpenModular();
}
