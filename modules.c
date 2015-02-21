// OpenModular
// modules.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn




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

#define GATE	gate[patchGate[id]]
#define NOTE	note[patchNote[id]]

void copymodstrings(int id, char* name, char* CtrlInNames, char* CtrlOutNames, char* AudInNames, char* AudOutNames){
	int i;
	
	for (i=0;i<4*MAXCTRLIN+1;i++)
		modCtrlInsName[id][i] = CtrlInNames[i];
	for (i=0;i<4*MAXCTRLOUT+1;i++)
		modCtrlOutsName[id][i] = CtrlOutNames[i];
	for (i=0;i<4*MAXAUDIIN+1;i++)
		modAudInsName[id][i] = AudInNames[i];
	for (i=0;i<4*MAXAUDIOUT+1;i++)
		modAudOutsName[id][i] = AudOutNames[i];
	for (i=0;i<9;i++)
		modName[id][i] = name[i];

	return;
}

// an amplifier and peak follower
void module_Gain(int id) { 
	volatile static char peak=0;
	volatile char newpeak=0;
	
	AOUT0 = AIN0 * CIN0/256.0;
	AOUT1 = AIN1 * CIN0/256.0;
	
	newpeak=(unsigned char)(AOUT0*127);
	if (peak<newpeak)
		COUT0 = newpeak;
		
	return;
}
void regModule_Gain(int id) {
	moduleRegistry[id] = module_Gain;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "GAIN                                                            \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "PEAK                                                            \0";
	char AudInNames[4*MAXAUDIIN+1]    = "IN1 IN2         \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "OUT1OUT2        \0";
//               "        \0";
	char name[9]="GAIN    \0";
	
	modCtrlIns[id]     = 1;
	modCtrlOuts[id]    = 1;
	modAudIns[id]      = 2;
	modAudOuts[id]     = 2;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_ADSR(int id) {
	// CIN0  : Atack
	// CIN1  : Decay
	// CIN2  : Sustain
	// CIN3  : Release
	// COUT0 : ADSRcontrol
	// GATE  : Trig on gate #
	
	volatile static int state=0;
	volatile static unsigned char lastgate=0;
	volatile static float d=0, signal=0, sustain=0;

	// Gate signal trigging
	if ( (lastgate==0) && (GATE!=0) ) {
		state=1; // Attack
	}
	if ((lastgate!=0) && (GATE==0)) {
		state=6; // Release
	}
	
	switch (state) {
		case 0: // Idle
			signal = 0.0f;
			break;
		case 1: // Attack calculate
			d=255.0f/(SAMPLERATEF/255.0f * (float)(CIN0+1));
//			signal = 0.0f;
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
			signal = 0.0f;
			state=0;
			break;
	} //switch
	lastgate = GATE;
	COUT0=(unsigned char)(signal);
	return;
}
void regModule_ADSR(int id) {
	moduleRegistry[id] = module_ADSR;

//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "ATT DEC SUS REL                                                 \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "ADSR                                                            \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "                \0";
//               "        \0";
	char name[9]="ADSR    \0";
	
	modCtrlIns[id]     = 4;
	modCtrlOuts[id]    = 1;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 0;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_LFO(int id) {
	// CIN0	 : 1/rate
	// CIN1  : max

	volatile static int counter=0;
	volatile static int dir=0;
	
	counter++;
	if (counter>CIN0*50) {
		if (dir) COUT0++; else COUT0--;
		counter=0;
	}
	if (COUT0 == CIN1)
		dir = 0;
	if (COUT0 == 0)
		dir = 1;
	return;
}
void regModule_LFO(int id) {
	moduleRegistry[id] = module_LFO;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "RATEMAX                                                         \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "TRI                                                             \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "                \0";
//               "        \0";
	char name[9]="LFOTRI  \0";
	
	modCtrlIns[id]     = 2;
	modCtrlOuts[id]    = 1;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 0;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_Smoothie(int id) {
	// Smoothie makes controller changes smooth, and it also interleaves
	// the changes, useful to limit CPU load of filter constants calculations
	// CIN0
	//  ...
	// CIN9
	// COUT0
	//  ...
	// COUT9


	volatile static int counter=0;
	
	counter++;
	if (counter>100) counter=0;
	switch (counter) {
		case 1:
			if ( CIN0 > COUT0 ) COUT0++;
			if ( CIN0 < COUT0 ) COUT0--;
			break;
		case 2:
			if ( CIN1 > COUT1 ) COUT1++;
			if ( CIN1 < COUT1 ) COUT1--;
			break;
		case 3:
			if ( CIN2 > COUT2 ) COUT2++;
			if ( CIN2 < COUT2 ) COUT2--;
			break;
		case 4:
			if ( CIN3 > COUT3 ) COUT3++;
			if ( CIN3 < COUT3 ) COUT3--;
			break;
		case 5:
			if ( CIN4 > COUT4 ) COUT4++;
			if ( CIN4 < COUT4 ) COUT4--;
			break;
		case 6:
			if ( CIN5 > COUT5 ) COUT5++;
			if ( CIN5 < COUT5 ) COUT5--;
			break;
		case 7:
			if ( CIN6 > COUT6 ) COUT6++;
			if ( CIN6 < COUT6 ) COUT6--;
			break;
		case 8:
			if ( CIN7 > COUT7 ) COUT7++;
			if ( CIN7 < COUT7 ) COUT7--;
			break;
		case 9:
			if ( CIN8 > COUT8 ) COUT8++;
			if ( CIN8 < COUT8 ) COUT8--;
			break;
		case 10:
			if ( CIN9 > COUT9 ) COUT9++;
			if ( CIN9 < COUT9 ) COUT9--;
			break;
		default:
			break;
	}
	
	return;
}

void regModule_Smoothie(int id) {
	moduleRegistry[id] = module_Smoothie;
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "IN0 IN1 IN2 IN3 IN4 IN5 IN6 IN7 IN8 IN9                         \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "OUT0OUT1OUT2OUT3OUT4OUT5OUT6OUT7OUT8OUT9                        \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "                \0";
//               "        \0";
	char name[9]="SMOOTHIE\0";
	
	modCtrlIns[id]     = 10;
	modCtrlOuts[id]    = 10;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 0;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_SampleAndHold(int id) {
	//AIN0   : Source to sample
	//COUT0  : SampleAndHold output
	
	volatile static int counter=0;
//	volatile unsigned char target;

	counter++;
	if (counter>SAMPLERATE/2) {
		COUT0 = (AIN0+1.0) * 128;
		counter = 0;
	}

	return;
}
void regModule_SampleAndHold(int id) {
	moduleRegistry[id] = module_SampleAndHold;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "                                                                \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "S&H                                                             \0";
	char AudInNames[4*MAXAUDIIN+1]    = "IN              \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "                \0";
//               "        \0";
	char name[9]="S&H     \0";
	
	modCtrlIns[id]     = 0;
	modCtrlOuts[id]    = 1;
	modAudIns[id]      = 1;
	modAudOuts[id]     = 0;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

/*
void moduleMoogFilter(int id) {
	// AIN0  : INPUT
	// AOUT0 : OUTPUT
	// CIN0	 : CUTOFF FREQUENCY
	// CIN1  : RESONANCE

//fs = sampling frequency //(e.g. 44100Hz)

	static unsigned char lastcin0=0, lastcin1=0;

	static float cutoff=0; // Hz
	static float res=0; // [0 - 1] //(minimum - maximum)
	static float f=0, k=0, p=0, scale=0, r=0;
	static float y1=0, y2=0, y3=0, y4=0, oldy1=0, oldy2=0, oldy3=0;

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

void module_Sequencer(int id) {
	volatile static int step = 0;
	volatile static int counter = -1;
	
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
				note[1]=49;
				break;
			case 2:
				note[0]=47;
				note[1]=42;
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
				note[1]=49;
				gate[1]=100;
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
void regModule_Sequencer(int id) {
	moduleRegistry[id] = module_Sequencer;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "                                                                \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "                                                                \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "                \0";
//               "        \0";
	char name[9]="SEQ     \0";
	
	modCtrlIns[id]     = 0;
	modCtrlOuts[id]    = 0;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 0;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_Filter1(int id) {
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

	volatile static unsigned char lastcin0 = 0;
	volatile static unsigned char lastcin1 = 0;
	
	volatile static float frequency=0, resonance=0;
	volatile static float freqinhz=0;
	volatile static float f=0, p=0, q=0;             //filter coefficients
	volatile static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	volatile static float t1=0, t2=0;              //temporary buffers
	
	// Set coefficients given frequency & resonance [0.0...1.0]
	if ((CIN0 != lastcin0) || (CIN1 != lastcin1)) {
		lastcin0 = CIN0; lastcin1 = CIN1;
		freqinhz = (float)(CIN0) * 3.90625;	// 0 to 1000Hz
		frequency = freqinhz * ( 1.0 / ( SAMPLERATEF/2.0) );
		resonance = (float)(CIN1) / 256.0;	// 0 to 1

		q = 1.0 - frequency;
		p = frequency + 0.8 * frequency * q;
		f = p + p - 1.0;
		q = resonance * (1.0 + 0.5 * q * (1.0 - q + 5.6 * q * q));
	}

	in = AIN0;
	in -= q * b4;                          //feedback
	t1 = b1;  b1 = (in + b0) * p - b1 * f;
	t2 = b2;  b2 = (b1 + t1) * p - b2 * f;
	t1 = b3;  b3 = (b2 + t2) * p - b3 * f;
	b4 = (b3 + t1) * p - b4 * f;
	b4 = b4 - b4 * b4 * b4 * 0.166667;    //clipping
	b0 = in;
  
	AOUT0 = b4;
	AOUT1 = in - b4;
	AOUT2 = 3.0 * (b3 - b4);
	
	return;
}
void regModule_Filter1(int id) {
	moduleRegistry[id] = module_Filter1;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "CF  RES                                                         \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "                                                                \0";
	char AudInNames[4*MAXAUDIIN+1]    = "IN              \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "LP  HP  BP      \0";
//               "        \0";
	char name[9]="FILTER1 \0";
	
	modCtrlIns[id]     = 2;
	modCtrlOuts[id]    = 0;
	modAudIns[id]      = 1;
	modAudOuts[id]     = 3;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}


void module_Filter2(int id) {
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
	
	volatile static float frequency=0, resonance=0;
	volatile static float freqinhz=0;
	volatile static float f=0, p=0, q=0;             //filter coefficients
	volatile static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	volatile static float t1=0, t2=0;              //temporary buffers
	

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
void regModule_Filter2(int id) {
	moduleRegistry[id] = module_Filter2;

//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "CF  RES                                                         \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "                                                                \0";
	char AudInNames[4*MAXAUDIIN+1]    = "IN              \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "LP  HP  BP      \0";
//               "        \0";
	char name[9]="FILTER2 \0";
	
	modCtrlIns[id]     = 2;
	modCtrlOuts[id]    = 0;
	modAudIns[id]      = 1;
	modAudOuts[id]     = 3;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_Oscilator1(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// CIN0	 : PULSEWIDTH
	// CIN1  : FINETUNE

	volatile static float freq=0, accfreq=0;
	volatile static float d=0;
	volatile static float pw=0;
	volatile static int basictone=0, octave=0;
	volatile static unsigned char lastnote=0;
	volatile int i=0;
	
	if (NOTE!=lastnote) {
		basictone	= NOTE%12; // attach this oscillator to the decoded voice
		octave		= NOTE/12;
		lastnote=NOTE;
	
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
	if (AOUT0 > pw)
		AOUT1 = 1.0;
	else
		AOUT1 = -1.0;
/*		
	if (gate[0]==0) {
		AOUT0=0;
		AOUT1=1;
	}
*/
	return;
}
void regModule_Oscilator1(int id) {

	moduleRegistry[id] = module_Oscilator1;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "PW  TUNE                                                        \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "                                                                \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "SAW SQR         \0";
	char name[9]="OSC1    \0";

	
	modCtrlIns[id]     = 2;
	modCtrlOuts[id]    = 0;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 2;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;
}

void module_Oscilator2(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// CIN0	 : PULSEWIDTH
	// CIN1  : FINETUNE

	volatile static float freq=0, accfreq=0;
	volatile static float d=0;
	volatile static float pw=0;
	volatile static int basictone=0, octave=0;
	volatile static unsigned char lastnote=0;
	volatile int i=0;
	
	if (note[1]!=lastnote) {
		basictone	= NOTE%12; // attach this oscillator to the decoded voice
		octave		= NOTE/12;
	
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

	return;
}
void regModule_Oscilator2(int id) {
	moduleRegistry[id] = module_Oscilator2;

//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char CtrlInNames[4*MAXCTRLIN+1]   = "PW  TUNE                                                        \0";
	char CtrlOutNames[4*MAXCTRLOUT+1] = "                                                                \0";
	char AudInNames[4*MAXAUDIIN+1]    = "                \0"; 	
	char AudOutNames[4*MAXAUDIOUT+1]  = "SAW SQR         \0";
	char name[9]="OSC2    \0";

	
	modCtrlIns[id]     = 2;
	modCtrlOuts[id]    = 0;
	modAudIns[id]      = 0;
	modAudOuts[id]     = 2;
	
	copymodstrings(id, name, CtrlInNames, CtrlOutNames, AudInNames, AudOutNames);
	return;

}

void presetPatches(unsigned char prg) {
	if (prg==0) {
		
		//osc1
		patchAudioOut[0][0] = 10;
		patchAudioOut[0][1] = 11;
		patchCtrlIn[0][0]   = 133;
		patchCtrlIn[0][1]   = 170;
		patchNote[0]        = 1;
		
		//osc2
		patchAudioOut[7][0] = 20;
		patchAudioOut[7][1] = 21;
		patchCtrlIn[7][0]   = 133;
		patchCtrlIn[7][1]   = 160;
		patchNote[7]        = 0;
		
		//s&h
		patchAudioIn[3][0]  = 10;
		patchCtrlOut[3][0]  = 132;
		
		//smoothie
		patchCtrlIn[2][0]   = 132;
		patchCtrlIn[2][5]   = 132;
		patchCtrlOut[2][0]  = 133;
		patchCtrlOut[2][5]  = 134;

		//ADSR
		patchCtrlIn[4][0]	= 140;
		patchCtrlIn[4][1]	= 141;
		patchCtrlIn[4][2]	= 142;
		patchCtrlIn[4][3]	= 143;
		patchCtrlOut[4][0]	= 144;
		patchGate[4]		= 0;

		//gain
		patchCtrlIn[5][0]	= 144;
		patchAudioIn[5][0]  = 3;
		patchAudioIn[5][1]  = 3;
		patchAudioOut[5][0] = 0;
		patchAudioOut[5][1] = 30;
		
		//filter1
		patchAudioIn[6][0]  = 10;
		patchAudioOut[6][0] = 3;
		patchAudioOut[6][1] = 11;
		patchAudioOut[6][2] = 12;
		patchCtrlIn[6][0]   = 133;
		patchCtrlIn[6][1]   = 165;

		//filter2
		patchAudioIn[8][0]  = 20;
		patchAudioOut[8][0] = 1;
		patchAudioOut[8][1] = 21;
		patchAudioOut[8][2] = 22;
		patchCtrlIn[8][0]   = 163;
		patchCtrlIn[8][1]   = 164;

		//lfo
		patchCtrlIn[9][0]	= 161;
		patchCtrlIn[9][1]	= 162;
		patchCtrlOut[9][0]  = 163;

		ctrlPatchBus[140]	= 30; 
		ctrlPatchBus[141]	= 100; 
		ctrlPatchBus[142]	= 50;
		ctrlPatchBus[143]	= 100;
		
		ctrlPatchBus[170]	= 120;
		ctrlPatchBus[160]	= 128;
		ctrlPatchBus[161]	= 70;
		ctrlPatchBus[162]	= 200;
		ctrlPatchBus[164]	= 180;
		ctrlPatchBus[165]	= 10;
	}
	return;
}


// Registration of the modules to the OpenModular
// This function is called by the OpenModular
void moduleRegistration(void) {
	regModule_Oscilator1(0);
	regModule_Sequencer(1);
	regModule_Smoothie(2);
	regModule_SampleAndHold(3);
	regModule_ADSR(4);
	regModule_Gain(5);
	regModule_Filter1(6);
	regModule_Oscilator2(7);
	regModule_Filter2(8);
	regModule_LFO(9);
	
//	regModule_Gain(0);
//	regModule_ADSR(1);
//	
//	regModule_Filter1(4);
//	regModule_Filter2(5);
//	regModule_LFO(6);
//	regModule_SampleAndHold(7);
//	regModule_Sequencer(8);

	numberOfModules=10;

	return;
}
