/* Thu Jan  2 04:17:13 NZDT 2003 */
/* command line parser thingy */
/* Checks for a - then an option letter. Sticks the parameter in */
/* optargument in the structure, use find_arg to return it */
/* By Daniel Richards <kyhwana@world-net.co.nz> */
/* This file is Public Domain. No Rights Reserved */
/* If you ever decide to use this, email me or something! */
/* TODO: Fix so it doesn't segfault if an option isn't in the option array thingy*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
int myopt(char *optarray,int argc, char **argv, argus *arglist);
char *find_arg(argus *arglist,char optlet);
*/
/* use linked lists/structures ? with an araay of varibles */
typedef struct Argustruct {
	char optlet;
	char optargument[256];
	struct Argustruct *next; /* linked list thingy */
} argus;


char *find_arg(argus *arglist,char optlet) {
	/* find optlet and stick the argument in argument */
	while (arglist->optlet) {
		printf("%c,%c,%s\n",arglist->optlet,optlet,arglist->optargument);
	
		if (arglist->optlet == optlet) {
			return arglist->optargument;
		}
		arglist = arglist->next;
		
	}
	printf("returning null\n");
	return NULL;
}
main(int argc, char *argv[]) {
	int x;
	char optarray[256];
	//char outarray[256][256];
	char *argument;
	char *argument2;
	argus *arglist;
	argument = calloc(256,1);
	argument2 = calloc(256,1);
	//memset(argument,0,256);
	//memset(argument2,0,256);
	memset(optarray,0,sizeof(optarray)); /* This is important! */
	arglist = calloc(1,sizeof(arglist));
	/* myopt takes an array of letters with :'s following if it*/
	/* requires a parameter */
	/* vvvvvvvvvvvvvvvvvvvvvvvvv*/
	strcpy(optarray,"k:f:x:pac:w:");
	printf("%s\n",optarray);

	myopt(optarray,argc,argv,arglist);
	/* Lets make find_arg find the parameter for an option letter */
	/* that we specify on the command line.*/
	/* ie "./clo -w x -x Stuff"  will stick Stuff in argument2 */
	
	argument = find_arg(arglist,'w');
	/* Is this the best way to check for required options? */
	
	if (argument == NULL) {
		printf("w not found\n");
		exit(0);
	}
	printf("argument = %s\n",argument);
	argument2 = find_arg(arglist,argument[0]);
	printf("argument2 = %s\n",argument2);
	return 0;	
}
/* This is it */
/* the structure now takes optional arguments. ie ./clo -o "optional argument" */
/* find_arg will return it */
int myopt(char *optarray,int argc, char **argv, argus *arglist) {
	int x,y,z;
	int nexta,optpcount;
	int outcount;
	char optlet;
	char optparam[10][256];
	//char optarray[256];
	
	optpcount = 0;
	outcount = 0;
	for (x = 0;x<argc;x++) {
		for (y = 0;y<strlen(argv[x]);y++) {
			printf("%c",argv[x][y]);
			if (argv[x][y] == '-') {
				printf("\n");
				optlet = argv[x][++y];
				arglist->optlet = optlet;
				//printf("optlet done\n");
				for (z = 0;z<256;z++) {
					//printf(" \"%c\"",optarray[z]);
					if (optlet == optarray[z]) {
						printf("next is :%c\n",optarray[z+1]);
						if (optarray[++z] == ':') {
							nexta = 1;
						} else { nexta = 0; }
						break;
					}
				}
				//printf("find letter function done\n");
				printf("%c:",optlet);
				//optlet = '\0';
				if (nexta) {
					if (!argv[x+1]) {
						printf("No parameter?\n");
						exit(-1);
					}
					printf("string length: %d\n",strlen(argv[x+1]));
					memset(optparam[optpcount],0,sizeof(optparam[optpcount]));
					strncpy(optparam[optpcount],argv[++x],strlen(argv[x+1]));
					
					if (optparam[optpcount][0] == '-') {
						printf("You passed an option instead of a parameter\n");
						exit(-1);
					} 
					
strncpy(arglist->optargument,optparam[optpcount],strlen(optparam[optpcount]));
					printf("option: %s\n",optparam[optpcount]);
					optpcount++;
					nexta = 0;
					printf("next with arg\n");
					if ((arglist->next = calloc(1,sizeof(arglist))) == NULL) {
						printf("calloc failed\n");
						exit(-1);
					}
					arglist = arglist->next;
					break;
				} else { /* end of nexta */
					if (argv[x+1][0] != '-') {
						memset(optparam[optpcount],0,sizeof(optparam[optpcount]));
					strncpy(optparam[optpcount],argv[++x],strlen(argv[x+1]));
						strncpy(arglist->optargument,optparam[optpcount],strlen(optparam[optpcount]));
					}
					printf("next\n"); 
					if ((arglist->next = calloc(1,sizeof(arglist))) == NULL) {
						printf("calloc failed\n");
						exit(-1);
					}
					arglist = arglist->next; 
					
				}
				
			}
		}
		printf("\n");
	}
	return 0;
}
