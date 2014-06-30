

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
#include "msr_thermal.h"
#include "msr_misc.h"
#include "msr_turbo.h"
#include "cpuid.h"

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
static int watts=115; 
static int retVal = -1;
static struct rapl_limit lim; 

int get_env_int(const char *name, int *val);
void printData(int i);

{{fn foo MPI_Init}}
	{{callfn}}
	int retVal,cpuid,powerbound_global;
        char entry[3];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

        retVal = get_env_int("PROCS_PER_PACKAGE",&procsPerPackage);
	
	if(retVal<0){
	        cpuid = sched_getcpu();
	        get_cpuinfo_entry(cpuid,"siblings",entry);
		procsPerPackage = atoi(entry); //value of siblings is stored as procsPerPackage
		fprintf(stderr,"PROCS_PER_PACKAGE not set! Assuming %d processor per package. Set Encironemnet vaiable!\n",procsPerPackage);
		
        }

	if(rank %procsPerPackage == 0)
	{
		retVal = get_env_int("POWERBOUND_GLOBAL",&powerbound_global);
		if(retVal==-1) {
			powerbound_global = 115* size/8;
			fprintf(stdout, "No global power cap specified. Using default of 115W per socket\n");
		}

		init_msr();	
        
		int socket, nPackages;
		get_cpuinfo_entry(cpuid,"physical id", entry);	
		socket = atoi(entry);
	
		nPackages = size / procsPerPackage;
		if(size%procsPerPackage)nPackages++;
		watts = powerbound_global / nPackages;
		printf("local:%d , global:%d\n",watts,powerbound_global);	
		if(retVal == 0){
			fprintf(stdout, "Global Power cap specified. Setting limit1 on all sockets to %d W for minimal time window\n", watts);
		
			lim.watts = watts;
          		lim.seconds = 0.009766;
                	lim.bits = 0;
          		set_rapl_limit(socket, &lim, NULL, NULL);
		}
	
		get_rapl_limit(0,&Px_1, &Px_2, &Px_DRAM);
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

int get_env_int(const char *name, int *val){
 	char *str=NULL;
        str = getenv(name);
        if(str ==NULL){
          *val = -1;
          return -1;
        }
        *val = (int) strtol(str, (char **)NULL, 0);
        return 0;
}

void get_cpuinfo_entry(int processor, char key[], char value[]){
        int i;
        FILE *fp;
        int keylen = strlen(key);
        char buf[1024];
        char *tok;
        fp = fopen("/proc/cpuinfo", "r");
        for( i = 0; i <= processor; i++){
                do{
                        fgets(buf, sizeof(buf), fp);
                }while(strncmp(key, buf, keylen) != 0);
        }
        tok = strtok(buf,":");
        tok = strtok(NULL,":"); // only works because the lines are in the format "description : value"
        strcpy(value,tok);
        fclose(fp);
}
