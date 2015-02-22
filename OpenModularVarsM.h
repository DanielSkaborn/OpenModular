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
/*
#define MAXCTRLIN	16
#define MAXCTRLOUT	16
#define MAXAUDIIN	4
#define MAXAUDIOUT	4
#define NOCTRLPATCHBUS 256
#define NOAUDIOPATCHBUS 128
*/

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

