// OpenModular
// moduleExample.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2


//#include "OpenModularVars.h"


#define e		2.718281828459045

// To simplify and ease reading, define som macros of the signals this module use
#define CIN0	ctrlPatchBus[patchCtrlIn[id][0]]
#define CIN1	ctrlPatchBus[patchCtrlIn[id][1]]
#define CIN2	ctrlPatchBus[patchCtrlIn[id][2]]
#define CIN3	ctrlPatchBus[patchCtrlIn[id][3]]
#define CIN4	ctrlPatchBus[patchCtrlIn[id][4]]
#define CIN5	ctrlPatchBus[patchCtrlIn[id][5]]
#define CIN6	ctrlPatchBus[patchCtrlIn[id][6]]
#define CIN7	ctrlPatchBus[patchCtrlIn[id][7]]
#define CIN8	ctrlPatchBus[patchCtrlIn[id][8]]
#define CIN9	ctrlPatchBus[patchCtrlIn[id][9]]
#define CIN10	ctrlPatchBus[patchCtrlIn[id][10]]
#define CIN11	ctrlPatchBus[patchCtrlIn[id][11]]
#define CIN12	ctrlPatchBus[patchCtrlIn[id][12]]
#define CIN13	ctrlPatchBus[patchCtrlIn[id][13]]
#define CIN14	ctrlPatchBus[patchCtrlIn[id][14]]
#define CIN15	ctrlPatchBus[patchCtrlIn[id][15]]

#define COUT0	ctrlPatchBus[patchCtrlOut[id][0]]
#define COUT1	ctrlPatchBus[patchCtrlOut[id][1]]
#define COUT2	ctrlPatchBus[patchCtrlOut[id][2]]
#define COUT3	ctrlPatchBus[patchCtrlOut[id][3]]
#define COUT4	ctrlPatchBus[patchCtrlOut[id][4]]
#define COUT5	ctrlPatchBus[patchCtrlOut[id][5]]
#define COUT6	ctrlPatchBus[patchCtrlOut[id][6]]
#define COUT7	ctrlPatchBus[patchCtrlOut[id][7]]
#define COUT8	ctrlPatchBus[patchCtrlOut[id][8]]
#define COUT9	ctrlPatchBus[patchCtrlOut[id][9]]
#define COUT10	ctrlPatchBus[patchCtrlOut[id][10]]
#define COUT11	ctrlPatchBus[patchCtrlOut[id][11]]
#define COUT12	ctrlPatchBus[patchCtrlOut[id][12]]
#define COUT13	ctrlPatchBus[patchCtrlOut[id][13]]
#define COUT14	ctrlPatchBus[patchCtrlOut[id][14]]
#define COUT15	ctrlPatchBus[patchCtrlOut[id][15]]

#define AIN0	audioPatchBus[patchAudioIn[id][0]]
#define AIN1	audioPatchBus[patchAudioIn[id][1]]
#define AIN2	audioPatchBus[patchAudioIn[id][2]]
#define AIN3	audioPatchBus[patchAudioIn[id][3]]
#define AIN4	audioPatchBus[patchAudioIn[id][4]]
#define AIN5	audioPatchBus[patchAudioIn[id][5]]
#define AIN6	audioPatchBus[patchAudioIn[id][6]]
#define AIN7	audioPatchBus[patchAudioIn[id][7]]
#define AIN8	audioPatchBus[patchAudioIn[id][8]]
#define AIN9	audioPatchBus[patchAudioIn[id][9]]
#define AIN10	audioPatchBus[patchAudioIn[id][10]]
#define AIN11	audioPatchBus[patchAudioIn[id][11]]
#define AIN12	audioPatchBus[patchAudioIn[id][12]]
#define AIN13	audioPatchBus[patchAudioIn[id][13]]
#define AIN14	audioPatchBus[patchAudioIn[id][14]]
#define AIN15	audioPatchBus[patchAudioIn[id][15]]

#define AOUT0	audioPatchBus[patchAudioOut[id][0]]
#define AOUT1	audioPatchBus[patchAudioOut[id][1]]
#define AOUT2	audioPatchBus[patchAudioOut[id][2]]
#define AOUT3	audioPatchBus[patchAudioOut[id][3]]
#define AOUT4	audioPatchBus[patchAudioOut[id][4]]
#define AOUT5	audioPatchBus[patchAudioOut[id][5]]
#define AOUT6	audioPatchBus[patchAudioOut[id][6]]
#define AOUT7	audioPatchBus[patchAudioOut[id][7]]
#define AOUT8	audioPatchBus[patchAudioOut[id][8]]
#define AOUT9	audioPatchBus[patchAudioOut[id][9]]
#define AOUT10	audioPatchBus[patchAudioOut[id][10]]
#define AOUT11	audioPatchBus[patchAudioOut[id][11]]
#define AOUT12	audioPatchBus[patchAudioOut[id][12]]
#define AOUT13	audioPatchBus[patchAudioOut[id][13]]
#define AOUT14	audioPatchBus[patchAudioOut[id][14]]
#define AOUT15	audioPatchBus[patchAudioOut[id][15]]

#define GATE	patchGate[id]
#define NOTE	patchNote[id]

void moduleGain(int id) { // an amplifier and peak follower
	volatile static char peak;
	volatile char newpeak;
	
	AOUT0 = AIN0 * CIN0/127.0;
	
	newpeak=(unsigned char)(AOUT0*127);
	if (peak<newpeak)
		COUT0 = newpeak;
		
	return;
}

void moduleADSR(int id) {
	// CIN0  : Atack
	// CIN1  : Decay
	// CIN2  : Sustain
	// CIN3  : Release
	// COUT0 : ADSRcontrol
	// GATE  : Trig on gate #
	
	volatile static int counter, state;
	volatile static unsigned char lastgate;
	volatile static float d, signal, sustain;
	// Gate signal trigging
	if ((lastgate==0) && (GATE!=0)) {
		lastgate=1;
		state=1; // Attack
	}
	if ((lastgate!=0) && (GATE==0)) {
		state=6; // Release
		lastgate=GATE;
	}
	
	switch (state) {
		case 0: // Idle
			signal = 0.0f;
			break;
		case 1: // Attack calculate
			d=255.0f/(SAMPLERATEF/255.0f * (float)(CIN0));
			signal = 0.0f;
			state  = 2;
		case 2: // Attack proceed
			signal+=d;
			if (signal>255.0f) {
				signal = 255.0f;
				state = 3;
			}
			break;
		case 3: // Decay calculate
			d=255.0f/(SAMPLERATEF/255.0f * (float)(CIN1));
			sustain = (float)(CIN2);
			state = 4;
		case 4: // Decay proceed
			signal-=d;
			if (signal < sustain) {
				signal = sustain;
				state = 5;
			}
			break;
		case 5: // Sustain
			break;
		case 6: // Release
			d=255.0f/(SAMPLERATEF/255.0f * (float)(CIN3));
		case 7: // Release proceed
			signal-=d;
			if (signal<0.0f) {
				signal = 0.0f;
				state  = 0;
			}
			break;
		default:
			state=0;
			break;
	} //switch
	lastgate = GATE;
	COUT0=(unsigned char)(signal);
	//printf("%f %f ",signal, sustain);
	return;
}

void moduleGainInit(int id) {
	
	// What does this module support
/*	modCtrlIns[id]			= 1;
	modCtrlInsName[id][]	= {"GAIN\0"};
	modCtrlOuts[id] 		= 1;
	modCtrlOutsName[id][]	= {"PEAK\0"};
	modAudIns[id]			= 1;
	modAudInsName[id][]		= {"IN  \0"};
	modAudOuts[id]			= 1;
	modAudOutsName[id][]	= {"OUT \0"};*/
	// put a function pointer to the process function in the OpenModular registry
//	modProcessPtr[id]		= (void (*)(int)) ModuleExampleProcess;
}

void moduleLFO(int id) {
	// CIN0	 : 1/rate
	// CIN1  : max

	volatile static int counter;
	volatile static int dir;
	
	counter++;
	if (counter>CIN0*50) {
		if (dir) COUT0++; else COUT0--;
		counter=0;
	}
	if (COUT0 > CIN1) {
		if (dir==0) dir=1;
		else dir=0;
	}
	return;
}

void moduleSampleAndHold(int id) {
	//AIN0   : Source to sample
	//COUT0  : SampleAndHold output
	
	volatile static int counter;
	volatile unsigned char target;
	counter++;
	if (counter>SAMPLERATE/2) {
		COUT0 = (AIN0+1.0) * 128;
		counter = 0;
	}
/*
	if (COUT0 > target) {
		COUT0=COUT0-1;
	}
	if (COUT0 < target) {
		COUT0=COUT0+1;
	}
*/	
//	printf("%d %d ",target, COUT0);
	return;
}
/*
void moduleMoogFilter(int id) {
	// AIN0  : INPUT
	// AOUT0 : OUTPUT
	// CIN0	 : CUTOFF FREQUENCY
	// CIN1  : RESONANCE

//fs = sampling frequency //(e.g. 44100Hz)

	static unsigned char lastcin0, lastcin1;

	static float cutoff; // Hz
	static float res; // [0 - 1] //(minimum - maximum)
	static float f, k, p, scale, r;
	static float y1, y2, y3, y4, oldy1, oldy2, oldy3;

// parameters

	if ((CIN0 != lastcin0) || CIN1 != lastcin1)) {
		lastcin0=CIN0; lastcin1=CIN1;

		cutoff = CIN0 * 19.53;		// 0 to 5000Hz
		res = CIN1 / 256.0;	// 0 to 1
		
		f = 2 * cutoff / SAMPLERATEF; //[0 - 1]
		k = 3.6*f - 1.6*f*f -1.; 	  //(Empirical tunning)
		p = (k+1)*0.5;
		scale = pow_f(e,((1-p)*1.386249;
		r = res*scale;
	}
	
	//--Inverted feed back for corner peaking
	x = AIN0 - r*y4;

	//Four cascaded onepole filters (bilinear transform)
	y1 = x  * p + oldx  * p - k * y1;
	y2 = y1 * p + oldy1 * p - k * y2;
	y3 = y2 * p + oldy2 * p - k * y3;
	y4 = y3 * p + oldy3 * p - k * y4;

	//Clipper band limited sigmoid
	y4 = y4 - (y4*y4*y4)/6.0;

	oldx = x;
	oldy1 = y1;
	oldy2 = y2;
	oldy3 = y3;

	AOUT0 = y4; // output
	return;
}
*/

void moduleSequencer(int id) {
	volatile static int step;
	volatile static int counter;
	
	counter++;
	if (counter > SAMPLERATE/10) {
		gate[0]=0;
	}
	if (counter > SAMPLERATE/5) {
		step++;
		counter=0;
		gate[0]=126;
		
		switch(step) {
			case 0:
				note[0]=42;
				gate[1]=200;
				note[1]=47;
				break;
			case 1:
				note[0]=49;
				break;
			case 2:
				note[0]=47;
				break;
			case 3:
				note[0]=42;
				gate[1]=0;
				break;
			case 4:
				note[0]=49;
				break;
			case 5:
				note[0]=47;
				break;
			case 6:
				note[0]=42;
				break;
			case 7:
				note[0]=50;
				step=-1;
				break;
			default:
				step=-1;
				break;
		}
	}
	return;
}

void moduleFilter1(int id) {
	// Moog 24 dB/oct resonant lowpass VCF
	// References: CSound source code, Stilson/Smith CCRMA paper.
	// Modified by paul.kellett@maxim.abel.co.uk July 2000
	// Modified to OpenModular by Daniel Skaborn Feb 2015

	// AIN0  : INPUT
	// AOUT0 : LOWPASS OUTPUT
	// AOUT1 : HIGHPASS OUTPUT
	// AOUT2 : BANDPASS OUTPUT
	// CIN0	 : CUTOFF FREQUENCY
	// CIN1  : RESONANCE

	volatile static unsigned char lastcin0, lastcin1;
	
	volatile static float frequency, resonance;
	volatile static float freqinhz;
	volatile static float f, p, q;             //filter coefficients
	volatile static float in, b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
	volatile static float t1, t2;              //temporary buffers
	

	// Set coefficients given frequency & resonance [0.0...1.0]
	if ((CIN0 != lastcin0) || (CIN1 != lastcin1)) {
		lastcin0=CIN0; lastcin1=CIN1;
		freqinhz = CIN0 * 3.90625;	// 0 to 1000Hz
		frequency = freqinhz * (1.f/( SAMPLERATEF/2.0f));
		resonance = CIN1 / 256.0;	// 0 to 1
				
		q = 1.0f - frequency;
		p = frequency + 0.8f * frequency * q;
		f = p + p - 1.0f;
		q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
	}

	// Filter (in [-1.0...+1.0])
	in = AIN0;
	in -= q * b4;                          //feedback
	t1 = b1;  b1 = (in + b0) * p - b1 * f;
	t2 = b2;  b2 = (b1 + t1) * p - b2 * f;
	t1 = b3;  b3 = (b2 + t2) * p - b3 * f;
	b4 = (b3 + t1) * p - b4 * f;
	b4 = b4 - b4 * b4 * b4 * 0.166667f;    //clipping
	b0 = in;
  
	AOUT0 = b4;
	AOUT1 = in - b4;
	AOUT2 = 3.0f * (b3 - b4);
	
	return;
}

void moduleFilter2(int id) {
	// Moog 24 dB/oct resonant lowpass VCF
	// References: CSound source code, Stilson/Smith CCRMA paper.
	// Modified by paul.kellett@maxim.abel.co.uk July 2000
	// Modified to OpenModular by Daniel Skaborn Feb 2015

	// AIN0  : INPUT
	// AOUT0 : LOWPASS OUTPUT
	// AOUT1 : HIGHPASS OUTPUT
	// AOUT2 : BANDPASS OUTPUT
	// CIN0	 : CUTOFF FREQUENCY
	// CIN1  : RESONANCE

	volatile static unsigned char lastcin0, lastcin1;
	
	volatile static float frequency, resonance;
	volatile static float freqinhz;
	volatile static float f, p, q;             //filter coefficients
	volatile static float in, b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
	volatile static float t1, t2;              //temporary buffers
	

	// Set coefficients given frequency & resonance [0.0...1.0]
	if ((CIN0 != lastcin0) || (CIN1 != lastcin1)) {
		lastcin0=CIN0; lastcin1=CIN1;
		freqinhz = CIN0 * 3.90625;	// 0 to 1000Hz
		frequency = freqinhz * (1.f/( SAMPLERATEF/2.0f));
		resonance = CIN1 / 256.0;	// 0 to 1
				
		q = 1.0f - frequency;
		p = frequency + 0.8f * frequency * q;
		f = p + p - 1.0f;
		q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
	}

	// Filter (in [-1.0...+1.0])
	in = AIN0;
	in -= q * b4;                          //feedback
	t1 = b1;  b1 = (in + b0) * p - b1 * f;
	t2 = b2;  b2 = (b1 + t1) * p - b2 * f;
	t1 = b3;  b3 = (b2 + t2) * p - b3 * f;
	b4 = (b3 + t1) * p - b4 * f;
	b4 = b4 - b4 * b4 * b4 * 0.166667f;    //clipping
	b0 = in;
  
	AOUT0 = b4;
	AOUT1 = in - b4;
	AOUT2 = 3.0f * (b3 - b4);
	
	return;
}

void moduleOscilator1(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// CIN0	 : PULSEWIDTH
	// CIN1  : FINETUNE

	volatile static float freq, accfreq;
	volatile static float d;
	volatile static float pw;
	volatile static int basictone, octave;
	volatile static unsigned char lastnote;
	volatile int i;
	
	if (note[0]!=lastnote) {
		basictone	= note[0]%12; // attach this oscillator to the decoded voice 0;
		octave		= note[0]/12;
	
		// lookuptable, basictone to frequency (HZ)
		switch(basictone) {
			case 0:  accfreq=261.63; break;  // C
			case 1:  accfreq=277.18; break;  // C#
			case 2:  accfreq=293.66; break;  // D
			case 3:  accfreq=311.13; break;  // D#
			case 4:  accfreq=329.63; break;  // E
			case 5:  accfreq=349.23; break;  // F
			case 6:  accfreq=369.99; break;  // F#
			case 7:  accfreq=392.00; break;  // G
			case 8:  accfreq=415.30; break;  // G#
			case 9:  accfreq=440.00; break;  // A
			case 10: accfreq=466.16; break;  // A#
			case 11: accfreq=493.88; break;  // H
			default: accfreq=1.00; break;    // default; 1Hz
		}
	
		// octave the frequency
		if (octave>4)
			for (i=0;i<octave-4;i++)
				accfreq*=2.0;
		if (octave<4)
			for (i=0;i>octave+4;i++)
				accfreq/=2;
	}
	freq = accfreq;
	
	// Apply finetune
	freq += ((CIN1-127)/5000.0);
	
	// generate a SAWUP
	d = freq / SAMPLERATEF * 2.0;	// calculate d
	
	AOUT0 += d;
	if (AOUT0 > 1.0) {
		AOUT0 -= 2.0;
	}
	
	// generate a PULSEWAVE, using the SAWUP
	pw=(CIN0/256.0*1.8)-0.9;
	if (AOUT0>pw)
		AOUT1=1.0;
	else
		AOUT1=-1.0;
/*		
	if (gate[0]==0) {
		AOUT0=0;
		AOUT1=1;
	}
*/
	return;
}


void moduleOscilator2(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// CIN0	 : PULSEWIDTH
	// CIN1  : FINETUNE

	volatile static float freq, accfreq;
	volatile static float d;
	volatile static float pw;
	volatile static int basictone, octave;
	volatile static unsigned char lastnote;
	volatile int i;
	
	if (note[1]!=lastnote) {
		basictone	= note[1]%12; // attach this oscillator to the decoded voice 0;
		octave		= note[1]/12;
	
		// lookuptable, basictone to frequency (HZ)
		switch(basictone) {
			case 0:  accfreq=261.63; break;  // C
			case 1:  accfreq=277.18; break;  // C#
			case 2:  accfreq=293.66; break;  // D
			case 3:  accfreq=311.13; break;  // D#
			case 4:  accfreq=329.63; break;  // E
			case 5:  accfreq=349.23; break;  // F
			case 6:  accfreq=369.99; break;  // F#
			case 7:  accfreq=392.00; break;  // G
			case 8:  accfreq=415.30; break;  // G#
			case 9:  accfreq=440.00; break;  // A
			case 10: accfreq=466.16; break;  // A#
			case 11: accfreq=493.88; break;  // H
			default: accfreq=1.00; break;    // default; 1Hz
		}
	
		// octave the frequency
		if (octave>4)
			for (i=0;i<octave-4;i++)
				accfreq*=2.0;
		if (octave<4)
			for (i=0;i>octave+4;i++)
				accfreq/=2;
	}
	freq = accfreq;
	
	// Apply finetune
	freq += ((CIN1-127)/5000.0);
	
	// generate a SAWUP
	d = freq / SAMPLERATEF * 2.0;	// calculate d
	
	AOUT0 += d;
	if (AOUT0 > 1.0) {
		AOUT0 -= 2.0;
	}
	
	// generate a PULSEWAVE, using the SAWUP
	pw=(CIN0/256.0*1.8)-0.9;
	if (AOUT0>pw)
		AOUT1=1.0;
	else
		AOUT1=-1.0;
/*		
	if (gate[0]==0) {
		AOUT0=0;
		AOUT1=1;
	}
*/
	return;
}

/*
void moduleTestTone(int id) {
	volatile static int counter;
	
	counter++;
	if (counter==220) {
		AOUT0=1.0;
		AOUT1=-0.250;
	}
	if (counter>439) {
		AOUT0=-1.0;
		AOUT1=0.250;
		counter=0;
	}
	
	return;
}
*/
