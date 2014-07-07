#include <stdio.h>
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <sched.h>

#include "msr_core.h"
#include "msr_rapl.h"

#include "utils.h"


void get_cpuinfo_entry(int processor, char key[], char value[]);

#ifndef SET_UP
static struct timeval startTime;
static int init = 0;
static int stop=0;
#endif

static struct itimerval tout_val;
static int rank;
static int size;
static int procsPerPackage;

static char hname[1025];
static struct rapl_limit Px_1, Px_2, Px_DRAM ;
static int watts=115;  //default
static double timewindow = 0.0009766; //default 
static int retVal = -1;
static struct rapl_limit lim; 

int get_env_int(const char *name, int *val);
void printData(int i);

{{fn foo MPI_Init}}
	{{callfn}}
	int retVal,cpuid;
	char entry[3];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);	
	
	retVal=get_env_int("PROCS_PER_PACKAGE", &procsPerPackage);
	cpuid = sched_getcpu();
	if(retVal<0){
		get_cpuinfo_entry(cpuid, "siblings", entry);
		procsPerPackage = atoi(entry);
		fprintf(stderr, "PROCS_PER_PACKAGE not set! Assuming %d processors per package. Set environment variable!\n", procsPerPackage);
	}
	if(rank%procsPerPackage == 0){
		init_msr(); //only needed once per MPI job or per package?
		
		int socket;
		get_cpuinfo_entry(cpuid,"physical id", entry);
		socket = atoi(entry);		
	
		retVal = get_env_int("POWERBOUND",&watts);
		if(retVal==-1){
			fprintf(stdout, "POWERBOUND not set. Using default. Set environment variable!\n");
		}else if(retVal==0){
			lim.watts=watts;
			lim.seconds = timewindow;
                	lim.bits = 0;
          		set_rapl_limit(socket, &lim, NULL, NULL);
		}
		get_rapl_limit(socket,&Px_1, &Px_2, &Px_DRAM);
		fprintf(stdout, "PKG%d, Limit1\n",socket);
		dump_rapl_limit(&Px_1);
	}	
	PMPI_Barrier(MPI_COMM_WORLD);
{{endfn}}


{{fn foo MPI_Finalize}}
	PMPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		finalize_msr;
	}
	{{callfn}}
{{endfn}}

