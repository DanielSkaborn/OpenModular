// OpenModular
// modules.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn



//float sinustable[8193];
float wavetable[4097][3];
int tableToUse = 0;

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
	patchBus[OUTL][togglerOut] = AIN0;
	patchBus[OUTR][togglerOut] = AIN1;
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

void module_ADSR1(int id) {
	static int state=0;
	static unsigned char lastgate=0;
	static float d=0, signal=0, sustain=0;

	if (id==-1) { // init statics
		state=0;
		lastgate=0;
		d=0.0; signal=0.0; sustain=0.0;
		return;
	}

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
			if (AIN0 != -1)
				d=1.0f/( (AIN0+1.0) * 2.5 * SAMPLERATEF);
			else
				d=0.3;
			signal = 0.0f;
			state  = 2;
		case 2: // Attack proceed
			signal+=d;
			if (signal > 1.0f) {
				signal = 1.0f;
				state = 3;
			}
			break;
		case 3: // Decay calculate
			if (AIN0 != -1)
				d = 1.0f/( (AIN0+1.0) * 2.5 * SAMPLERATEF);
			else
				d = 0.1;
			sustain = (float)(AIN2+1.0)/2.0;
			state = 4;
		case 4: // Decay proceed
			signal -= d;
			if (signal < sustain) {
				state = 5;
			}
			break;
		case 5: // Sustain
			break;
		case 6: // Release
			d=1.0f/( (AIN3+1.0) * 2.5 * SAMPLERATEF);
			state = 7;
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
	AOUT0=signal;
	
	// just a little VCA
	AOUT1=AIN4*signal;
	AOUT2=AIN5*signal;
	return;
}
void regModule_ADSR1(int id) {
	moduleRegistry[id] = module_ADSR1;
	module_ADSR1(-1); // init
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "ATT DEC SUS REL IN1 IN2                                         \0";
	char outNames[4*MAXOUT+1] = "ADSROUT1OUT2                                                    \0";
//               "        \0";
	char name[9]="ADSR1   \0";

	modIns[id]     = 6;
	modOuts[id]    = 3;

	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_ADSR2(int id) {
	static int state=0;
	static unsigned char lastgate=0;
	static float d=0, signal=0, sustain=0;

	if (id==-1) { // init statics
		state=0;
		lastgate=0;
		d=0.0; signal=0.0; sustain=0.0;
		return;
	}

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
			if (AIN0 != -1)
				d=1.0f/( (AIN0+1.0) * 2.5 * SAMPLERATEF);
			else
				d=0.3;
			signal = 0.0f;
			state  = 2;
		case 2: // Attack proceed
			signal+=d;
			if (signal > 1.0f) {
				signal = 1.0f;
				state = 3;
			}
			break;
		case 3: // Decay calculate
			if (AIN0 != -1)
				d = 1.0f/( (AIN0+1.0) * 2.5 * SAMPLERATEF);
			else
				d = 0.1;
			sustain = (float)(AIN2+1.0)/2.0;
			state = 4;
		case 4: // Decay proceed
			signal -= d;
			if (signal < sustain) {
				state = 5;
			}
			break;
		case 5: // Sustain
			break;
		case 6: // Release
			d=1.0f/( (AIN3+1.0) * 2.5 * SAMPLERATEF);
			state = 7;
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
	AOUT0=signal;
	
	// just a little VCA
	AOUT1=AIN4*signal;
	AOUT2=AIN5*signal;
	return;
}
void regModule_ADSR2(int id) {
	moduleRegistry[id] = module_ADSR2;
	module_ADSR2(-1); // init
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "ATT DEC SUS REL IN1 IN2                                         \0";
	char outNames[4*MAXOUT+1] = "ADSROUT1OUT2                                                    \0";
//               "        \0";
	char name[9]="ADSR2   \0";

	modIns[id]     = 6;
	modOuts[id]    = 3;

	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_WTCrunch(int id) {
	static float s=0;
	static int state = 0;
	static int tableToEdit=1;
	static int c=0;
	static int rest;
	
	if (id==-1) {
		s=0.0;
		state = 0;
		tableToEdit=1;
		c=0;
		rest=0;
		return;
	}
	
	if (c==4097) {
		state=0;
		c=0;
		s=0;
		tableToEdit++;
		if (tableToEdit == 3) tableToEdit=0;
		tableToUse++;
		if (tableToUse == 3) tableToUse=0;
	}
	
	switch (state) {
		case 0:
			s = wavetable[c][tableToEdit] = s + (AIN4*(AIN5+1.0)/3.0) + (AIN0+1.1)/1024;
			if ( s < -1.0 ) state = 1;
			if ( s >  1.0 ) state = 1;
			break;
		case 1:
			s = wavetable[c][tableToEdit] = s + (AIN4*(AIN5+1.0)/3.0) - (AIN1+1.1)/1024;
			if ( s < -1.0 ) state = 2;
			if ( s >  1.0 ) state = 2;
			break;
		case 2:
			s = wavetable[c][tableToEdit] = s + (AIN4*(AIN5+1.0)/3.0) + (AIN2+1.1)/1024;
			if ( s < -1.0 ) state = 3;
			if ( s >  1.0 ) state = 3;
			break;
		case 3:
			rest = 4096-c;
			state = 4;
		case 4:
			s = wavetable[c][tableToEdit] = s + (AIN4*(AIN5+1.0)/3.0) - (AIN1+1.1)/rest;
			if ( s < -1.0 ) state = 5;
			if ( s >  1.0 ) state = 5;
			break;
		case 5:
			s = wavetable[c][tableToEdit] = s + (AIN4*(AIN5+1.0)/3.0);
			break;
		default:
			state = 0;
			break;		
	}
	c++;
	
	return;
}
void regModule_WTCrunch(int id) {
	moduleRegistry[id] = module_WTCrunch;
	module_WTCrunch(-1);
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "XYZ YZX ZXY ZYX IN  iLEV                                        \0";
	char outNames[4*MAXOUT+1] = "                                                                \0";
//               "        \0";
	char name[9]="WTCrunch\0";

	modIns[id]     = 5;
	modOuts[id]    = 0;

	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Gate2Bus(int id) { 
	static float temp1=0;
	static float temp2=0;
	float rel;
	
	if (id == -1) {
		temp1=0.0; temp2=0.0;
		return;
	}
	
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
	module_Gate2Bus(-1); // init
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
	static int counter=0;
	static int countto=10;
	static int dir=0;
	static float accout=0.0;
	static float temp0,temp1;
	
	if (id==-1) { // init statics
		counter=0; countto=10;
		dir=0; accout=0.0;
		temp0=0.0; temp1=0.0;
		return;
	}
			
	counter++;
	if ( counter == countto ) {

		
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
	module_LFO(-1); // init
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

void module_LFO2(int id) {
	static int counter=0;
	static int countto=10;
	static int dir=0;
	static float accout=0.0;
	static float temp0,temp1;

	if (id==-1) { // init statics
		counter=0; countto=10;
		dir=0; accout=0.0;
		temp0=0.0; temp1=0.0;
		return;
	}
			
	counter++;
	if ( counter == countto ) {
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
	moduleRegistry[id] = module_LFO2;
	module_LFO2(-1); // init
	
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
	static int counter=0;
	float smooth;
	
	if (id==-1) {
		counter=0;
		smooth=0.0;
		return;
	}
	
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
	module_Smoothie(-1); // init
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
	static int counter=0;
	float temp;

	if (id==-1) {
		counter=0;
		return;
	}

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
	module_SampleAndHold(-1);
	
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
	static int step = 0;
	static int counter = -1;
	
	if (id==-1) { // init statics
		step=0;
		counter = 0;
		return;
	}
	
	counter++;
	if (counter > SAMPLERATE/10) {
		gate[0]=0;
	}
	if (counter > SAMPLERATE/5) {
		step++;
		counter=0;
		
		switch(step) {
			case 0:
				gate[0]=126;
				note[0]=49;
				gate[1]=200;
				note[1]=47;
				break;
			case 1:
				note[0]=47;
				note[1]=49;
				break;
			case 2:
				note[0]=42;
				note[1]=42;
				break;
			case 3:
				note[0]=49;
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
	module_Sequencer(-1); // init
	
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

	static float lastcin0 = -2;
	static float lastcin1 = -2;
	
	static float frequency=0, resonance=0;
	static float freqinhz=0;
	static float f=0, p=0, q=0;             //filter coefficients
	static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	static float t1=0, t2=0;              //temporary buffers
	
	float temp;
	
	if (id == -1) { // init statics
		lastcin0 = -2; lastcin1 = -2; frequency=0.0; resonance=0.0; freqinhz=0.0;
		f=0.0; p=0.0; q=0.0;
		in=0.0, b0=0.0, b1=0.0, b2=0.0, b3=0.0, b4=0.0;
		t1=0.0, t2=0.0;
		return;
	}
	
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
	module_Filter1(-1);
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
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

	static float lastcin0 = -2;
	static float lastcin1 = -2;
	
	static float frequency=0, resonance=0;
	static float freqinhz=0;
	static float f=0, p=0, q=0;             //filter coefficients
	static float in=0, b0=0, b1=0, b2=0, b3=0, b4=0;  //filter buffers (beware denormals!)
	static float t1=0, t2=0;              //temporary buffers
	
	static float temp;

	if (id == -1) { // init statics
		lastcin0 = -2; lastcin1 = -2; frequency=0.0; resonance=0.0; freqinhz=0.0;
		f=0.0; p=0.0; q=0.0;
		in=0.0, b0=0.0, b1=0.0, b2=0.0, b3=0.0, b4=0.0;
		t1=0.0, t2=0.0;
		temp = 0.0;
		return;
	}

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
	module_Filter2(-1); // init
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
	static float freq=0, toneFreq=0;
	static float d=0;
	static float pw=0;
	static unsigned char lastnote=0;
	static float temp=0;
	
	float tempsq;
	int i;

	if (id == -1) { // init statics
		d=0; pw=0; lastnote = 0; toneFreq=20.1; freq=0;
		return;
	}
	
	if (NOTE!=lastnote) {
		lastnote=NOTE;
	
		// lookuptable, basictone to frequency (HZ)
		toneFreq = noteToFreqLUT[NOTE + (int)(AIN2*12)];
	}
		
	// Apply finetune
	
	freq = toneFreq + (AIN1*2.0); // +/- 0.5Hz
	if (freq<SAMPLERATEF/3.0) {
		// generate a SAWUP
		d = freq / SAMPLERATEF /2.0; // calculate d
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
	
//		AOUT2 = sinf(phase);
//		phase = phase + ((6.28318530718 * freq) / SAMPLERATEF);
//		if (phase > 6.28318530718)
//			phase = phase - 6.28318530718;
	} else {
		AOUT0=0;
		AOUT1=0;
//		AOUT2=0;
	}
	return;
}
void regModule_Oscilator1(int id) {

	moduleRegistry[id] = module_Oscilator1;
	module_Oscilator1(-1); // init
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "PW  TUNECRS                                                     \0";
	char outNames[4*MAXOUT+1] = "SAW SQR                                                         \0";
	char name[9]="OSC1    \0";
	
	modIns[id]     = 3;
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

	static float freq=0, toneFreq=0;
	static float d=0;
	static float pw=0;
	static unsigned char lastnote=0;
	static float temp=0;
	
	int i;
	float tempsq;
	
	if (id == -1) { // init statics
		d=0; pw=0; lastnote = 0; toneFreq=20.1; freq=0;
		return;
	}
	
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
	if (temp>(1.0-(d*4.0)))
		AOUT0 = 1.0 - d/2.0;
	else if (temp<(-1.0+(d*4)))
		AOUT0 = -1.0 + d/2.0;
		
	AOUT1 = tempsq;

	return;
}
void regModule_Oscilator2(int id) {
	moduleRegistry[id] = module_Oscilator2;
	module_Oscilator2(-1);
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "PW  TUNE                                                        \0";
	char outNames[4*MAXOUT+1] = "SAW SQR                                                         \0";
	char name[9]="OSC2    \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void module_WavetableOsc(int id) {
	
	static int switcher=0;
	static unsigned char lastnote=0;
	static float toneFreq=20.1;
	static float phase = 0.0;
	float freq;
	float temp;
	unsigned char lowbit;

	if (id == -1) { // init statics
		switcher = 0;
		lastnote = 0;
		toneFreq=20.1;
		phase = 0.0;
		return;
	}

	if (NOTE != lastnote) {
		lastnote=NOTE;
		toneFreq = noteToFreqLUT[NOTE];
	}
	
	freq=toneFreq;

	phase = phase + (4096.0/SAMPLERATEF * (freq+AIN0*5.0) );
	if (phase > 4096.0) {
		phase = phase - 4096.0;
		switcher = tableToUse;
	}
	
	temp = AOUT0 = wavetable[(int)(phase)][switcher];
	lowbit = (unsigned char)(temp*255);
	AOUT1 = ((float)(lowbit))/255.0;
	
	return;
}

void regModule_WavetableOsc(int id) {
	int i;
	moduleRegistry[id] = module_WavetableOsc;
	module_WavetableOsc(-1); // init
	for (i=0;i<2048;i++)
		wavetable[i][0] = -1.0;
	for (i=2049;i<4096;i++)
		wavetable[i][0] = 1.0;
	for (i=0;i<4096;i++)
		wavetable[i][1] = (i/1024.0)-1.0;


//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "TUNE                                                            \0";
	char outNames[4*MAXOUT+1] = "OUT 8BIT                                                        \0";
//               "        \0";
	char name[9]="WaveTabl\0";
	
	modIns[id]     = 1;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void module_WavetableOsc2(int id) {
	static int switcher=0;
	static unsigned char lastnote=0;
	static float toneFreq=20.1;
	static float phase = 0.0;

	float freq;
	float temp;
	unsigned char lowbit;
	
	if (id == -1) { // init statics
		switcher = 0;
		lastnote = 0;
		toneFreq=20.1;
		phase = 0.0;
		return;
	}
	
	if (NOTE != lastnote) {
		lastnote=NOTE;
		toneFreq = noteToFreqLUT[NOTE];
	}
	
	freq=toneFreq;

	phase = phase + (4096.0/SAMPLERATEF * (freq+AIN0*5.0) );
	if (phase > 4096.0) {
		phase = phase - 4096.0;
		switcher = tableToUse;
	}
	
	temp = AOUT0 = wavetable[(int)(phase)][switcher];
	lowbit = (unsigned char)(temp*255);
	AOUT1 = ((float)(lowbit))/255.0;
}

void regModule_WavetableOsc2(int id) {
	int i;
	moduleRegistry[id] = module_WavetableOsc2;
	module_WavetableOsc2(-1); // init
	
	for (i=0;i<2048;i++)
		wavetable[i][0] = -1.0;
	for (i=2049;i<4096;i++)
		wavetable[i][0] = 1.0;
	for (i=0;i<4096;i++)
		wavetable[i][1] = (i/1024.0)-1.0;


//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "TUNE                                                            \0";
	char outNames[4*MAXOUT+1] = "OUT 8BIT                                                        \0";
//               "        \0";
	char name[9]="WaveTabl\0";
	
	modIns[id]     = 1;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}
/*
void module_Additive(int id) {
	
	static unsigned char lastnote=0;
	int i;
	static float toneFreq=20.1, out=0.0;
	float freq;
	static float phase[20] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, };
	
	if (NOTE != lastnote) {
		lastnote=NOTE;
		toneFreq = noteToFreqLUT[NOTE];
	}	
			
	freq=toneFreq;

	out = 0.0;
	for (i=0;i<20;i++) {
		phase[i] = phase[i] + (8192.0/SAMPLERATEF * freq );
		if (phase[i] > 8192.0) phase[i] = phase[i] - 8192.0;
		if (freq < (SAMPLERATEF/2.0))
			out = out + (sinustable[(int)(phase[i])] * (1/(float)(i+1)));
		freq=freq+toneFreq+(toneFreq*((AIN0+1.0)/2.0));
	}
	AOUT0 = out * (AIN1+1.0);
}

void regModule_Additive(int id) {
	int i;
	moduleRegistry[id] = module_Additive;

	for (i=0;i<8193;i++)
		sinustable[i] = sinf(2.0*3.14159265359*(float)(i)/8192.0)*sinf(2.0*3.14159265359*(float)(i)/8192.0);


//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "SHP LEV                                                         \0";
	char outNames[4*MAXOUT+1] = "OUT                                                             \0";
//               "        \0";
	char name[9]="ADDITIVE\0";
	
	modIns[id]     = 2;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}
*/
void setPB(int bus, float v) {
	patchBus[bus][0]= v; 
	patchBus[bus][1]= v; 
	return;
}

void presetPatches(unsigned char prg) {
	switch (prg) {
		case 0: patchIn[0][0] = 121; patchIn[0][1] = 123; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchNote[1] = 0; patchGate[1] = 2; patchIn[2][0] = 255; patchNote[2] = 1; patchGate[2] = 2; patchNote[3] = 0; patchGate[3] = 2;
			break;
		case 8:	patchIn[0][0] = 124; patchIn[0][1] = 127; patchNote[0] = 0; patchGate[0] = 2; patchNote[1] = 0; patchGate[1] = 2; patchIn[2][0] = 255; patchIn[2][1] = 255; patchIn[2][2] = 255; patchNote[2] = 0; patchGate[2] = 2; patchIn[3][0] = 255; patchIn[3][1] = 255; patchNote[3] = 1; patchGate[3] = 2; patchIn[4][0] = 120; patchIn[4][1] = 134; patchIn[4][2] = 10; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 122; patchIn[5][1] = 135; patchIn[5][2] = 10; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 129; patchIn[6][1] = 136; patchNote[6] = 2; patchGate[6] = 1; patchIn[7][0] = 122; patchIn[7][1] = 255; patchIn[7][2] = 6; patchIn[7][3] = 130; patchIn[7][4] = 130; patchIn[7][5] = 255; patchIn[7][6] = 255; patchIn[7][7] = 255; patchIn[7][8] = 255; patchIn[7][9] = 255; patchIn[7][10] = 255; patchNote[7] = 2; patchGate[7] = 2; 
			break;
		case 1: patchIn[0][0] = 134; patchIn[0][1] = 125; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 144; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 123; patchIn[2][1] = 134; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 122; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 0; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 121; patchIn[6][1] = 120; patchIn[6][2] = 10; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 124; patchIn[7][2] = 10; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 138; patchIn[10][1] = 142; patchNote[10] = 2; patchGate[10] = 2;
			break;
		case 2: patchIn[0][0] = 134; patchIn[0][1] = 125; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 144; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 123; patchIn[2][1] = 134; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 122; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 0; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 121; patchIn[6][1] = 120; patchIn[6][2] = 10; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 124; patchIn[7][2] = 10; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 138; patchIn[10][1] = 142; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 146; patchIn[11][1] = 147; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		case 3:	patchIn[0][0] = 255; patchIn[0][1] = 255; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 255; patchIn[1][1] = 255; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 255; patchIn[2][1] = 255; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 255; patchIn[3][1] = 255; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 146; patchIn[6][1] = 10; patchIn[6][2] = 255; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 147; patchIn[7][1] = 10; patchIn[7][2] = 255; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 120; patchIn[10][1] = 122; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 138; patchIn[11][1] = 141; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		case 4: patchIn[0][0] = 144; patchIn[0][1] = 200; patchNote[0] = 0; patchGate[0] = 2; patchIn[1][0] = 134; patchIn[1][1] = 200; patchNote[1] = 1; patchGate[1] = 2; patchIn[2][0] = 134; patchIn[2][1] = 140; patchIn[2][2] = 255; patchIn[2][3] = 255; patchIn[2][4] = 255; patchIn[2][5] = 255; patchIn[2][6] = 255; patchIn[2][7] = 255; patchIn[2][8] = 255; patchIn[2][9] = 255; patchIn[2][10] = 255; patchNote[2] = 2; patchGate[2] = 2; patchIn[3][0] = 120; patchIn[3][1] = 12; patchNote[3] = 2; patchGate[3] = 2; patchIn[4][0] = 255; patchIn[4][1] = 255; patchIn[4][2] = 255; patchIn[4][3] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 255; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 120; patchIn[6][1] = 10; patchIn[6][2] = 1; patchNote[6] = 2; patchGate[6] = 2; patchIn[7][0] = 122; patchIn[7][1] = 148; patchIn[7][2] = 1; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 255; patchIn[8][1] = 255; patchNote[8] = 2; patchGate[8] = 2; patchIn[9][0] = 255; patchIn[9][1] = 255; patchNote[9] = 2; patchGate[9] = 2; patchIn[10][0] = 121; patchIn[10][1] = 123; patchNote[10] = 2; patchGate[10] = 2; patchIn[11][0] = 146; patchIn[11][1] = 147; patchNote[11] = 2; patchGate[11] = 2; 
			break;
		case 5: patchIn[0][0] = 124; patchIn[0][1] = 127; patchNote[0] = 0; patchGate[0] = 2; patchNote[1] = 0; patchGate[1] = 2; patchIn[2][0] = 255; patchNote[2] = 0; patchGate[2] = 2; patchIn[3][0] = 255; patchNote[3] = 1; patchGate[3] = 2; patchIn[4][0] = 121; patchIn[4][1] = 255; patchIn[4][2] = 255; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 123; patchIn[5][1] = 255; patchIn[5][2] = 255; patchNote[5] = 2; patchGate[5] = 2;
			break;
		case 6: patchIn[0][0] = 124; patchIn[0][1] = 127; patchNote[0] = 0; patchGate[0] = 2; patchNote[1] = 0; patchGate[1] = 2; patchIn[2][0] = 255; patchNote[2] = 0; patchGate[2] = 2; patchIn[3][0] = 255; patchNote[3] = 1; patchGate[3] = 2; patchIn[4][0] = 120; patchIn[4][1] = 130; patchIn[4][2] = 121; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 122; patchIn[5][1] = 130; patchIn[5][2] = 122; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 129; patchIn[6][1] = 120; patchIn[6][2] = 120; patchIn[6][3] = 123; patchIn[6][4] = 255; patchIn[6][5] = 255; patchNote[6] = 2; patchGate[6] = 1; 
			break;
		case 7: patchIn[0][0] = 124; patchIn[0][1] = 127; patchNote[0] = 0; patchGate[0] = 2; patchNote[1] = 0; patchGate[1] = 2; patchIn[2][0] = 255; patchNote[2] = 0; patchGate[2] = 2; patchIn[3][0] = 255; patchNote[3] = 1; patchGate[3] = 2; patchIn[4][0] = 121; patchIn[4][1] = 134; patchIn[4][2] = 10; patchNote[4] = 2; patchGate[4] = 2; patchIn[5][0] = 123; patchIn[5][1] = 135; patchIn[5][2] = 10; patchNote[5] = 2; patchGate[5] = 2; patchIn[6][0] = 129; patchIn[6][1] = 136; patchIn[6][2] = 120; patchIn[6][3] = 123; patchIn[6][4] = 255; patchIn[6][5] = 255; patchNote[6] = 2; patchGate[6] = 1; patchIn[7][0] = 122; patchIn[7][1] = 255; patchNote[7] = 2; patchGate[7] = 2; patchIn[8][0] = 133; patchIn[8][1] = 133; patchIn[8][2] = 125; patchIn[8][3] = 255; patchIn[8][4] = 255; patchIn[8][5] = 255; patchIn[8][6] = 255; patchIn[8][7] = 255; patchIn[8][8] = 255; patchIn[8][9] = 255; patchIn[8][10] = 134; patchNote[8] = 2; patchGate[8] = 2; 
			break;
		default:
			break;
	}
	
	return;
}


// Registration of the modules to the OpenModular
// This function is called by the OpenModular
void moduleRegistration(void) {
	//regModule_Oscilator1(0);
	regModule_Output(0);
//	regModule_Oscilator1(2);
//	regModule_Oscilator2(1);
	regModule_WavetableOsc(1);
	regModule_WavetableOsc2(2);
	regModule_Sequencer(3);
	regModule_Filter1(4);
	regModule_Filter2(5);
//	regModule_ADSR1(6);
//	regModule_WTCrunch(7);
	regModule_LFO(7);
	regModule_SampleAndHold(6);
//	regModule_Smoothie(7);

/*	regModule_Oscilator1(0);
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
*/	//regModule_Sequencer(12);

	numberOfModules=4;

	return;
}
