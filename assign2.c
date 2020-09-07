/*
 * assign2.c
 *
 * Name: Kelvin Leung	
 * Student Number: V00927380
 */
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "train.h"
#include <sys/types.h>
#include <stdbool.h>

/*
 * If you uncomment the following line, some debugging
 * output will be produced.
 *
 * Be sure to comment this line out again before you submit 
 */

#define DEBUG	1 
pthread_mutex_t arrive; 
pthread_mutex_t leave;
void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);
int Eastbound = 0; 
int Westbound = 0; 
int easttid[25]; 
int westtid[25]; 
int eastindex = 0; 
int westindex = 0; 
int west = 0; 
int east = 0; 
int on_bridge = 0; 
pthread_cond_t block; 
/*
 * This function is started for each thread created by the
 * main thread.  Each thread is given a TrainInfo structure
 * that specifies information about the train the individual 
 * thread is supposed to simulate.
 */
void * Train ( void *arguments )
{
	TrainInfo	*train = (TrainInfo *)arguments;

	/* Sleep to simulate different arrival times */
	usleep (train->length*SLEEP_MULTIPLE);

	ArriveBridge (train);
	CrossBridge  (train);
	LeaveBridge  (train); 

	/* I decided that the paramter structure would be malloc'd 
	 * in the main thread, but the individual threads are responsible
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	free (train);
	return NULL;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
void ArriveBridge ( TrainInfo *train )
{
	printf ("Train %2d arrives going %s\n", train->trainId, 
		(train->direction == DIRECTION_WEST ? "West" : "East"));
	/* Your code here... */
	//trains do not overtake each other, trains cross the bridge in the order they 
	//arrive, 2 east for every west. 
	//maybe make an array to keep track of who arrives first. 
	//printf("%lu\n", pthread_self()); 
	 fflush( stdout );
	pthread_mutex_lock(&arrive); 
	
		if(train->direction == 2){
			easttid[eastindex] = train->trainId; 
			eastindex++; 
					
		}
		else{
			westtid[westindex] = train->trainId; 
			westindex++; 
		} 
	pthread_mutex_unlock(&arrive); 
	pthread_mutex_lock(&leave); 
	    bool done = false;
    while(!done){
		
	    if(((Westbound > 0) & (easttid[east] != -1))){
            if(train->trainId != easttid[east]){
                pthread_cond_wait(&block,&leave);
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}   
			else{
				on_bridge = 1; 
				done = true; 
			}
        }
		else if((Eastbound > 1) & (westtid[west] != -1)){
			if(train->trainId != westtid[west]){ 
                pthread_cond_wait(&block,&leave);  
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}   
			else{
				on_bridge = 1; 
				done = true;
			}
        }
        else if(easttid[east] == -1){
            if (train->trainId != westtid[west]){
                pthread_cond_wait(&block, &leave); 
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}      
            else{  
				on_bridge = 1; 
                done = true; 
            }
        }

        else if(westtid[west] == -1){
            if(train->trainId != easttid[east]){
                pthread_cond_wait(&block,&leave);   
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}   
            else{
                done = true; 
				on_bridge = 1; 
            }     
        }

        else if(easttid[east] != -1){
        	if(train->trainId != easttid[east]){	
                pthread_cond_wait(&block,&leave);   
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}   
			else{
				on_bridge = 1; 
				done = true; 
			}
        }
		else if(westtid[west] != -1){
        	if(train->trainId != westtid[west]){	
                pthread_cond_wait(&block,&leave); 
            }
			else if(on_bridge != 0){
				pthread_cond_wait(&block,&leave);
			}   
			else{
				on_bridge = 1; 
				done = true; 
			}
        }    
    }	
pthread_mutex_unlock(&leave);	
}

/*
 * Simulate crossing the bridge.  You shouldn't have to change this
 * function.
 */
void CrossBridge ( TrainInfo *train )
{
	printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
	
	/* 
	 * This sleep statement simulates the time it takes to 
	 * cross the bridge.  Longer trains take more time.
	 */
	usleep (train->length*SLEEP_MULTIPLE);
	printf ("Train %2d is OFF the bridge(%s)\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
}

/*
 * Add code here to make the bridge available to waiting
 * trains...
 */
void LeaveBridge ( TrainInfo *train )
{
	if(train->direction == 1 ){
		west++; 				
		Eastbound = 0; 
		if(easttid[east] != -1){
			Westbound++;
		}
	}
	else if(train->direction == 2 ){
		
		east++;
		if(westtid[west] != -1){  
			Eastbound++; 
		}
		if(Eastbound > 1){
			Westbound = 0; 
		}
	}
	else {
        printf("Something went really wrong");
    }
	on_bridge = 0;  
	pthread_cond_broadcast(&block); 
	
}

int main ( int argc, char *argv[] )
{
	int		trainCount = 0;
	char 		*filename = NULL;
	pthread_t	*tids;
	int		i;

		
	/* Parse the arguments */
	if ( argc < 2 )
	{
		printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
		printf ("\t\tfilename is input file to use (optional)\n");
		exit(0);
	}
	
	if ( argc >= 2 )
	{
		trainCount = atoi(argv[1]);
	}
	if ( argc == 3 )
	{
		filename = argv[2];
	}	
	
	initTrain(filename);
	
	/*
	 * Since the number of trains to simulate is specified on the command
	 * line, we need to malloc space to store the thread ids of each train
	 * thread.
	 */
	tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
	
	/*
	 * Create all the train threads pass them the information about
	 * length and direction as a TrainInfo structure
	 */
	for(int i = 0; i < 25; i++ ){
		easttid[i] = -1;  
		westtid[i] = -1; 
	}
	if(pthread_mutex_init(&arrive, NULL) != 0){
		printf("first mutex failed"); 
	}
	if(pthread_mutex_init(&leave, NULL) != 0){
		printf("second mutex failed"); 
	} 
	for (i=0;i<trainCount;i++)
	{
		TrainInfo *info = createTrain();

		printf ("Train %2d headed %s length is %d\n", info->trainId,
			(info->direction == DIRECTION_WEST ? "West" : "East"),
			info->length );

		if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
		{
			printf ("Failed creation of Train.\n");
			exit(0);
		}
	}
	//close the initTrain fileinput
	
	/*
	 * This code waits for all train threads to terminate
	 */
	for (i=0;i<trainCount;i++)
	{
		pthread_join (tids[i], NULL);
	}
	
	free(tids);
	return 0;
}

