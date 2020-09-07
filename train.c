/*
 * train.c
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "train.h"
#include <string.h>
 
/* A global to assign IDs to our trains */ 
int idNumber = 0;

/* If this value is set to 1, trains lengths
 * etc will be generated randomly.
 * 
 * If it is set to 0, the lengths etc will be
 * input from a file.
 */
int doRandom = 0;

/* The file to input train data from */
FILE *inputFile;
TrainInfo Trainlist[MAX_LENGTH]; 
/* You can assume that no more than 80 characters
 * will be on any line in the input file
 */
#define MAXLINE		80


void	initTrain ( char *filename )
{
	doRandom = 0;
	
	/* If no filename is specified, generate randomly */
	if ( !filename )
	{
		doRandom = 1;
		srandom(getpid());
	}
	else
	{
		/* remove this line and add your code here */
		
		//fopen the file and save to inputfile
		inputFile = fopen(filename,"r");
		if(!inputFile){
			printf("File opening failed");
        	
		}
		else{
			char parse[MAXLINE+1]; 
			
			int id = 0;  
			char trainlength[MAXLINE+1]; 
			int direct;
			
			while(fgets(parse, 81,inputFile) != NULL){
				Trainlist[id].trainId = id;
				parse[strlen(parse)-1] = '\0'; 
				if(parse[0] == 'e' || parse [0] == 'E'){
					direct = 2; 
				}
				else{
					direct = 1; 
				}
				Trainlist[id].direction = direct; 
				strncpy(trainlength,parse+1,81); 
				Trainlist[id].arrival = 0; 
				Trainlist[id].length = atoi(trainlength); 
				
				id++;
			}
		}
	}

}
 
/*
 * Allocate a new train structure with a new trainId, trainIds are
 * assigned consecutively, starting at 0
 *
 * Either randomly create the train structures or read them from a file
 *
 * This function malloc's space for the TrainInfo structure.  
 * The caller is responsible for freeing it.
 */
TrainInfo *createTrain ( void )
{
	TrainInfo *info = (TrainInfo *)malloc(sizeof(TrainInfo));

	/* I'm assigning the random values here in case
	 * there is a problem with the input file.  Then
	 * at least we know all the fields are initialized.
	 */	 
	info->trainId = idNumber++;
	info->arrival = 0;
	info->direction = (random() % 2 + 1);
	info->length = (random() % MAX_LENGTH) + MIN_LENGTH;

	if (!doRandom)
	{
		/* Your code here to read a line of input
		 * from the input file 
		 */
		idNumber--; 
		info->trainId = Trainlist[idNumber].trainId; 
		info->arrival = Trainlist[idNumber].arrival; 
		info->direction = Trainlist[idNumber].direction;
		info->length = Trainlist[idNumber].length; 
		idNumber++;
	}
	return info;
}


