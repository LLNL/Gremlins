#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

#include "msr_core.h"
#include "msr_thermal.h"


static struct itimerval tout_val;
static int rank;
static int size;

#ifndef SET_UP
static struct timeval startTime;
static int init = 0;
#endif

void reset_tout_val();
void printData(int i);

{{fn foo MPI_Init}}
        {{callfn}}
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        if(rank == 0)
        {
                init_msr();

                reset_tout_val();
                setitimer(ITIMER_REAL, &tout_val, 0);

                signal(SIGALRM, printData);
        }
        PMPI_Barrier(MPI_COMM_WORLD);
{{endfn}}


{{fn foo MPI_Finalize}}
        PMPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0)
        {
                tout_val.it_interval.tv_sec = 0;
                tout_val.it_interval.tv_usec = 0;
                tout_val.it_value.tv_sec = 0;
                tout_val.it_value.tv_usec = 0;
                setitimer(ITIMER_REAL, &tout_val, 0);
                finalize_msr();
        }
        {{callfn}}
{{endfn}}


void printData(int i){
        signal(SIGALRM, printData);

        if(!init){
                init = 1;
                gettimeofday(&startTime, NULL);
        }

        struct timeval currentTime;
        gettimeofday(&currentTime,NULL);

        double timestamp = (double)(currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;

	fprintf(stdout, "Temp@%3.2lf: ",timestamp);
	dump_thermal_terse();
	fprintf(stdout, "\n");

        reset_tout_val();
        setitimer(ITIMER_REAL, &tout_val, 0);
}

void reset_tout_val(){
        tout_val.it_interval.tv_sec = 0;
        tout_val.it_interval.tv_usec = 0;
        tout_val.it_value.tv_sec = 0;
        tout_val.it_value.tv_usec = 500000;
}

