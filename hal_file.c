// OpenModular
// HAL.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

pthread_t patchconnector;
int MIDIin_d;
int MIDIout_d;

int MIDIinit(void) {
	return 0;
}

int MIDIin(unsigned char *data) {
	return 0;
}


void MIDIout(unsigned char outbuf) {

	if (outbuf&0x80) printf("\n");
	printf("%02X ",outbuf);
	
	return;
}

int AudioFIFOfull(void) {
	return 0;
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
    
	if (c==SAMPLERATE*60) {
		fclose(f);
		printf("Closed file OpenModularAudio.bin\n");
	}
	
	return;
}

void editor(void) {
	pthread_create(&patchconnector, NULL, patchtexteditor, NULL);
	return;
}

int main(void) {
	mainOpenModular();
	return 0;
}
