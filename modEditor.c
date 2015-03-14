#include <stdio.h>
void printConnectsTo(int bus);
void printModOuts(void);
void printModIns(int id);
void printMods(void);
void *patchtexteditor(void *arg);

void storePatch(void);
void loadPatch(int prg);
void clearPatches(void);

volatile int statpb=-1;
volatile float statvl;
volatile int chpb=-1;
volatile int chid;
volatile int chin;

void printModOuts(void) {
	int i, ii, iii, id;
	int temp;
	
	for (i=0;i<120;i++) {
		temp=0;
		for (id=0;id<numberOfModules;id++)
			for (ii=0;ii<modIns[id];ii++)
				if (patchIn[id][ii]==i) 
					temp=1;
			if (temp==1) {
				printf("\n %03d MIDI CC          ",i);
				printConnectsTo(i);
			}
	}
	printf("\n 120 PitchBend        ");
	printConnectsTo(120);
	
	printf("\n\n");
	
	for (i=0;i<numberOfModules;i++) {
		for(ii=0;ii<modOuts[i];ii++) {
			printf(" %02d ",patchOut[i][ii]); // patchnumber
			printf(" %02d ",i); // id
			for(iii=0;iii<9;iii++)
				putchar(modName[i][iii]);
			putchar(' ');
			for(iii=0;iii<4;iii++)
				putchar(modOutsName[i][ii*4+iii]);
			printConnectsTo(patchOut[i][ii]);
			printf("\n");
		}
	}
	printf("\n 995 Gate\n 996 Note\n 997 Clear\n 998 Load\n 999 Save\n 1000+ Set static value to bus\n");
}

void printConnectsTo(int bus) {
	int id, i, ii;

	for (id=0;id<numberOfModules;id++) {
		for (i=0;i<modIns[id];i++)
			if (patchIn[id][i]==bus) {
				printf(" > %02d ",id); // id
				for(ii=0;ii<9;ii++)
					putchar(modName[id][ii]);
				putchar(' ');
				for(ii=0;ii<4;ii++)
					putchar(modInsName[id][i*4+ii]);
			}
	}
}

void printModIns(int id) {
	int i, ii;
	
	for(i=0;i<9;i++) // Module name
		putchar(modName[id][i]);
	for(i=0;i<modIns[id];i++) {
		printf("\n %02d: ",i);
		for(ii=0;ii<4;ii++)
			putchar(modInsName[id][i*4+ii]);
		printf(" %d",patchIn[id][i]);
	}
	return;	
}

void printMods(void) {
	int id, i;
	
	for(id=0;id<numberOfModules;id++) {
		printf("\n %02d: ",id); // id
		for(i=0;i<9;i++)
			putchar(modName[id][i]);
	}
	return;
}

void savePatch(void) {
	int id, in;
	
	for(id=0;id<numberOfModules;id++) {
		for (in=0;in<modIns[id];in++)
			printf("patchIn[%d][%d] = %d; ", id, in, patchIn[id][in]);
		printf("patchNote[%d] = %d; ", id, patchNote[id]);
		printf("patchGate[%d] = %d; ", id, patchGate[id]);
	}
	scanf("%d",&in);
}

void patch_set(int pb, int id, int in) {
	chpb=pb;
	chid=id;
	chin=in;
	return;
}

void patch_static(int pb, float vl) {
	statpb=pb;
	statvl=vl;
	printf("patchstatic\n");
	return;
}

void editor_doparams(void) {
	if (chpb!=-1) {
		patchIn[chid][chin]=chpb;
		chpb=-1;
	}
	if (statpb!=-1) {
		patchBus[statpb][1] = patchBus[statpb][0] = statvl;
		statpb=-1;
		printf("statpbchange\n");
	}
	return;
}

void *patchtexteditor(void *arg) {
	int p, id, in;
	
	while(1) {
		printf("\033[2J"); // clear screen
		printf("OpenModular TextPatcher\n\n");
		printModOuts();
		
		printf("\nPB> ");
		scanf("%d",&p);
		if ( p > 799 ) {
			if (p == 997) clearPatches();
			if (p == 998) {
				printf("\nPatchNumber to load> ");
				scanf("%d",&in);
				loadPatch(in);
			}
			if (p == 999) storePatch();
			if (p == 996) {
				printMods();
				printf("\nNote bus assign\nID> ");
				scanf("%d",&id);
				if ( id > numberOfModules ) {
					printf("\n Invalid module ID, out of range.\n");
				} else {
					printf("\nBus (0=first, 1=sec, 2=no\nNB> ");
					scanf("%d",&p);
					if (!(p>2)) patchNote[id]=p;
				}
			} else if (p==995) {
				printMods();
				printf("\nID> ");
				scanf("%d",&id);
				if ( id > numberOfModules ) {
					printf("\n Invalid module ID, out of range.\n");
				} else {
					printf("\nGB> ");
					scanf("%d",&p);
					if (!(p>2)) patchGate[id]=p;
				}
			}
			if ((p > 999) && (p < 1121)) {
				if ((p-1000<NOPATCHBUS) && (p>1000)) {
				printf("\nSet static value to bus %03d (-100 to 100) > ", p-1000);
				scanf("%d",&id);
				//patchBus[p-1000][1] = patchBus[p-1000][0] = (float)(id)/100.0;
				patch_static(p-1000, (float)(id)/100.0);
				} else {
					printf("\n Invalid bus number.\n");
				}
			}
		}
		else if ( p > NOPATCHBUS ) {
			printf("\n Invalid patchbus, out of range\n");
		} else {
			printMods();
			printf("\nID> ");
			scanf("%d",&id);
			if ( id > numberOfModules ) {
				printf("\n Invalid module ID, out is of range.\n");
			} else {
				printModIns(id);
				printf("\nIN> ");
				scanf("%d",&in);
				if (in > modIns[id]) {
					printf(" Invalid module input, out is of range.\n");
				} else {
					patch_set(p, id, in);
//					patchIn[id][in]=p;
				}
			}
		}
	}
	return NULL;
}
