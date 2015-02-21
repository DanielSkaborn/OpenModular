// OpenModular
// HAL_RPi.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include "rpi-systimer.c"

#define SAMPLERATE		44100
#define SAMPLERATEF		44100.0

#define GPIO_BASE       0x20200000UL

#define GPIO_GPFSEL0    0
#define GPIO_GPFSEL1    1
#define GPIO_GPFSEL2    2
#define GPIO_GPFSEL3    3
#define GPIO_GPFSEL4    4
#define GPIO_GPFSEL5    5

#define GPIO_GPSET0     7
#define GPIO_GPSET1     8

#define GPIO_GPCLR0     10
#define GPIO_GPCLR1     11

#define GPIO_GPLEV0     13
#define GPIO_GPLEV1     14

#define GPIO_GPEDS0     16
#define GPIO_GPEDS1     17

#define GPIO_GPREN0     19
#define GPIO_GPREN1     20

#define GPIO_GPFEN0     22
#define GPIO_GPFEN1     23

#define GPIO_GPHEN0     25
#define GPIO_GPHEN1     26

#define GPIO_GPLEN0     28
#define GPIO_GPLEN1     29

#define GPIO_GPAREN0    31
#define GPIO_GPAREN1    32

#define GPIO_GPAFEN0    34
#define GPIO_GPAFEN1    35

#define GPIO_GPPUD      37
#define GPIO_GPPUDCLK0  38
#define GPIO_GPPUDCLK1  39

#define GPIO_FSEL0_ALT0	0x4
#define GPIO_FSEL5_ALT0	0x20000

#define UART0_DR		0x00
#define UART0_RSRECR	0x04
#define UART0_FR		0x18
#define UART0_ILPR		0x20
#define UART0_IBRD		0x24
#define UART0_FBRD		0x28
#define UART0_LCRH		0x2C
#define UART0_CR		0x30
#define UART0_IFLS		0x34
#define UART0_IMSC		0x38
#define UART0_RIS		0x3C
#define UART0_MIS		0x40
#define UART0_ICR 		0x44
#define UART0_DMACR 	0x48
#define UART0_ITCR 		0x80
#define UART0_ITIP 		0x84
#define UART0_ITOP 		0x88
#define UART0_TDR 		0x8C
#define UART0_BASE		0x20201000

#define PWM_BASE       	0x2020C000UL

#define PWM_CTL        	0x0>>2
#define PWM_RNG1       	0x10>>2
#define PWM_RNG2       	0x20>>2
#define PWM_USEF1      	0x20
#define PWM_PWEN1      	0x1
#define PWM_USEF2      	0x2000
#define PWM_PWEN2      	0x100
#define PWM_CLRF1      	0x40
#define PWM_FIF1       	0x18>>2
#define PWM_STA        	0x4>>2
#define PWM_FULL1      	0x1

#define CM_BASE			0x20101000UL

#define CM_PWMCTL		0x0A0>>2
#define CM_PWMDIV		0x0A4>>2
#define CM_PASSWORD		0x5A000000
#define CM_ENAB			0x10
#define CM_SRC_PLLCPER	0x05
#define CM_SRC_OSCILLATOR	0x01

/** GPIO Register set */
volatile unsigned int* gpioR;
/** CM Register set */
volatile unsigned int* cmR;
/** PWM Register set */
volatile unsigned int* pwmR;
/** UART Register set */
volatile unsigned int* uartR;



int MIDIdataavail(void) { // pollcheck of MIDI input
    if((uartR[UART0_FR]&0x10)==0)
		return 1;
	else
		return 0;
}
unsigned char MIDIrcv(void) {
	return uartR[UART0_DR];
}
void MIDIout(unsigned char outbuf) {
	
	return;
}

int AudioFIFOfull(void) {
	return (pwmR[PWM_STA]&PWM_FULL1);
}
void loadLED(int s) {
	if (s) {
		// Set the GPIO16 output low ( Turn OK LED on )
        gpioR[GPIO_GPCLR0] = (1 << 16);
    } else {
		// Set the GPIO16 output high ( Turn OK LED off )
        gpioR[GPIO_GPSET0] = (1 << 16);
	}
	return;
}

void AudioOut(void) {
	volatile static int c;
	
	int out;
	out = (int)(audioPatchBus[0]*0x800)+0x800;
	pwmR[PWM_FIF1] = out;
	out = (int)(audioPatchBus[1]*0x800)+0x800;
	pwmR[PWM_FIF1] = out;

	c++;
	if (c==220) {
		// Set the GPIO16 output low ( Turn OK LED on )
        gpioR[GPIO_GPCLR0] = (1 << 16);
	}
	if (c>440) {
		c=0;
		// Set the GPIO16 output high ( Turn OK LED off )
        gpioR[GPIO_GPSET0] = (1 << 16);
	}

	return;
}

int main(void) {
//	asm volatile("ldr sp, =(128 * (1024 * 1024))\n");
	asm volatile("mov sp, #0x8000\n");                 
	asm volatile("mrc p15, 0, r0, c1, c0, 2\n");
	asm volatile("orr r0, r0, #0x300000\n");             // single precision 
	asm volatile("orr r0, r0, #0xC00000\n");             // double precision 
	asm volatile("mcr p15, 0, r0, c1, c0, 2\n");
	asm volatile("mov r0, #0x40000000\n");               // Enable FPU 
	asm volatile("fmxr fpexc,r0\n");
	

	/* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    gpioR	= (unsigned int*)GPIO_BASE;
    pwmR	= (unsigned int*)PWM_BASE;
    cmR		= (unsigned int*)CM_BASE;
	uartR	= (unsigned int*)UART0_BASE;
    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpioR[GPIO_GPFSEL1] |= (1 << 18);
	
	// Set GPIO 40 & 45 (Phone Jack) To Alternate PWM Function 0
	gpioR[GPIO_GPFSEL4] = (GPIO_FSEL0_ALT0 + GPIO_FSEL5_ALT0);

	// Set Clock
	cmR[CM_PWMDIV] = (CM_PASSWORD + 0x2000 );
	cmR[CM_PWMCTL] = (CM_PASSWORD + CM_ENAB + CM_SRC_PLLCPER + CM_SRC_OSCILLATOR);

	// Set PWM Range = 12bit 44100Hz Stereo
	pwmR[PWM_RNG1] = 0x1624;
	pwmR[PWM_RNG2] = 0x1624;

	//----- Enable PWM
	pwmR[PWM_CTL] = (PWM_USEF2 + PWM_PWEN2 + PWM_USEF1 + PWM_PWEN1 + PWM_CLRF1);
	
	//----- Init UART MIDI
	// Disable UART0.
   	uartR[UART0_CR] = 0x00000000;
	// Setup the GPIO pin 14 && 15.
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	// uartR[GPPUD] = 0x00000000;
	// delay(150);
	
	
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
   	gpioR[GPIO_GPPUDCLK0] = (1 << 14) | (1 << 15);
   	RPI_WaitMicroSeconds( 150 );
	// Write 0 to GPPUDCLK0 to make it take effect.
   	uartR[GPIO_GPPUDCLK0] = 0x00000000;
    // Clear pending interrupts.
   	uartR[UART0_ICR] = 0x7FF;
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 31250.
	// Divider = 3000000 / (16 * 31250) = 6
	// Fractional part register = 0
   	uartR[UART0_IBRD] = 6;
   	uartR[UART0_FBRD] = 0;
    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
   	uartR[UART0_LCRH] = (1 << 4) | (1 << 5) | (1 << 6);
    // Mask all interrupts.
   	uartR[UART0_IMSC] = (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10);
    // Enable UART0, receive & transfer part of UART.
    uartR[UART0_CR] = (1 << 0) | (1 << 8) | (1 << 9);

    // Set the GPIO16 output low ( Turn OK LED on )
    gpioR[GPIO_GPCLR0] = (1 << 16);

	mainOpenModular();
	return 0;
}
