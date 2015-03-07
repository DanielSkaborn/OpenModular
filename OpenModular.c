// OpenModular
//
// OpenModular.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#include <math.h>
#include <stdint.h>


// receive MIDIchannel-1 (0 to 15)
#define CHANNEL 0
#define OPMODMFG1	0x7F
#define OPMODMFG2	0x7F
#define OPMODMFG3	0x7F

void mainOpenModular(void);

//#define TEXTEDIT
//#define TARGET_RPI


#include "OpenModularVarsM.h"
#ifdef TEXTEDIT
#include "modEditor.c"
#endif

#ifdef BAREMETAL_RPI
#include "hal_RPi.c"
#endif

#ifdef ALSA_RPI
#include "hal_RPi_alsa.c"
#endif

#ifdef ALSA
#include "hal_alsa.c"
#endif

//#include "hal_text.c"
//#include "hal_file.c"

#include "modules.c"



void sendModulesInfo(void) {
	int id,i;

	MIDIout( 0xF0 );			// SysEx
	MIDIout( OPMODMFG1 );
	MIDIout( OPMODMFG2 );
	MIDIout( OPMODMFG3 );
	MIDIout( 0x07 );			// Modules information
	MIDIout( numberOfModules );	// Number of modules
	for (id=0 ; id<numberOfModules ; id++) {
		MIDIout( id );
		for (i=0 ; i<8 ; i++)
			MIDIout( modName[id][i] );
		MIDIout( modIns[id] );
		MIDIout( modOuts[id] );
		for (i=0;i<4*modIns[id];i++)
			MIDIout( modInsName[id][i] );
		for (i=0;i<4*modOuts[id];i++)
			MIDIout( modOutsName[id][i] );
	}
	MIDIout(0xF7);
	return;
	}

void sendPatchDump(void) {
	int id, i;
	unsigned char temp;

	MIDIout( 0xF0 );				// SysEx
	MIDIout( OPMODMFG1 );
	MIDIout( OPMODMFG2 );
	MIDIout( OPMODMFG3 );
	MIDIout( 0x08 );				// Patchdump
	MIDIout( numberOfModules );		// Number of modules
	for (id=0 ; id<numberOfModules ; id++) {
		MIDIout( id );
		MIDIout( patchGate[id] );
		MIDIout( patchNote[id] );
		for (i=0;i<	MAXIN;i++) {
			temp = patchIn[id][i];
			MIDIout( temp&0x7F);
			MIDIout( (temp/0x80)&0x7F);
		}
		for (i=0;i<	MAXOUT;i++) {
			temp = patchOut[id][i];
			MIDIout( temp&0x7F);
			MIDIout( (temp/0x80)&0x7F);
		}
	}
	MIDIout( 0xF7 );
	return;
}

void parse(unsigned char inbuf){
	static int mps=0;
	static char tmp1=0, tmp2=0;

	//printf("parse 0x%02x\n",inbuf);
	switch (mps) {
		case 0: // wait for any data
			if (inbuf == (0xB0+CHANNEL)) mps=1; // CC
			if (inbuf == (0x90+CHANNEL)) mps=3; // Note on
			if (inbuf == (0x80+CHANNEL)) mps=5; // Note off
			if (inbuf == (0xC0+CHANNEL)) mps=7; // program change
			if (inbuf == (0xE0+CHANNEL)) mps=8; // pitchbend
			if (inbuf == (0xF0)) mps=10; // sysex
			break;

		case 1: // CC #
			tmp1 = inbuf;
			mps=2;
			break;
		case 2: // CC value
			if (tmp1==123) { // all notes off
				gate[0]=0;
				gate[1]=0;
			} else {
//				printf("cc %d\n",tmp1);
				patchBus[(int)(tmp1)][togglerIn]=(float)(inbuf)/64.0-1.0;
			}
			mps=0;
			break;

		case 3: // Note On
			tmp1 = inbuf; // key#
			mps=4;
			break;
		case 4: // Note On velocity
			if (inbuf==0) { // Note off
				if (tmp1==note[0]) { gate[0]=0; }
				if (tmp1==note[1]) { gate[1]=0; }
			}
			else {
				if (gate[0] != 0) {
					note[1]=tmp1;
					gate[1]=inbuf;
				} else {
					note[0] = note[1] = tmp1;
					gate[0] = gate[1] = inbuf;
				}
//				printf("N0:%d G0:%d N1:%d G1:%d\n",note[0], gate[0], note[1], gate[1]);
			}
			mps=0;
			break;

		case 5: // Notes off
			tmp1 = inbuf;
			mps=6;
			break;
		case 6: // Note off
			if (tmp1==note[0]) gate[0]=0;
			if (tmp1==note[1]) gate[1]=0;
			mps=0;
			break;

		case 7: // Program change
			//preset_program = inbuf;
			mps=0;
			break;

		case 8: // Pitchbend
			tmp1= inbuf;
			mps=9;
			break;
		case 9:
			pitchBendRaw = tmp1 + (inbuf * 0x100);
			pitchBend = (float)(pitchBendRaw-16382) / 16382.0;
			mps=0;
			break;

		case 10: // Sys-ex
			if (inbuf == OPMODMFG1) mps=11;
			else mps=0;
			break;
		case 11:
			if (inbuf == OPMODMFG2) mps=12;
			else mps=0;
			break;
		case 12:
			if (inbuf == OPMODMFG3) mps=13;
			else mps=0;
			break;
		case 13:
			mps=0;
			if (inbuf == 0x01) mps=14; // Set AudioPatchIn
			if (inbuf == 0x02) mps=17; // Set AudioPatchOut
			if (inbuf == 0x03) mps=20; // Set CtrlPatchIn
			if (inbuf == 0x04) mps=23; // Set CtrlPatchOut
			if (inbuf == 0x05) mps=26; // Request Patch Dump
			if (inbuf == 0x06) mps=27; // Request Modules Information

			break;
		case 14: // Set PatchIn
			tmp1=inbuf; // moduleID
			mps=15;
			break;
		case 15: // TODO CHANGE TO HIGHBYTE / LOWBYTE
			tmp2=inbuf; // moduleInPort
			mps=16;
			break;
		case 16:
			patchIn[(int)(tmp1)][(int)(tmp2)] = (unsigned char)(inbuf);
			mps=0;
			break;
		case 17: // Set PatchOut
			tmp1=inbuf; // moduleID
			mps=18;
			break;
		case 18:
			tmp2=inbuf; // moduleInPort
			mps=19;
			break;
		case 19:
			patchOut[(int)(tmp1)][(int)(tmp2)] = (unsigned char)(inbuf); // TODO CHANGE TO HIGH7BITS andLOW7BITS
			mps=0;
			break;
		case 26: // Request Of Patch Dump
			if (inbuf==0xF7) sendPatchDump();
			mps=0;
			break;

		case 27: // // Request Modules Information
			if (inbuf==0xF7) sendModulesInfo();
			mps=0;
			break;

		default:
			mps=0;
	}
	return;
}

void clearBusses(void) {
	int i;
	for (i=0 ; i<NOPATCHBUS ; i++) {
		patchBus[i][0]=patchBus[i][1]=0;
	}

	note[0] = 1;
	note[1] = 1;
	note[2] = 1;
	gate[0] = 0;
	gate[1] = 0;
	gate[2] = 0;
	pitchBend = 0.0;
	pitchBendRaw = 0;
	togglerIn=0;
	togglerOut=1;
	return;
}

void clearPatches(void) {
	int i,ii;

	for (i=0 ; i<MAXMODS ; i++) {
		for (ii=0 ; ii<MAXIN ; ii++)
			patchIn[i][ii] = DUMP;
		patchGate[i] = 2;
		patchNote[i] = 2;
	}
	return;
}

void mainOpenModular(void) {
	int i, ii;
	int n=120;
	unsigned char mididata;
	makeNoteToFreqLUT(0);

	clearPatches();
	clearBusses();
	
	moduleRegistration();
		
	for (i=0;i<numberOfModules;i++) {
		for(ii=0;ii<modOuts[i];ii++) {
			patchOut[i][ii]=n;
			n++;
		}
	}
	presetPatches(0);
	
#ifdef TEXTEDIT
	editor();
#endif

	while(1) { // forever loop
/*
		if( MIDIin(&mididata)==1 ) {
			parse(mididata);
		}*/
		if(AudioFIFOfull()==0) {
			// Toggle bus
			togglerIn=togglerOut;
			if (togglerIn) togglerOut=0;

			// Execute one sampletick from all modules
			for (i=0;i<numberOfModules;i++) { 
				moduleRegistry[i](i);
			}
			AudioOut();
		}
	}
	return; // never reached
}

