
void printConnectsTo(int bus);
void printModOuts(void);
void printModIns(int id);
void printMods(void);
void *patchtexteditor(void *arg);

void clearPatches(void);

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
	printf("\n\n");
	//printf("\n\n BUS ID module   OUT   > CONNECTED TO\n");
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
	printf("\n 998 Clear\n 999 Save\n1000 Note\n1001 Gate\n");
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
}

void *patchtexteditor(void *arg) {
	int p, id, in;

	printf("\n\nOpenModular TextPatcher\n\n");
	
	while(1) {
		printModOuts();
		
		printf("\nPB> ");
		scanf("%d",&p);
		if (p == 998) clearPatches();
		if (p == 999) savePatch();
		if ( p > 999 ) {
			if (p==1000) {
				printMods();
				printf("\nID> ");
				scanf("%d",&id);
				if ( id > numberOfModules ) {
					printf("\n Invalid module ID, out is of range.\n");
				} else {
					printf("\nNB> ");
					scanf("%d",&p);
					if (!(p>2)) patchNote[id]=p;
				}
			} else if (p==1001) {
				printMods();
				printf("\nID> ");
				scanf("%d",&id);
				if ( id > numberOfModules ) {
					printf("\n Invalid module ID, out is of range.\n");
				} else {
					printf("\nGB> ");
					scanf("%d",&p);
					if (!(p>2)) patchGate[id]=p;
				}
			}
		}
		else if ( p > NOPATCHBUS ) {
			printf("\n Invalid patchbus out is of range\n");
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
					patchIn[id][in]=p;
				}
			}
		}
	}
	return NULL;
}
