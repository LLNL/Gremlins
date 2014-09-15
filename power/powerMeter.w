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
#include <stdlib.h>
#include <sys/time.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

#include "utils.h"

#include "msr_core.h"
#include "msr_rapl.h"


static struct itimerval tout_val;
static int rank;
static int size;

int retVal;
static int interval_s;
static int interval_us;
static int procsPerPackage;

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
 	
	retVal = get_env_int("PROCS_PER_PACKAGE",&procsPerPackage);

	if(retVal<0){
		char entry[3];
		int cpuid = sched_getcpu();
	        get_cpuinfo_entry(cpuid,"siblings",entry);
		procsPerPackage = atoi(entry); //value of siblings is stored as procsPerPackage
		fprintf(stderr,"PROCS_PER_PACKAGE not set! Assuming %d processor per package. Set environment vaiable!\n",procsPerPackage);
        }
	
	retVal = get_env_int("INTERVAL_S",&interval_s);
	if(retVal<0){
		interval_s = 0;
		fprintf(stderr,"INTERVAL_S not set! Set environment vaiable!\n");
	}	
	retVal = get_env_int("INTERVAL_US",&interval_us);
	if(retVal<0){
		interval_us  = 500000; //set default to every 0.5sec
		fprintf(stderr,"INTERVAL_US not set! Default time: %duSec; Set environment vaiable!\n",interval_us);
	}	
	
	if(rank % procsPerPackage == 0)
	{	
		FILE *writeFile = getFileID(rank);	
		init_msr();		
		reset_tout_val();
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
		finalize_msr();
		getFileID(-2);
	}
	{{callfn}}
{{endfn}}


void printData(int i){
//	signal(SIGALRM, printData);
	
	if(!init){
		init = 1;
		gettimeofday(&startTime, NULL);
	}
///////////
	FILE *writeFile = getFileID(-1);
///////////	

	struct rapl_data r1,r2;
	
	r1.flags=0;
	r2.flags=0;
	
	struct timeval currentTime;
	gettimeofday(&currentTime,NULL);

	double timeStamp = (double)(currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;
	
	read_rapl_data(0,&r1);
	read_rapl_data(1,&r2);
	
	fprintf(writeFile, "Power@%3.2lf PKG0/DRAM0/PKG1/DRAM1: %8.4lf %8.4lf %8.4lf %8.4lf\n",timeStamp, r1.pkg_watts, r1.dram_watts, r2.pkg_watts, r2.dram_watts); 
	
	fflush(writeFile);
	
	reset_tout_val();		
	setitimer(ITIMER_REAL, &tout_val, 0);
}

void reset_tout_val(){
	tout_val.it_interval.tv_sec = 0;
       	tout_val.it_interval.tv_usec = 0;
       	tout_val.it_value.tv_sec = interval_s;
       	tout_val.it_value.tv_usec = interval_us;
}
