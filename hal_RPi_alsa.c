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
#include <termios.h>
#include <sys/mman.h>
#include <errno.h>

#define UART_BASE   0x20201000
#define BLOCK_SIZE   4096

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0f

#define MIDIDEVICE		"/dev/ttyAMA0"
//"/dev/snd/midiC1D0"
#define ALSADEVICE      "plughw:0,1"

//int MIDIin_d;
//int MIDIout_d;

int MIDI_fd;
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

void MIDI_RPi_init(void) { // this opens the UART and tweeks the baudrate to MIDI's 31250
	int mem_fd = 0;
    int errsv = 0;

    volatile unsigned* uart = NULL;
    void* uart_map = NULL;
    void* IBRD = NULL;
    void* FBRD = NULL;
    void* LCRH = NULL;
    void* UARTCR = NULL;
    void* UARTFR = NULL;

    uint32_t brd = 0;

    struct termios termios_p;
       
    MIDI_fd = open( MIDIDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK );

    if( MIDI_fd > 0 ) // set the baud rate and other configurations
    {
       tcgetattr( MIDI_fd, &termios_p );

       termios_p.c_lflag &= ~ISIG;      // no signals
       termios_p.c_lflag &= ~ICANON;   // no canonical mode
       termios_p.c_lflag &= ~ECHO;   // no echo input
       termios_p.c_lflag &= ~NOFLSH;   // no flushing on SIGINT
       termios_p.c_lflag &= ~IEXTEN;   // no input processing
       
       termios_p.c_cc[VMIN] = 0;
       termios_p.c_cc[VTIME] = 0;
          
       tcsetattr( MIDI_fd, TCSANOW, &termios_p );

       // Now set the uart baud rate divisors
       if( ( mem_fd = open( "/dev/mem", O_RDWR | O_SYNC ) ) < 0 )
       {
          fprintf( stdout, "can't open /dev/mem for mmap(). Did you use 'sudo' ?\n" );
          return;
       }

       uart_map = mmap( NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, UART_BASE );

       close( mem_fd );

       if( uart_map == MAP_FAILED )
       {
          errsv = errno;
          fprintf( stdout,"uart mmap failed\n" );
          fprintf( stdout, "error %d %s\n", errsv, strerror( errsv ) );
       }
       else
       {
          uart = (volatile unsigned*)uart_map;
          UARTFR = (void*)( (uint32_t)uart + 0x18 );
          IBRD = (void*)( (uint32_t)uart + 0x24 );
          FBRD = (void*)( (uint32_t)uart + 0x28 );
          LCRH = (void*)( (uint32_t)uart + 0x2C );
          UARTCR = (void*)( (uint32_t)uart + 0x30 );

          // first, disable the uart and flush the fifos
          brd = 0x0B00;
          memcpy( UARTCR, &brd, 4 );

          // check for completion of any tx/rx
          memcpy( &brd, UARTFR, 4 );
          while( brd & 0x08 )
          {
             usleep( 1 ); // wait a bit
             memcpy( &brd, UARTFR, 4 );
          }

          brd = 6; // 3000000 / ( 16 * 31250 ) = 6.0
          memcpy( IBRD, &brd, 4 );
          brd = 0;
          memcpy( FBRD, &brd, 4 );
          brd = 0x70; // 8 bit data, FIFO enabled
          memcpy( LCRH, &brd, 4 );

          // enable uart, tex, rex etc.
          brd = 0x0B01;
          memcpy( UARTCR, &brd, 4 );

          // close mmap once done here
          munmap( uart_map, BLOCK_SIZE );
       }
    }
    else
    {   
       errsv = errno;
       fprintf( stdout,"%s access failed\n", MIDIDEVICE );
       fprintf( stdout, "error %d %s\n", errsv, strerror( errsv ) );
       MIDI_fd = 0;
    }
}


int MIDIin(unsigned char *data) {
	return read(MIDI_fd, data, 1 );
}

void MIDIout(unsigned char outbuf) {
	write(MIDI_fd, &outbuf, 1);
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
// 	MIDIin_d  = open(MIDIDEVICE,O_RDONLY);
//	MIDIout_d = open(MIDIDEVICE,O_WRONLY);
//	if (MIDIin_d==-1) { printf("could not open MIDI in %s\n", MIDIDEVICE); exit(0); }
//	if (MIDIout_d==-1) { printf("could not open MIDI out %s\n", MIDIDEVICE); exit(0); }

//	flags = fcntl(MIDIin_d, F_GETFL, 0);
//	fcntl(MIDIin_d, F_SETFL, flags | O_NONBLOCK);

	// Init the RPi serial port for MIDI. Tweeks the baudbase...
	MIDI_RPi_init();

 	// Init ALSA sound device

 	if ( snd_pcm_open(&handle, ALSADEVICE, SND_PCM_STREAM_PLAYBACK, 0)<0 ) {
		exit(0);
	}
	snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, SAMPLERATE, 0, 20000 );

	mainOpenModular();

	snd_pcm_close(handle);
	return 0;
}

void editor(void) {
	pthread_create(&patchconnector, NULL, patchtexteditor, NULL);
	return;
}

