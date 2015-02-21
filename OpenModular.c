// OpenModular
//
// OpenModular.c 
// HardwareAbstractionLayer
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn


// receive MIDIchannel-1 (0 to 15)
#define CHANNEL 1
#define OPMODMFG1	0x7F
#define OPMODMFG2	0x7F
#define OPMODMFG3	0x7F

void mainOpenModular(void);

#include "OpenModularVarsM.h"
//#include "hal_text.c"
#include "hal_RPi.c"
#include "modules.c"

void sendModulesInfo(void) {
	int id,i;
	
	MIDIout( 0xF0 );				// SysEx
	MIDIout( OPMODMFG1 );
	MIDIout( OPMODMFG2 );
	MIDIout( OPMODMFG3 );
	MIDIout( 0x07 );				// Modules information
	MIDIout( numberOfModules );	// Number of modules
	for (id=0 ; id<numberOfModules ; id++) {
		MIDIout( id );
		for (i=0 ; i<8 ; i++)
			MIDIout( modName[id][i] );
		MIDIout( modAudIns[id] );
		MIDIout( modAudOuts[id] );
		MIDIout( modCtrlIns[id] );
		MIDIout( modCtrlOuts[id] );
		for (i=0;i<4*modAudIns[id];i++)
			MIDIout( modAudInsName[id][i] );
		for (i=0;i<4*modAudOuts[id];i++)
			MIDIout( modAudOutsName[id][i] );
		for (i=0;i<4*modCtrlIns[id];i++)
			MIDIout( modCtrlInsName[id][i] );
		for (i=0;i<4*modCtrlOuts[id];i++)
			MIDIout( modCtrlOutsName[id][i] );
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
		for (i=0;i<MAXAUDIIN;i++)
			MIDIout( patchAudioIn[id][i] );
		for (i=0;i<MAXAUDIOUT;i++)
			MIDIout( patchAudioOut[id][i] );
		for (i=0;i<	MAXCTRLIN;i++) {
			temp = patchCtrlIn[id][i];
			MIDIout( temp&0x7F);
			MIDIout( (temp/0x80)&0x7F);
		}
		for (i=0;i<	MAXCTRLIN;i++) {
			temp = patchCtrlOut[id][i];
			MIDIout( temp&0x7F);
			MIDIout( (temp/0x80)&0x7F);
		}
	}
	MIDIout( 0xF7 );
	return;
}

void parse(){
	static int mps=0;
	char tmp1=0, tmp2=0;
	
	unsigned char inbuf;
	inbuf = MIDIrcv();
	
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
				ctrlPatchBus[(int)(tmp1)]=(unsigned char)(inbuf*2);
			}
			mps=0;
			break;
			
		case 3: // Note On
			tmp1 = inbuf;
			mps=4;
			break;
		case 4: // Note On velocity
			if (inbuf==0) { // Note off
				if (tmp1==note[0]) gate[0]=0;
				if (tmp1==note[1]) gate[1]=0;
			}
			else {
				if (gate[0]!=0) {
					note[1]=tmp1;
					gate[1]=inbuf;
				} else {
					note[0]=tmp1;
					gate[0]=inbuf;
				}
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
		case 14: // Set AudioPatchIn
			tmp1=inbuf; // moduleID
			mps=15;
			break;
		case 15:
			tmp2=inbuf; // moduleAudioInPort
			mps=16;
			break;
		case 16:
			patchAudioIn[(int)(tmp1)][(int)(tmp2)] = (unsigned char)(inbuf);
			mps=0;
			break;
		case 17: // Set AudioPatchOut
			tmp1=inbuf; // moduleID
			mps=18;
			break;
		case 18:
			tmp2=inbuf; // moduleAudioInPort
			mps=19;
			break;
		case 19:
			patchAudioOut[(int)(tmp1)][(int)(tmp2)] = (unsigned char)(inbuf);
			mps=0;
			break;
		case 20: // Set CtrlPatchIn
			tmp1=inbuf; // moduleID
			mps=21;
			break;
		case 21:
			tmp2=inbuf; // moduleAudioInPort
			mps=22;
			break;
		case 22:
			patchCtrlIn[(int)(tmp1)][(int)(tmp2)] = (unsigned char)(inbuf);
			mps=0;
			break;
		case 23: // Set CtrlPatchIn
			tmp1=inbuf; // moduleID
			mps=24;
			break;
		case 24:
			tmp2=inbuf; // moduleAudioInPort
			mps=25;
			break;
		case 25:
			patchCtrlOut[(int)(tmp1)][(int)(tmp2)] = (unsigned char)inbuf;
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
	
	for (i=0 ; i<256 ; i++)
		ctrlPatchBus[i]=0;
		
	for (i=0 ; i<128 ; i++)
		audioPatchBus[i]=0;
		
	note[0] = 1;
	note[1] = 1;
	note[2] = 1;
	gate[0] = 0;
	gate[1] = 0;
	gate[2] = 0;
	pitchBend = 0.0;
	pitchBendRaw = 0;
	return;
}

void clearPatches(void) {
	int i,ii;
		
	for (i=0 ; i<MAXMODS ; i++) {
		for (ii=0 ; ii<MAXCTRLIN ; ii++)
			patchCtrlIn[i][ii] = NOCTRLPATCHBUS-1;
		for (ii=0 ; ii<MAXCTRLOUT ; ii++)
			patchCtrlOut[i][ii] = NOCTRLPATCHBUS-2;
		for (ii=0 ; ii<MAXAUDIIN ; ii++)
			patchAudioIn[i][ii] = NOAUDIOPATCHBUS-1;
		for (ii=0 ; ii<MAXAUDIOUT ; ii++)
			patchAudioOut[i][ii] = NOAUDIOPATCHBUS-2;
			
		patchGate[i] = 2;
		patchNote[i] = 2;
	}
	return;
}
	
void mainOpenModular(void) {	
	int i;
	
	moduleRegistration();
	clearPatches();
	clearBusses();
		
	presetPatches(0);
	
	while(1) { // forever loop
//		if(MIDIdataavail()) {
//			parse();
//		}
		if(AudioFIFOfull()==0) {
//			loadLED(1);
			// Execute one sampletick from all modules
			for (i=0;i<numberOfModules;i++) { 
				moduleRegistry[i](i);
			}
			AudioOut();
		} else {
//			loadLED(0);
		}
	}
	return; // never reached
}

