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

static char hname[1025];
static struct rapl_limit Px_1, Px_2, Px_DRAM ;
static int watts=115;
static double timewindow = 0.0009766;
static int retVal = -1;
static struct rapl_limit lim; 

int get_env_int(const char *name, int *val);
void printData(int i);

{{fn foo MPI_Init}}
	{{callfn}}
	int retVal,cpuid,powerbound_global,procsPerPackage;
        char entry[3];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

        retVal = get_env_int("PROCS_PER_PACKAGE",&procsPerPackage);
        cpuid = sched_getcpu();
	if(retVal<0){
	        get_cpuinfo_entry(cpuid,"siblings",entry);
		procsPerPackage = atoi(entry); //value of siblings is stored as procsPerPackage
		fprintf(stderr,"PROCS_PER_PACKAGE not set! Assuming %d processor per package. Set environment vaiable!\n",procsPerPackage);
		
        }
	if(rank %procsPerPackage == 0){
		init_msr();	
		
		int socket, nPackages;
		get_cpuinfo_entry(cpuid,"physical id", entry);	
		socket = atoi(entry);
		
		retVal = get_env_int("POWERBOUND_GLOBAL",&powerbound_global);
		if(retVal<0) {
			powerbound_global = watts* (size/procsPerPackage);
			fprintf(stdout, "POWERBOUND_GLOBAL not set. Using default of %dW per socket. Set environment variable!\n",watts);
		}

	printf("2pPP%d\n",procsPerPackage);
		nPackages = size / procsPerPackage;
		if(size%procsPerPackage)nPackages++;
		watts = powerbound_global / nPackages;
		if(retVal == 0){
			printf("Powerbound - local:%d , global:%d\n",watts,powerbound_global);	
		
			lim.watts = watts;
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

