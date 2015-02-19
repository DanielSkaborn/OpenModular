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

#define NOCTRLPATCHBUS 256
#define NOAUDIOPATCHBUS 128

// These are the 'live' bus signals
volatile unsigned char	ctrlPatchBus[NOCTRLPATCHBUS];
volatile float 			audioPatchBus[NOAUDIOPATCHBUS];
volatile float			pitchBend;
volatile int			pitchBendRaw;
volatile unsigned char	gate[3];
volatile unsigned char	note[3];

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

typedef void (*modulefunc_t)(int id);

volatile modulefunc_t 	moduleRegistry[MAXMODS];
volatile int 	numberOfModules;
