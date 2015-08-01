// OpenModular
// modules.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn




#define e		2.718281828459045

//float cosLut50[] = {1, 0.9921147013, 0.9685831611, 0.9297764859, 0.87630668, 0.8090169944, 0.7289686274, 0.6374239897, 0.535826795, 0.4257792916, 0.3090169944, 0.1873813146, 0.0627905195, -0.0627905195, -0.1873813146, -0.3090169944, -0.4257792916, -0.535826795, -0.6374239897, -0.7289686274, -0.8090169944, -0.87630668, -0.9297764859, -0.9685831611, -0.9921147013, -1, -0.9921147013, -0.9685831611, -0.9297764859, -0.87630668, -0.8090169944, -0.7289686274, -0.6374239897, -0.535826795, -0.4257792916, -0.3090169944, -0.1873813146, -0.0627905195, 0.0627905195, 0.1873813146, 0.3090169944, 0.4257792916, 0.535826795, 0.6374239897, 0.7289686274, 0.8090169944, 0.87630668, 0.9297764859, 0.9685831611, 0.9921147013, 1};

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

void module_JanostDCF(int id) {
	static int32_t f,q,vi,vo,t1,t2,t3,t4,t5,t6,t7,t8;
	
	f = (AIN1+1.0) * 32767;
	q = (AIN1+1.0) * 32767;
	vi = (int)(AIN0*32767);
	
	vo = ((vo * (65536 - f)) + (t1 * f)) / 65536;
	t1 = ((t1 * (65536 - f)) + (t2 * f)) / 65536;
	t2 = ((t2 * (65536 - f)) + (t3 * f)) / 65536;
	t3 = ((t3 * (65536 - f)) + (t4 * f)) / 65536;
	t4 = ((t4 * (65536 - f)) + (t5 * f)) / 65536;
	t5 = ((t5 * (65536 - f)) + (t6 * f)) / 65536;
	t6 = ((t6 * (65536 - f)) + (t7 * f)) / 65536;
	t7 = ((t7 * (65536 - f)) + (t8 * f)) / 65536;
	t8 = vi-((vo*q)/65536);
	if (vo>32767) vo= 32767;
	if (vo<-32767) vo= -32767;
	
	AOUT0 = vo / 32767.0;
	return;
}

void regModule_JanostDCF(int id) {
	moduleRegistry[id] = module_JanostDCF;

//                               "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]    = "IN  F   Q                                                       \0"; 	
	char outNames[4*MAXOUT+1]  = "OUT                                                             \0";
//               "        \0";
	char name[9]="Jano-DCF\0";

	modIns[id]      = 3;
	modOuts[id]     = 1;

	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Gain(int id) { 
	AOUT0 = AIN0 * AIN1;
	AOUT1 = AIN2 * AIN3;
	AOUT2 = AIN4 * AIN5;
	AOUT3 = AIN6 * AIN7;
	AOUT4 = (AOUT0 + AOUT1 + AOUT2 + AOUT3)/4.0;
	return;
}
void regModule_Gain(int id) {
	moduleRegistry[id] = module_Gain;

//                               "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]    = "X1  Y1  X2  Y2  X3  Y3  X4  Y4                                  \0"; 	
	char outNames[4*MAXOUT+1]  = "Z1  Z2  Z3  Z4  SUMZ                                            \0";
//               "        \0";
	char name[9]="MULTIPLY\0";

	modIns[id]      = 8;
	modOuts[id]     = 5;

	copymodstrings(id, name, inNames, outNames);
	return;
}

// Output module
void module_Output(int id) { 
	patchBus[OUTL][togglerOut] = AIN0 ;
	patchBus[OUTR][togglerOut] = AIN1 ;
	return;
}
void regModule_Output(int id) {
	moduleRegistry[id] = module_Output;

//                               "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]    = "L   R                                                           \0"; 	
	char outNames[4*MAXOUT+1]  = "                                                                \0";
//               "        \0";
	char name[9]="OUTPUT  \0";

	modIns[id]      = 2;
	modOuts[id]     = 0;

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

//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "ATT DEC SUS REL                                                 \0";
	char outNames[4*MAXOUT+1] = "ADSR                                                            \0";
//               "        \0";
	char name[9]="ADSR    \0";

	modIns[id]     = 4;
	modOuts[id]    = 1;

	copymodstrings(id, name, inNames, outNames);
	return;
}
void module_Gate2Bus(int id) { 
	static float temp1=0;
	static float temp2=0;
	float rel;
	AOUT2 = (float)(gate[0]) / 128.0;
	AOUT3 = (float)(gate[1]) / 128.0;

	if (AOUT2 > temp1) temp1 += 0.1;
	if (AOUT3 > temp2) temp2 += 0.1;

	if ((AOUT2!=0.0) && (temp1 > AOUT2)) temp1 = AOUT2;
	if ((AOUT3!=0.0) && (temp2 > AOUT3)) temp2 = AOUT3;

	rel = (AIN2+1.0)/2.0;
	rel/=10000.0;
	if (rel < 0.0001) rel=0.0001;

	temp1 = temp1 - 0.0001;
	temp2 = temp2 -0.0001;

	if (temp1 < 0.0) temp1 = 0.0;
	if (temp2 < 0.0) temp2 = 0.0;

	AOUT0 = temp1 * AIN0;
	AOUT1 = temp2 * AIN1;
	return;
}

void regModule_Gate2Bus(int id) {
	moduleRegistry[id] = module_Gate2Bus;

//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN1 IN2 R                                                       \0";
	char outNames[4*MAXOUT+1] = "GA1 GA2 G1  G2                                                  \0";
//               "        \0";
	char name[9]="GATE2BUS\0";

	modIns[id]     = 3;
	modOuts[id]    = 4;

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
	char name[9]="LFOTRI2 \0";
	
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
	char inNames[4*MAXIN+1]   = "IN0 IN1 IN2 IN3 IN4 IN5 IN6 IN7 IN8 IN9 SMTH                    \0";
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
//				note[1]=47;
				break;
			case 1:
				note[0]=49;
//				note[1]=49;
				break;
			case 2:
				note[0]=47;
//				note[1]=42;
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
//				note[1]=49;
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
	int i;
	static float temp=0;
	float tempsq;
	
	if (NOTE!=lastnote) {
		lastnote=NOTE;
	
		// lookuptable, basictone to frequency (HZ)
		toneFreq = noteToFreqLUT[NOTE];
	}	
		
	// Apply finetune
	freq = toneFreq + AIN1/2.0; // +/- 0.5Hz
	
	// generate a SAWUP
	d = freq / SAMPLERATEF /2.0;//*2 0;	// calculate d
	pw = (AIN0 + 1.0)*0.45;
	
	tempsq=0;
	
	// 4x oversampling
	for (i=0;i<4;i++) {
		// Generate SAWUP
		temp += d;
		if (temp > 1.0) 
			temp -= 2.0;

		// Generate SQUARE-PW
		if (temp > pw)
			tempsq+=0.25;
		else
			tempsq-=0.25;
	 }

	AOUT0 = temp;
	AOUT1 = tempsq;

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
	int i;
	static float temp=0;
	float tempsq;
	
	if (NOTE!=lastnote) {
		lastnote=NOTE;
	
		// lookuptable, basictone to frequency (HZ)
		toneFreq = noteToFreqLUT[NOTE];
	}	
		
	// Apply finetune
	freq = toneFreq + AIN1/2.0; // +/- 0.5Hz
	
	// generate a SAWUP
	d = freq / SAMPLERATEF /2.0;//*2 0;	// calculate d
	pw = (AIN0 + 1.0)*0.45;
	
	tempsq=0;
	
	// 4x oversampling
	for (i=0;i<4;i++) {
		// Generate SAWUP
		temp += d;
		if (temp > 1.0) 
			temp -= 2.0;

		// Generate SQUARE-PW
		if (temp > pw)
			tempsq+=0.25;
		else
			tempsq-=0.25;
	 }

	AOUT0 = temp;
	AOUT1 = tempsq;

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
	switch (prg) {
		case 0: patchIn[0][0] = 144; patchIn[0][1] = 200; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 134; patchIn[1][1] = 200; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 134; patchIn[2][1] = 140; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 12; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 121; patchIn[6][1] = 10; patchIn[6][2] = 1; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 148; patchIn[7][2] = 1; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 121; patchIn[10][1] = 123; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 138; patchIn[11][1] = 141; patchNote[11] = 2; patchGate[11] = 2; patchNote[12] = 2; patchGate[12] = 2;
			break;
		case 1: patchIn[0][0] = 134; patchIn[0][1] = 125; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 144; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 123; patchIn[2][1] = 134; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 122; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 0; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 121; patchIn[6][1] = 120; patchIn[6][2] = 10; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 124; patchIn[7][2] = 10; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 138; patchIn[10][1] = 142; patchNote[10] = 2; patchGate[10] = 2;
			break;
		case 2: patchIn[0][0] = 134; patchIn[0][1] = 125; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 144; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 123; patchIn[2][1] = 134; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 122; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 0; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 121; patchIn[6][1] = 120; patchIn[6][2] = 10; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 124; patchIn[7][2] = 10; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 138; patchIn[10][1] = 142; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 146; patchIn[11][1] = 147; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		case 3:	patchIn[0][0] = 255; patchIn[0][1] = 255; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 255; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 255; patchIn[2][1] = 255; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 255; patchIn[3][1] = 255; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 146; patchIn[6][1] = 10; patchIn[6][2] = 255; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 147; patchIn[7][1] = 10; patchIn[7][2] = 255; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 120; patchIn[10][1] = 122; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 138; patchIn[11][1] = 141; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		case 4: patchIn[0][0] = 144; patchIn[0][1] = 200; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 134; patchIn[1][1] = 200; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 134; patchIn[2][1] = 140; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 12; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 120; patchIn[6][1] = 10; patchIn[6][2] = 1; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 148; patchIn[7][2] = 1; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 121; patchIn[10][1] = 123; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 146; patchIn[11][1] = 147; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		default:
			break;
	}
	
	return;
}


// Registration of the modules to the OpenModular
// This function is called by the OpenModular
void moduleRegistration(void) {
	regModule_Oscilator1(0);
	regModule_Oscilator2(1);
	regModule_Smoothie(2);
	regModule_SampleAndHold(3);
	regModule_ADSR(4);
	regModule_Gain(5);
	regModule_Filter1(6);
	regModule_Filter2(7);
	regModule_LFO(8);
	regModule_LFO2(9);
	regModule_Gate2Bus(10);
	regModule_Output(11);
	regModule_Sequencer(12);
	regModule_JanostDCF(13);

	numberOfModules=14;

	return;
}
