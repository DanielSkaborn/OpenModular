// OpenModular
// modules.c
//
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include <time.h>
#include <stdlib.h>
#include <math.h>

float sinustable[8193];
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
	char name[9]="Multiply\0";

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
	char name[9]="Output  \0";

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
			d=1.0f/( (AIN0+1.02) * 2.5 * SAMPLERATEF);
			//signal = 0.0f;
			state  = 2;
		case 2: // Attack proceed
			signal+=d;
			if (signal > 1.0f) {
				signal = 1.0f;
				state = 3;
			}
			break;
		case 3: // Decay calculate
			d = 1.0f/( (AIN1+1.02) * 2.5 * SAMPLERATEF);
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
			d=1.0f/( (AIN3+1.02) * 2.5 * SAMPLERATEF);
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
	AOUT0=(signal*2)-1.0;

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
	char name[9]="Env-1   \0";

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
			d=1.0f/( (AIN0+1.02) * 2.5 * SAMPLERATEF);
//			signal = 0.0f;
			state  = 2;
		case 2: // Attack proceed
			signal+=d;
			if (signal > 1.0f) {
				signal = 1.0f;
				state = 3;
			}
			break;
		case 3: // Decay calculate
			d = 1.0f/( (AIN1+1.02) * 2.5 * SAMPLERATEF);
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
			d=1.0f/( (AIN3+1.02) * 2.5 * SAMPLERATEF);
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
	AOUT0=(signal*2)-1;

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
	char name[9]="Env-2   \0";

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
	char name[9]="Gatebus \0";

	modIns[id]     = 3;
	modOuts[id]    = 4;

	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_LFO1(int id) {
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
		
	AOUT0=(accout-1.0)*AIN3;
	AOUT1=-AOUT0;
	return;
}
void regModule_LFO1(int id) {
	moduleRegistry[id] = module_LFO1;
	module_LFO1(-1); // init
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "RATEMAX AMT                                                     \0";
	char outNames[4*MAXOUT+1] = "TRI INV                                                         \0";
//               "        \0";
	char name[9]="LFOTRI1 \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
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
		
	AOUT0=(accout-1.0)*AIN3;;
	AOUT1=-AOUT0;
	return;
}
void regModule_LFO2(int id) {
	moduleRegistry[id] = module_LFO2;
	module_LFO2(-1); // init
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "RATEMAX AMT                                                     \0";
	char outNames[4*MAXOUT+1] = "TRI INV                                                         \0";
//               "        \0";
	char name[9]="LFOTRI2 \0";
	
	modIns[id]     = 3;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_BD(int id) {
	static int lastgate, /*lastnote,*/ state;
	static int counter;	
	static float phase, freq, dfreq, level, levstep;
	
	if (id==-1) {	// init
		int i;
		for (i=0;i<8193;i++)
			sinustable[i] = sinf(2.0*3.14159265359*(float)(i)/8192.0)*sinf(2.0*3.14159265359*(float)(i)/8192.0);

		lastgate=0;
//		lastnote=1;
		state=0;
		return;
	}
	
	// Gate signal trigging
	if ( (lastgate==0) && (GATE!=0) ) {
		counter = ((AIN0+1)*SAMPLERATE)/20; // up to 1/10 sec
		if (counter<0)
			counter=0;
		freq = (AIN2+1.001) * 250.0;
		dfreq = (AIN3+1) * 0.01;
		phase = 0.0;
		state=1; // Go for it!
	}
	
	switch (state) {
		case 0:
			AOUT0=0; // nada
			break;
			
		case 1: // rise freq
			AOUT0 = sinustable[(int)(phase)];
			phase = phase + (8192.0/SAMPLERATEF*freq);
			if (phase > 8192.0)
				phase = phase - 8192.0;
			freq += dfreq;

			if (counter==0) {
				counter = (AIN1+1)*SAMPLERATE/4; // up to 0.5sec
				levstep = 1.0/(float)(counter);
				dfreq = (AIN4+1)*0.01;
				level=1;
				state=3;
			} else {
				counter--;
			}
			break;
		
		case 3: // drop freq
			AOUT0 = sinustable[(int)(phase)] * level;
			level -= levstep;
			phase = phase + (8192.0/SAMPLERATEF)*freq;
			if (phase > 8192.0) {
				phase = phase - 8192.0;
			}
			if (freq>20) {
				freq -= dfreq;
				dfreq/=1.0001;
			}

			if (freq<10.0) {
				freq=10.0;
				levstep=0.05;
			}
			if (counter==0) {
				state=0;
			} else {
				counter--;
			}
			break;
		default:
			state=0;
			AOUT0=0;
			break;
	}
	lastgate=GATE;
	return;
}

void regModule_BD(int id) {
	moduleRegistry[id] = module_BD;
	module_BD(-1); // init
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "T1  T2  F   dF1 dF2                                             \0";
	char outNames[4*MAXOUT+1] = "BD                                                              \0";
//               "        \0";
	char name[9]="BassDrum\0";
	
	modIns[id]     = 5;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Derive(int id) {
	static float lastvalue1, lastvalue0;
	
	if (id==-1) {
		lastvalue0=0;
		lastvalue1=0;
		return;
	}
	AOUT0 = (AIN0-lastvalue0) * (50*(AIN2+1));
	AOUT1 = (AIN1-lastvalue1) * (50*(AIN2+1));
	lastvalue0 = AIN0;
	lastvalue1 = AIN1;
	return;
}
void regModule_Derive(int id) {
	moduleRegistry[id] = module_Derive;
	module_Derive(-1); // init
	
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "X   Y   Amp                                                     \0";
	char outNames[4*MAXOUT+1] = "dXdTdYdT                                                        \0";
//               "        \0";
	char name[9]="Derive  \0";
	
	modIns[id]     = 3;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}
void module_Smoothie(int id) {
	static int counter=0;
	static float smooth, laout0, laout1, laout2, laout3;
	if (id==-1) {
		laout0 = 0.0;
		laout1 = 0.0;
		laout2 = 0.0;
		laout3 = 0.0;
		smooth = 0.0;
		counter = 0;
		return;
	}
	
	counter++;
	if (counter>201) counter=0;
	switch (counter) {
		case 0:
			smooth = AIN4;
			if ( smooth < 0   ) smooth = - smooth;
			if ( smooth > 1.0 ) smooth = 1.0;
			smooth /= 255.0;
			break;
		case 50:
			if ( AIN0 > laout0 ) AOUT0 = laout0 + smooth;
			if ( AIN0 < laout0 ) AOUT0 = laout0 - smooth;
			laout0 = AOUT0;
			break;
		case 100:
			if ( AIN1 > laout1 ) AOUT1 = laout1 + smooth;
			if ( AIN1 < laout1 ) AOUT1 = laout1 - smooth;
			laout1 = AOUT1;
			break;
		case 150:
			if ( AIN2 > laout2 ) AOUT2 = laout2 + smooth;
			if ( AIN2 < laout2 ) AOUT2 = laout2 - smooth;
			laout2 = AOUT2;
			break;
		case 200:
			if ( AIN3 > laout3 ) AOUT3 = laout3 + smooth;
			if ( AIN3 < laout3 ) AOUT3 = laout3 - smooth;
			laout3 = AOUT3;
			break;
		default:
			break;
	}

	return;
}

void regModule_Smoothie(int id) {
	moduleRegistry[id] = module_Smoothie;
	module_Smoothie(-1); // init
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN0 IN1 IN2 IN3 SMTH                                            \0";
	char outNames[4*MAXOUT+1] = "OUT0OUT1OUT2OUT3                                                \0";
//               "        \0";
	char name[9]="Smoothie\0";
	
	modIns[id]     = 5;
	modOuts[id]    = 4;
	
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
				gate[0]=0;
				note[0]=47;
				note[1]=49;
				break;
			case 2:
				gate[0]=100;
				note[0]=42;
				note[1]=42;
				break;
			case 3:
				note[0]=49;
				gate[0]=0;
				gate[1]=0;
				break;
			case 4:
				gate[0]=120;
				note[0]=47;
				break;
			case 5:
				gate[0]=0;
				note[0]=47;
				note[1]=49;
				gate[1]=100;
				break;
			case 6:
				gate[0]=100;
				note[0]=42;
				break;
			case 7:
				gate[0]=0;
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
	char name[9]="Seq     \0";
	
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
	char name[9]="Filter1 \0";
	
	modIns[id]     = 3;
	modOuts[id]    = 3;
	
	copymodstrings(id, name, inNames, outNames);
	return;
}

void module_Bwlpf(int id) {
	static float s, r, A[4], d1[4], d2[4], Lw0[4], Lw1[4], Lw2[4], Rw0[4], Rw1[4], Rw2[4], lastcin3, lastcin2;
	static int n;

	float temp, f, a, a2 ;
	int i;
	
	if (id==-1) {	// init
		lastcin2=-2;
		lastcin3=-2;
		return;
	}
	
	if ( ( lastcin2 != AIN2 ) || ( lastcin3!=AIN3 ) ) { // calc for new freq / order
		lastcin2 = AIN2; lastcin3 = AIN3;
		if (AIN2<-1.0)
			temp=-0.99;
		else
			temp=AIN2; 
		temp+=1.0;
		f = 20.0f + (temp * 2000.0);	// 20 to 4020Hz
		a = tan(M_PI * f/SAMPLERATEF);
		a2 = a * a;
		
		if ( AIN3 < -0.33 )
			n=2;
		else if ( AIN3 < 0.33)
			n=3;
		else
			n=4;
		
		for(i=0; i<n; ++i) {
			r = sin( M_PI * ( 2.0 * i + 1.0 ) / ( 4.0 * n ) );
			s = a2 + 2.0 * a * r + 1.0;
			A[i] = a2 / s;
			d1[i] = 2.0 * ( 1 - a2 ) / s;
			d2[i] = -(a2 - 2.0 * a * r + 1.0) / s;
		}
	}
	
	// process
	for(i=0; i<n; ++i) {
		Lw0[i] = d1[i]*Lw1[i] + d2[i]*Lw2[i] + AIN0;
		AOUT0 = A[i]*(Lw0[i] + 2.0*Lw1[i] + Lw2[i]);
		Lw2[i] = Lw1[i];
		Lw1[i] = Lw0[i];
      
		Rw0[i] = d1[i]*Rw1[i] + d2[i]*Rw2[i] + AIN1;
		AOUT1 = A[i]*(Rw0[i] + 2.0*Rw1[i] + Rw2[i]);
		Rw2[i] = Rw1[i];
		Rw1[i] = Rw0[i];
    }
	return;
}

void regModule_Bwlpf(int id) {
	moduleRegistry[id] = module_Bwlpf;
	module_Bwlpf(-1); // init
//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "IN1 IN2 CF  N                                                   \0";
	char outNames[4*MAXOUT+1] = "LP1 LP2                                                         \0";
//               "        \0";
	char name[9]="Buttwlpf\0";
	
	modIns[id]     = 4;
	modOuts[id]    = 2;
	
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
	char name[9]="Filter2 \0";
	
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
	char name[9]="Osc1    \0";
	
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
	char name[9]="Osc2    \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void module_Knaster(int id) {
	//static int c;
	int r;
	if (id==-1) {
		//c=0;
		srand(time(NULL));
		return;
	}

	r=rand();
	if (r<0) r=-r;
	
	if (r < (AIN1+1) * RAND_MAX/1000)
		AOUT0 = (float)(rand()) / ((float)(RAND_MAX)) * AIN0;
	else
		AOUT0=0;
	
	return;
}

void regModule_Knaster(int id) {
	moduleRegistry[id] = module_Knaster;
	module_Knaster(-1); // init

//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "LEV PROB                                                        \0";
	char outNames[4*MAXOUT+1] = "OUT                                                             \0";
//               "        \0";
	char name[9]="Knaster \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 1;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void module_Delay(int id) {

	static float delaymem[SAMPLERATE]; // 1s of delay memory
	static int storepointer, playbackpointer;
	static float storepointerf;
	float step;

	if (id==-1) {
		storepointerf=0.0;
		return;
	}

	step = ((AIN2+1.0f)/2.0f);
	if (step>1.0) step = 1.0;
	if (step<0.0) step = 0.01;
	storepointerf += step;
	if (storepointerf > SAMPLERATEF) storepointerf-=SAMPLERATEF;
	storepointer = (int)(storepointerf); 


	delaymem[storepointer] = AIN0;

	playbackpointer = storepointer - ((AIN1+1) * SAMPLERATE/2);
	if (playbackpointer < 0) playbackpointer += SAMPLERATE;

	AOUT0 = delaymem[playbackpointer];
	return;
}

void regModule_Delay(int id) {
        moduleRegistry[id] = module_Delay;
        module_Delay(-1); // init

//                                  "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
        char inNames[4*MAXIN+1]   = "IN  TIMESLOW                                                    \0";
        char outNames[4*MAXOUT+1] = "OUT                                                             \0";
//                   "        \0";
        char name[9]="Delay   \0";

        modIns[id]     = 3;
        modOuts[id]    = 1;

        copymodstrings(id, name, inNames, outNames);
        return;
}

void module_Noise(int id) {
	static unsigned long m_white, m_seed;

	if (id==-1) {
		srand(time(NULL));
		m_white = 0;
		m_seed = time(NULL);
		return;
	}
	
	// Uniform noise 
	AOUT0=((AIN0+1.0)/2.0)*(float)(rand()/(float)(RAND_MAX/2.0));

	// White noise
	m_seed   = (m_seed * 196314165) + 907633515;
	m_white  = m_seed >> 9; 
	m_white |= 0x40000000; 
	AOUT1 = ((*(float*)&m_white)-3.0f)*((AIN1+1.0)/2.0); 
	
	return;
}

void regModule_Noise(int id) {
	moduleRegistry[id] = module_Noise;
	module_Noise(-1); // init

//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "LEVULEVW                                                        \0";
	char outNames[4*MAXOUT+1] = "UNI WHTE                                                        \0";
//               "        \0";
	char name[9]="Noise   \0";
	
	modIns[id]     = 2;
	modOuts[id]    = 2;
	
	copymodstrings(id, name, inNames, outNames);
	return;

}

void module_WavetableOsc1(int id) {
	static int switcher=0;
	static unsigned char lastnote=0;
	static float toneFreq=20.1;
	static float phase;
	static float phasesin1, phasesin2;
	float freq;
	float temp;
	unsigned char lowbit;

	if (id == -1) { // init statics
		switcher = 0;
		lastnote = 0;
		toneFreq=20.1;
		phasesin1 = phasesin2 = phase = 0.0;
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
	
	phasesin1 = phasesin1 + (8192.0/SAMPLERATEF * (freq+AIN0*5.0) );
	phasesin2 = phasesin2 + (8192.0/SAMPLERATEF * (freq+AIN1*5.0) );
	if (phasesin1 > 8192.0) {
		phasesin1 = phasesin1 - 8192.0;
		if (AIN2 > 0) // sync on
			phasesin2=0;
	}
	if (phasesin2 > 8192.0) {
		phasesin2 = phasesin2 - 8192.0;
	}
	AOUT2 = sinustable[(int)(phasesin1)];
	AOUT3 = sinustable[(int)(phasesin2)];
	
	return;
}

void regModule_WavetableOsc1(int id) {
	int i;
	moduleRegistry[id] = module_WavetableOsc1;
	module_WavetableOsc1(-1); // init
	for (i=0;i<2048;i++)
		wavetable[i][0] = -1.0;
	for (i=2049;i<4096;i++)
		wavetable[i][0] = 1.0;
	for (i=0;i<4096;i++)
		wavetable[i][1] = (i/1024.0)-1.0;


//                              "0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  \0"
	char inNames[4*MAXIN+1]   = "TUN1TUN2SYNC                                                    \0";
	char outNames[4*MAXOUT+1] = "OUT 8BITSIN SINS                                                \0";
//               "        \0";
	char name[9]="WaveTab1\0";
	
	modIns[id]     = 3;
	modOuts[id]    = 4;
	
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
	char name[9]="WaveTab2\0";
	
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
/*
	//regModule_Oscilator1(0);
	regModule_Output(0);
	regModule_Oscilator1(1);
	regModule_Oscilator2(2);
	regModule_WavetableOsc1(3);
	regModule_Filter1(4);
	regModule_Filter2(5);
	regModule_ADSR1(6);
	regModule_ADSR2(7);
	regModule_WTCrunch(8);
	regModule_LFO1(9);
	regModule_LFO2(10);
	regModule_SampleAndHold(11);
	regModule_Smoothie(12);
	regModule_Gain(13);
	regModule_Knaster(14);
	regModule_Noise(15);
	regModule_Delay(16);
	regModule_BD(17);
	regModule_Derive(18);
	regModule_Bwlpf(19);
	regModule_Sequencer(20);
	
	numberOfModules=21;
*/
	return;
}
