// OpenModular
// OpenModularVarsM.h
//
// The OpenModular data for each module
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2
// Daniel Skaborn

#define MAXMODS		16
#define MAXIN		16
#define MAXOUT		16
#define NOPATCHBUS 256

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
#define OUTL	NOPATCHBUS-2
#define OUTR	NOPATCHBUS-3


// These are the 'live' bus signals
//volatile unsigned char	ctrlPatchBus[NOCTRLPATCHBUS];
volatile float 			patchBus[NOPATCHBUS][2]; // for every sample step last index is toggles
volatile float			pitchBend;
volatile int			pitchBendRaw;
volatile unsigned char	gate[3];
volatile unsigned char	note[3];
volatile int			togglerIn, togglerOut;

// These defines the patch of the modular
volatile int	patchIn[MAXMODS][MAXIN];
volatile int	patchOut[MAXMODS][MAXOUT];
/*
volatile int	patchCtrlIn[MAXMODS][MAXCTRLIN];
volatile int	patchCtrlOut[MAXMODS][MAXCTRLOUT];
volatile int	patchAudioIn[MAXMODS][MAXAUDIIN];
volatile int	patchAudioOut[MAXMODS][MAXAUDIOUT];
*/
volatile int	patchGate[MAXMODS];
volatile int	patchNote[MAXMODS];

// Modules information fields
typedef void (*modulefunc_t)(int id);
volatile modulefunc_t	moduleRegistry[MAXMODS];
volatile char 	modName[MAXMODS][9];
volatile unsigned char	modIns[MAXMODS];
volatile unsigned char	modOuts[MAXMODS];	
volatile unsigned char	modInsName[MAXMODS][4*MAXIN+1];
volatile unsigned char	modOutsName[MAXMODS][4*MAXOUT+1];

volatile int 	numberOfModules;

float noteToFreqLUT[128];

void makeNoteToFreqLUT(float masterTune) {
	int i;
	float tempFreq;
	
	tempFreq=440.0+(masterTune*2.0);
	
	for (i=69;i<128;i++) {
		noteToFreqLUT[i]=tempFreq;
		tempFreq*=1.0594631;
	}
	tempFreq=440.0+(masterTune*2.0);
	for (i=68;i>0;i--) {
		tempFreq/=1.0594631;
		noteToFreqLUT[i]=tempFreq;
	}
	return;
}

