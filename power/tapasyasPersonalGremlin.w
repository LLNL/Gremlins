//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory
//
// Written by Martin Schulz et al <schulzm@llnl.gov>
// LLNL-CODE-645436
// All rights reserved.
//
// This file is part of the GREMLINS framework.
// For details, see http://scalability.llnl.gov/gremlins.
// Please read the LICENSE file for further information.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the disclaimer below.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the disclaimer (as
//       noted below) in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the LLNS/LLNL nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE
// LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

#include "msr_core.h"
#include "msr_rapl.h"
#include "msr_thermal.h"
#include "msr_misc.h"
#include "msr_turbo.h"
#include "msr_counters.h"
#include "msr_thermal.h"
#include "msr_clocks.h"

#include "utils.h"

FILE* getFileID(int rank);

static struct timeval startTime;
static int init = 0;

static struct itimerval tout_val;
static int rank;
static int size;

static char hname[1025];
static struct rapl_limit P0_1, P0_2, P0_DRAM, P1_1, P1_2, P1_DRAM;
static int watts=115; 
static int retVal = -1;
static struct rapl_limit lim; 
long dummy=0;
int cpuid, procsPerPackage;
char entry[3];


/*These need to be global as we are taking deltas*/

void printData(int i);
void printDataUninterrupted(char *foo);

{{fn foo MPI_Init}}
	{{callfn}}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	retVal = get_env_int("PROCS_PER_PACKAGE",&procsPerPackage);
	if(retVal<0){
		cpuid = sched_getcpu();
	        get_cpuinfo_entry(cpuid,"siblings",entry);
		procsPerPackage = atoi(entry); //value of siblings is stored as procsPerPackage
		fprintf(stderr,"PROCS_PER_PACKAGE not set! Assuming %d processor per package. Set environment vaiable!\n",procsPerPackage);
        }

	if(rank % procsPerPackage == 0)
	{
		FILE *writeFile = getFileID(rank);
		gethostname(hname, 1024);	
		fprintf(writeFile, "Hostname: %s\n", hname);
		
		tout_val.it_interval.tv_sec = 30;
        	tout_val.it_interval.tv_usec = 0;
        	tout_val.it_value.tv_sec = 0;
        	tout_val.it_value.tv_usec = 0;

	
		init_msr();	

		retVal = get_env_int("POWER_CAP", &watts);
		if(retVal==-1) {
			fprintf(writeFile, "No power cap specified. Using default of 115W per socket\n");
		}

		if(retVal ==0){
			fprintf(writeFile, "Power cap specified. Setting limit1 on both sockets to %d W for minimal time window\n", watts);
		
			lim.watts = watts;
          		lim.seconds = 0.0009766;
                	lim.bits = 0;
          		set_rapl_limit(0, &lim, NULL, NULL);
          		set_rapl_limit(1, &lim, NULL, NULL);
		}
	
		get_rapl_limit(0,&P0_1, &P0_2, &P0_DRAM);
		get_rapl_limit(1, &P1_1, &P1_2, &P1_DRAM);
		
		fprintf(writeFile, "PKG0, Limit1\n");
		dump_rapl_limit(&P0_1, writeFile);

		fprintf(writeFile, "PKG1, Limit1\n");
		dump_rapl_limit(&P1_1, writeFile);

		fclose(writeFile);
	
		//Initial Dump
		char str[5]="Init";
		printDataUninterrupted(str);       
	
		setitimer(ITIMER_REAL, &tout_val, 0);
		signal(SIGALRM, printData);
	}

	PMPI_Barrier(MPI_COMM_WORLD);

{{endfn}}


{{fn foo MPI_Finalize}}

	PMPI_Barrier(MPI_COMM_WORLD);

	if(rank % procsPerPackage == 0)
	{
		tout_val.it_interval.tv_sec = 0;
        	tout_val.it_interval.tv_usec = 0;
        	tout_val.it_value.tv_sec = 0;
        	tout_val.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tout_val, 0);

		//Final Dump
		char str[5]="Fini";
		printDataUninterrupted(str);       
//
		finalize_msr();
		getFileID(-2);//close file
	}
	{{callfn}}
{{endfn}}


void printData(int i)
{
	signal(SIGALRM, printData);
	struct timeval currentTime;
	struct itimerval tout_val;
	

	if(!init)
	{
		init = 1;
		gettimeofday(&startTime, NULL);
	}


	FILE *writeFile = getFileID(-1);

	gettimeofday(&currentTime, NULL);
	double timeStamp = (double)(currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;


	fprintf(writeFile, "Timestamp: %3.2lf\n", timeStamp);	

	fprintf(writeFile, "CoreTemp: ");
	dump_thermal_terse(writeFile);	
	fprintf(writeFile, "\n");

	fprintf(writeFile, "Power PKG0/DRAM0/PKG1/DRAM1: ");
	dump_rapl_terse(writeFile);
	fprintf(writeFile, "\n");

	fprintf(writeFile, "ThreadFreq APERF/MPERF/TSC (16 Threads): ");
	enable_fixed_counters();
	dump_clocks_terse(writeFile);
	fprintf(writeFile, "\n");

	fprintf(writeFile, "ThreadCounters UCC/IR/URC (16 Threads): ");
 	dump_fixed_terse(writeFile);
	fprintf(writeFile, "\n");

	tout_val.it_interval.tv_sec = 0;
	tout_val.it_interval.tv_usec = 0;
	tout_val.it_value.tv_sec = 30;
	tout_val.it_value.tv_usec = 0;
	
//	fclose(writeFile);

	setitimer(ITIMER_REAL, &tout_val, 0);
	
}

void printDataUninterrupted(char *foo)
{
	struct timeval currentTime;
	if(!init)
	{
		init = 1;
		gettimeofday(&startTime, NULL);
	}
	

	FILE *writeFile = getFileID(-1);

	gettimeofday(&currentTime, NULL);
	double timeStamp = (double)(currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;
	fprintf(writeFile, "%s \n", foo);	
	fprintf(writeFile, "Timestamp: %3.2lf\n", timeStamp);	

	fprintf(writeFile, "CoreTemp: ");
	dump_thermal_terse(writeFile);	
	fprintf(writeFile, "\n");

	fprintf(writeFile, "Power PKG0/DRAM0/PKG1/DRAM1: ");
	dump_rapl_terse(writeFile);
	fprintf(writeFile, "\n");

	fprintf(writeFile, "ThreadFreq APERF/MPERF/TSC (16 Threads): ");
	enable_fixed_counters();
	dump_clocks_terse(writeFile);
	fprintf(writeFile, "\n");

	fprintf(writeFile, "ThreadCounters UCC/IR/URC (16 Threads): ");
 	dump_fixed_terse(writeFile);
	fprintf(writeFile, "\n");

//	fclose(writeFile);
	
}
 
