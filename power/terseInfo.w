//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014, Lawrence Livermore National Security, LLC.
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
//
// Written by Matthias Maiterth
//////////////////////////////////////////////////////////////////////////////

/*
* Used to print per Node data (2 Sockets assumed so far)
* Set PROCS_PER_NODE 
* set INTERVAL_S  for interval between measurements in seconds.
* and INTERVAL_US for interval  between measurements in microseconds.
* One file per Node will be created at NODE_OUTPUT_PATH (see utils/utils.c)
* Relies on libmsr library functions.
*/

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
static int procsPerNode;

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
 	
	retVal = get_env_int("PROCS_PER_NODE",&procsPerNode);
	if(retVal<0){
		procsPerNode = getProcsPerNode();		
		fprintf(stderr,"PROCS_PER_NODE not set! Assuming %d processor per node. Set environment vaiable!\n",procsPerNode);
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

	if(rank % procsPerNode == 0)
	{	
		FILE *writeFile = getFileID(rank);	
		init_msr();		
		printData(0); // initial Print
		reset_tout_val();
		setitimer(ITIMER_REAL, &tout_val, 0);
		signal(SIGALRM, printData);
	}
	PMPI_Barrier(MPI_COMM_WORLD);
{{endfn}}


{{fn foo MPI_Finalize}}
	PMPI_Barrier(MPI_COMM_WORLD);
	
	if(rank % procsPerNode == 0)
	{	
		tout_val.it_interval.tv_sec = 0;
        	tout_val.it_interval.tv_usec = 0;
        	tout_val.it_value.tv_sec = 0;
        	tout_val.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tout_val, 0);
		printData(0); // final Print
		finalize_msr();
		getFileID(-2);
	}
	{{callfn}}
{{endfn}}


void printData(int i){
	
	if(!init){
		init = 1;
		gettimeofday(&startTime, NULL);
	}

	FILE *writeFile = getFileID(-1);

	struct timeval currentTime;
	gettimeofday(&currentTime,NULL);
	double timeStamp = (double)(currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;
	
	fprintf(writeFile, "Timestamp: % 3.2lf\n", timeStamp);

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
	 
	fflush(writeFile);
}

void reset_tout_val(){
	tout_val.it_interval.tv_sec = interval_s;
       	tout_val.it_interval.tv_usec = interval_us;
       	tout_val.it_value.tv_sec = interval_s;
       	tout_val.it_value.tv_usec = interval_us;
}
