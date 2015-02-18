// OpenModular
// OpenModularVars.h
//
// The OpenModular data for each module
// 20150216
// GNU GENERAL PUBLIC LICENSE Version 2


#define MAXMODS		16
#define MAXCTRLIN	16
#define MAXCTRLOUT	16
#define MAXAUDIIN	4
#define MAXAUDIOUT	4

//((void (*)(int))) modProcessPtr[MAXMODS];

// These are the 'live' bus signals
volatile unsigned char	ctrlPatchBus[256];
volatile float 			audioPatchBus[128];
volatile float			pitchBend;
volatile int			pitchBendRaw;
volatile unsigned char	gate[2];
volatile unsigned char	note[2];

// These defines the patch of the modular
volatile int	patchCtrlIn[MAXMODS][MAXCTRLIN];
volatile int	patchCtrlOut[MAXMODS][MAXCTRLOUT];
volatile int	patchAudioIn[MAXMODS][MAXAUDIIN];
volatile int	patchAudioOut[MAXMODS][MAXAUDIOUT];
volatile int	patchGate[MAXMODS];
volatile int	patchNote[MAXMODS];

// Modules information fields
volatile unsigned char modCtrlIns[MAXMODS];
volatile unsigned char modCtrlOuts[MAXMODS];
volatile unsigned char modAudIns[MAXMODS];
volatile unsigned char modAudOuts[MAXMODS];	
volatile unsigned char modCtrlInsName[MAXMODS][4*MAXCTRLIN+1];
volatile unsigned char modCtrlOutsName[MAXMODS][4*MAXCTRLIN+1];
volatile unsigned char modAudInsName[MAXMODS][4*MAXCTRLIN+1];
volatile unsigned char modAudOutsName[MAXMODS][4*MAXCTRLIN+1];
