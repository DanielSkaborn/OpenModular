
void printConnectsTo(int bus);
void printModOuts(void);
void printModIns(int id);
void printMods(void);
void *connect(void *arg);

void printModOuts(void) {
	int i, ii, iii;
	
	printf("\n\n ID module   OUT  BUS %d\n",numberOfModules);
	printf(" ====================\n");
	for (i=0;i<numberOfModules;i++) {
		for(ii=0;ii<modOuts[i];ii++) {
			printf(" %02d ",i); // id
			for(iii=0;iii<9;iii++)
				putchar(modName[i][iii]);
			putchar(' ');
			for(iii=0;iii<4;iii++)
				putchar(modOutsName[i][ii*4+iii]);
			printf(" %d ",patchOut[i][ii]);
			printConnectsTo(patchOut[i][ii]);
			printf("\n");
		}
	}
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

void *patchtexteditor(void *arg) {
	int p, id, in;

	printf("\n\nOpenModular TextPatcher\n\n");
	
	while(1) {
		printModOuts();
		
		printf("\nPB> ");
		scanf("%d",&p);
	
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