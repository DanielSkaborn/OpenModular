// OpenModular
// modules.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn




#define e		2.718281828459045

// To simplify and ease reading, define som macros of the signals this module use

#define AIN0	patchBus[patchIn[id][0]][togglerIn]
#define AIN1	patchBus[patchIn[id][1]][togglerIn]
#define AIN2	patchBus[patchIn[id][2]][togglerIn]
#define AIN3	patchBus[patchIn[id][3]][togglerIn]
#define AIN4	patchBus[patchIn[id][4]][togglerIn]
#define AIN5	patchBus[patchIn[id][5]][togglerIn]
#define AIN6	patchBus[patchIn[id][6]][togglerIn]
#define AIN7	patchBus[patchIn[id][7]][togglerIn]
#define AIN8	patchBus[patchIn[id][8]][togglerIn]
#define AIN9	patchBus[patchIn[id][9]][togglerIn]
#define AIN10	patchBus[patchIn[id][10]][togglerIn]
#define AIN11	patchBus[patchIn[id][11]][togglerIn]
#define AIN12	patchBus[patchIn[id][12]][togglerIn]
#define AIN13	patchBus[patchIn[id][13]][togglerIn]
#define AIN14	patchBus[patchIn[id][14]][togglerIn]
#define AIN15	patchBus[patchIn[id][15]][togglerIn]

#define AOUT0	patchBus[patchOut[id][0]][togglerOut]
#define AOUT1	patchBus[patchOut[id][1]][togglerOut]
#define AOUT2	patchBus[patchOut[id][2]][togglerOut]
#define AOUT3	patchBus[patchOut[id][3]][togglerOut]
#define AOUT4	patchBus[patchOut[id][4]][togglerOut]
#define AOUT5	patchBus[patchOut[id][5]][togglerOut]
#define AOUT6	patchBus[patchOut[id][6]][togglerOut]
#define AOUT7	patchBus[patchOut[id][7]][togglerOut]
#define AOUT8	patchBus[patchOut[id][8]][togglerOut]
#define AOUT9	patchBus[patchOut[id][9]][togglerOut]
#define AOUT10	patchBus[patchOut[id][10]][togglerOut]
#define AOUT11	patchBus[patchOut[id][11]][togglerOut]
#define AOUT12	patchBus[patchOut[id][12]][togglerOut]
#define AOUT13	patchBus[patchOut[id][13]][togglerOut]
#define AOUT14	patchBus[patchOut[id][14]][togglerOut]
#define AOUT15	patchBus[patchOut[id][15]][togglerOut]

#define GATE	gate[patchGate[id]]
#define NOTE	note[patchNote[id]]

#define DUMP	NOPATCHBUS-1

float notetofreqLUT(unsigned char theNote) {
	
}

void copymodstrings(int id, char* name, char* inNames, char* outNames){
	int i;
	
	for (i=0;i<4*MAXIN+1;i++)
		modInsName[id][i] = inNames[i];
	for (i=0;i<4*MAXOUT+1;i++)
		modOutsName[id][i] = outNames[i];
	for (i=0;i<9;i++)
		modName[id][i] = name[i];

	return;
}

// an amplifier and peak follower
void module_Gain(int id) { 
	volatile static char peak=0;
	volatile char newpeak=0;
	
	AOUT0 = AIN0 * AIN3;
	AOUT1 = AIN1 * AIN3;
	
/*	newpeak=(unsigned char)(AOUT0*127);
	if (peak<newpeak)
		AOUT3 = newpeak;
*/		
	return;
}
void regModule_Gain(int id) {
	moduleRegistry[id] = module_Gain;
	
//                               "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]    = "IN1 IN2 GAIN                                                    \0"; 	
	char outNames[4*MAXOUT+1]  = "OUT1OUT2                                                        \0";
//               "        \0";
	char name[9]="GAIN    \0";
	
	modIns[id]      = 3;
	modOuts[id]     = 2;
	
	copymodstrings(id, name, inNames, outNames);
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
			d=255.0f/(SAMPLERATEF/255.0f * (float)(AIN0+1));
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
			d=255.0f/(SAMPLERATEF/255.0f * (float)(AIN1));
			sustain = (float)(AIN2);
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
			d=255.0f/(SAMPLERATEF/255.0f * (float)(AIN3));
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
	AOUT0=(unsigned char)(signal);
	return;
}

void regModule_ADSR(int id) {
	moduleRegistry[id] = module_ADSR;

//                                  "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "ATT DEC SUS REL                                                 \0";
	char outNames[4*MAXOUT+1] = "ADSR                                                            \0";
//               "        \0";
	char name[9]="ADSR    \0";
	
	modIns[id]     = 4;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_LFO(int id) {
	// CIN0	 : 1/rate
	// CIN1  : max

	volatile static int counter=0;
	volatile static int countto=10;
	volatile static int dir=0;
	volatile static float accout=0.0;
	volatile static float temp0,temp1;
			
	counter++;
	if ( counter == countto ) {
//		printf("LFOstep\n");
		
		temp0=AIN0;
		temp0+=1.0;
		if (temp0<0.0) temp0=0.0;
		if (temp0>2.0) temp0=2.0;
		countto = temp0 * (SAMPLERATEF/4.0);

		temp1=AIN1;
		temp1+=1.0;
				
		if (temp1<0) temp1=0.0;
		if (temp1>2.0) temp1=2.0;
		
		if (dir) accout+=0.015; else accout-=0.015;
		counter=0;
		if (accout > temp1)
			dir = 0;
		if (accout < 0.0)
			dir = 1;
	}
		
	AOUT0=accout-1.0;
	return;
}
void regModule_LFO(int id) {
	moduleRegistry[id] = module_LFO;
	
//                                  "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "RATEMAX                                                         \0";
	char outNames[4*MAXOUT+1] = "TRI                                                             \0";
//               "        \0";
	char name[9]="LFOTRI  \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_LFO2(int id) {
	// CIN0	 : 1/rate
	// CIN1  : max

	volatile static int counter=0;
	volatile static int countto=10;
	volatile static int dir=0;
	volatile static float accout=0.0;
	volatile static float temp0,temp1;
			
	counter++;
	if ( counter == countto ) {
//		printf("LFOstep\n");
		
		temp0=AIN0;
		temp0+=1.0;
		if (temp0<0.0) temp0=0.0;
		if (temp0>2.0) temp0=2.0;
		countto = temp0 * (SAMPLERATEF/4.0);

		temp1=AIN1;
		temp1+=1.0;
				
		if (temp1<0) temp1=0.0;
		if (temp1>2.0) temp1=2.0;
		
		if (dir) accout+=0.015; else accout-=0.015;
		counter=0;
		if (accout > temp1)
			dir = 0;
		if (accout < 0.0)
			dir = 1;
	}
		
	AOUT0=accout-1.0;
	return;
}
void regModule_LFO2(int id) {
	moduleRegistry[id] = module_LFO;
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "RATEMAX                                                         \0";
	char outNames[4*MAXOUT+1] = "TRI                                                             \0";
//               "        \0";
	char name[9]="LFOTRI  \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Smoothie(int id) {
	// Smoothie makes controller changes smooth, and it also interleaves
	// the changes, useful to limit CPU load of filter constants calculations
	// AIN0
	//  ...
	// AIN9
	// AIN10 Smoothrate
	// AOUT0
	//  ...
	// AOUT9

	volatile static int counter=0;
	volatile float smooth;
	
	
	counter++;
	if (counter>50) counter=0;
	switch (counter) {
		case 0:
			smooth = AIN10;
			if ( smooth < -1.0 ) smooth =-0.9;
			if ( smooth >  1.0 ) smooth =1;
			smooth += 1.0;
			smooth /= 127.0;
			break;
		case 1:
			if ( AIN0 > AOUT0 ) AOUT0++;
			if ( AIN0 < AOUT0 ) AOUT0--;
			break;
		case 2:
			if ( AIN1 > AOUT1 ) AOUT1++;
			if ( AIN1 < AOUT1 ) AOUT1--;
			break;
		case 3:
			if ( AIN2 > AOUT2 ) AOUT2++;
			if ( AIN2 < AOUT2 ) AOUT2--;
			break;
		case 4:
			if ( AIN3 > AOUT3 ) AOUT3++;
			if ( AIN3 < AOUT3 ) AOUT3--;
			break;
		case 5:
			if ( AIN4 > AOUT4 ) AOUT4++;
			if ( AIN4 < AOUT4 ) AOUT4--;
			break;
		case 6:
			if ( AIN5 > AOUT5 ) AOUT5++;
			if ( AIN5 < AOUT5 ) AOUT5--;
			break;
		case 7:
			if ( AIN6 > AOUT6 ) AOUT6++;
			if ( AIN6 < AOUT6 ) AOUT6--;
			break;
		case 8:
			if ( AIN7 > AOUT7 ) AOUT7++;
			if ( AIN7 < AOUT7 ) AOUT7--;
			break;
		case 9:
			if ( AIN8 > AOUT8 ) AOUT8++;
			if ( AIN8 < AOUT8 ) AOUT8--;
			break;
		case 10:
			if ( AIN9 > AOUT9 ) AOUT9++;
			if ( AIN9 < AOUT9 ) AOUT9--;
			break;
		default:
			break;
	}
	
	return;
}
void regModule_Smoothie(int id) {
	moduleRegistry[id] = module_Smoothie;
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN0 IN1 IN2 IN3 IN4 IN5 IN6 IN7 IN8 IN9 SMOO                    \0";
	char outNames[4*MAXOUT+1] = "OUT0OUT1OUT2OUT3OUT4OUT5OUT6OUT7OUT8OUT9                        \0";
//               "        \0";
	char name[9]="SMOOTHIE\0";
	
	modIns[id]     = 11;
	modOuts[id]    = 10;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_SampleAndHold(int id) {
	//AIN0   : Source to sample
	//AIN2   : holdTime
	//AOUT0  : SampleAndHold output
	
	volatile static int counter=0;
	float temp;

	counter--;
	if (counter<0) {
		AOUT0 = AIN0;
		// time to next sample
		temp=AIN1;
		if (temp<-1.0) temp=-1.0;
		temp+=1.0;
		counter = (int)(SAMPLERATEF*temp); // 0 to 2s
	}

	return;
}
void regModule_SampleAndHold(int id) {
	moduleRegistry[id] = module_SampleAndHold;
	
//                                  "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN  TIME                                                        \0";
	char outNames[4*MAXOUT+1] = "S&H                                                             \0";
//               "        \0";
	char name[9]="S&H     \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

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
				note[0]=47;
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
	char inNames[4*MAXIN+1]   = "                                                                \0";
	char outNames[4*MAXOUT+1] = "                                                                \0";
//               "        \0";
	char name[9]="SEQ     \0";
	
	modIns[id]     = 0;
	modOuts[id]    = 0;
	
	copymodstrings(id, name, inNames, outNames);
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
	// AIN1	 : CUTOFF FREQUENCY
	// AIN2  : RESONANCE

	volatile static float lastcin0 = -2;
	volatile static float lastcin1 = -2;
	
	volatile static float frequency=0, resonance=0;
	volatile static float freqinhz=0;
	volatile static float f=0, p=0, q=0;             //filter coefficients
	volatile static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	volatile static float t1=0, t2=0;              //temporary buffers
	
	float temp;
	
	// Set coefficients given frequency & resonance [0.0...1.0]
	if ((AIN1 != lastcin0) || (AIN2 != lastcin1)) {
		if (AIN1<-1.0) temp=-0.99; else temp=AIN1; 
		temp+=1.0;
		lastcin0 = AIN1;
		freqinhz = 200.0f + (temp * 1000.0f);	// 200 to 2200Hz
		frequency = freqinhz * ( 1.0 / ( SAMPLERATEF/2.0) );
		if (AIN2<-1.0) temp=-0.99; else temp=AIN2; 
		lastcin1 = AIN2;
		resonance = (temp+1.0)*0.5; // 0 to 1

//		printf("F1: AIN1=%f CF=%f %fHz AIN2=%f res=%f\n",AIN1, frequency, freqinhz, AIN2, resonance);

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
	char inNames[4*MAXIN+1]   = "IN  CF  RES                                                     \0";
	char outNames[4*MAXOUT+1] = "LP  HP  BP                                                      \0";
//               "        \0";
	char name[9]="FILTER1 \0";
	
	modIns[id]     = 3;
	modOuts[id]    = 3;
	
	copymodstrings(id, name, inNames, outNames);
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
	// AIN1	 : CUTOFF FREQUENCY
	// AIN2  : RESONANCE

	volatile static float lastcin0 = -2;
	volatile static float lastcin1 = -2;
	
	volatile static float frequency=0, resonance=0;
	volatile static float freqinhz=0;
	volatile static float f=0, p=0, q=0;             //filter coefficients
	volatile static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	volatile static float t1=0, t2=0;              //temporary buffers
	
	volatile static float temp;

	// Set coefficients given frequency & resonance [0.0...1.0]
	if ((AIN1 != lastcin0) || (AIN2 != lastcin1)) {
		if (AIN1<-1.0) temp=-0.99; else temp=AIN1; 
		temp+=1.0;
		lastcin0 = AIN1;
		freqinhz = 200.0f + (temp * 1000.0);	// 200 to 2200Hz
		frequency = freqinhz * ( 1.0 / ( SAMPLERATEF/2.0) );
		if (AIN2<-1.0) temp=-0.99; else temp=AIN2; 
		lastcin1 = AIN2;
		resonance = (temp+1.0)*0.5; // 0 to 1

//		printf("F2: AIN1=%f CF=%f %fHz AIN2=%f res=%f\n",AIN1, frequency, freqinhz, AIN2, resonance);

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
void regModule_Filter2(int id) {
	moduleRegistry[id] = module_Filter2;
	
//                                      "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN  CF  RES                                                     \0";
	char outNames[4*MAXOUT+1] = "LP  HP  BP                                                      \0";
//               "        \0";
	char name[9]="FILTER2 \0";
	
	modIns[id]     = 3;
	modOuts[id]    = 3;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Oscilator1(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// AIN0	 : PULSEWIDTH
	// AIN1  : FINETUNE
	// NOTE  :

	volatile static float freq=0, toneFreq=0;
	volatile static float d=0;
	volatile static float pw=0;
	volatile static unsigned char lastnote=0;
	volatile int i=0;
	
	if (NOTE!=lastnote) {
		lastnote=NOTE;
	
		// lookuptable, basictone to frequency (HZ)
		toneFreq = noteToFreqLUT[NOTE];
	}	
		
	// Apply finetune
	freq = toneFreq + AIN1; // +/- 1Hz
	
	// generate a SAWUP
	d = freq / SAMPLERATEF * 2.0;	// calculate d
	
	AOUT0 += d;
	if (AOUT0 > 1.0) {
		AOUT0 -= 2.0;
	}
	
	// generate a PULSEWAVE, using the SAWUP
	pw = AIN0 * 0.95;
	if (AOUT0 > pw)
		AOUT1 = 1.0;
	else
		AOUT1 = -1.0;

	return;
}
void regModule_Oscilator1(int id) {

	moduleRegistry[id] = module_Oscilator1;
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "PW  TUNE                                                        \0";
	char outNames[4*MAXOUT+1] = "SAW SQR                                                         \0";
	char name[9]="OSC1    \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Oscilator2(int id) {
	// AOUT0 : SAWUP
	// AOUT1 : SQUARE PWM
	// AIN0	 : PULSEWIDTH
	// AIN1  : FINETUNE
	// NOTE  :

	volatile static float freq=0, toneFreq=0;
	volatile static float d=0;
	volatile static float pw=0;
	volatile static unsigned char lastnote=0;
	volatile int i=0;
	
	if (NOTE!=lastnote) {
		lastnote=NOTE;
	
		// lookuptable, basictone to frequency (HZ)
		toneFreq = noteToFreqLUT[NOTE];
	}	
		
	// Apply finetune
	freq = toneFreq + AIN1*5.0; // +/- 5Hz
	
	// generate a SAWUP
	d = freq / SAMPLERATEF * 2.0;	// calculate d
	
	AOUT0 += d;
	if (AOUT0 > 1.0) {
		AOUT0 -= 2.0;
	}
	
	// generate a PULSEWAVE, using the SAWUP
	pw = AIN0 * 0.95;
	if (AOUT0>pw)
		AOUT1=1.0;
	else
		AOUT1=-1.0;

	return;
}
void regModule_Oscilator2(int id) {
	moduleRegistry[id] = module_Oscilator2;

//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "PW  TUNE                                                        \0";
	char outNames[4*MAXOUT+1] = "SAW SQR                                                         \0";
	char name[9]="OSC2    \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void setPB(int bus, float v) {
	patchBus[bus][0]= v; 
	patchBus[bus][1]= v; 
	return;
}

void presetPatches(unsigned char prg) {
	if (prg==0) {
// AOUT0 : SAWUP
// AOUT1 : SQUARE PWM
// AIN0	 : PULSEWIDTH
// AIN1  : FINETUNE
		//osc1
		patchOut[0][0]	= 0;
		patchOut[0][1]	= 10;
		patchIn[0][0]	= 160;
		patchIn[0][1]	= 131;
		patchNote[0]	= 0;

		//osc2
		patchOut[1][0]	= 1; 
		patchOut[1][1]	= 11;
		patchIn[1][0]	= 140;//130;
		patchIn[1][1]	= 130;
		patchNote[1]	= 1;
		
		setPB(130,0.3);
		setPB(131,0);

		// filter1
		patchIn[6][0]	= 20;  // signal in
		patchIn[6][1]	= 145; // CF
		patchIn[6][2]	= 146; // res
		patchOut[6][0]	= 1;
		patchOut[6][1]	= DUMP;
		patchOut[6][2]	= DUMP;
		
		// filter2
		patchIn[8][0]	= 11;  // signal in
		patchIn[8][1]	= 140; // CF
		patchIn[8][2]	= 146; // res
		patchOut[8][0]	= 0;
		patchOut[8][1]	= DUMP;
		patchOut[8][2]	= DUMP;
		setPB(141,0.8);
		setPB(140,0.6);

		patchIn[5][0]   = 9;
		patchIn[5][1]   = 8;
		patchIn[5][2]   = 21;
		patchOut[5][0]  = DUMP;
		patchOut[5][1]  = DUMP;
		
		//LFO
		patchIn[9][0]	= 142;
		patchIn[9][1]	= 143;
		patchOut[9][0]	= 140;
		
		setPB(142,-0.2);
		setPB(143,0.98);
		
		//LFO2
		patchIn[10][0]	= 144;
		patchIn[10][1]	= 143;
		patchOut[10][0]	= 145;
		
		setPB(144,0.5);
		setPB(146,0.0);

	}
	
	return;
}


// Registration of the modules to the OpenModular
// This function is called by the OpenModular
void moduleRegistration(void) {
	regModule_Oscilator1(0);
	regModule_Oscilator2(1);
	regModule_Sequencer(2);
/*	regModule_Smoothie(2);
	regModule_SampleAndHold(3);
	regModule_ADSR(4);
	regModule_Gain(5);
	regModule_Filter1(6);
	
	regModule_Filter2(8);
	regModule_LFO(9);
	regModule_LFO2(10);
	*/
//	regModule_Gain(0);
//	regModule_ADSR(1);
//	
//	regModule_Filter1(4);
//	regModule_Filter2(5);
//	regModule_LFO(6);
//	regModule_SampleAndHold(7);
//	regModule_Sequencer(8);

	numberOfModules=3;

	return;
}
