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

static int procsPerPackage;

int get_env_int(const char *name, int *val);
void printData(int i);

{{fn foo MPI_Init}}
	{{callfn}}
	int retVal,cpuid,powerbound_global;
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
		
		retVal = get_env_int("POWER_CAP_GLOBAL",&powerbound_global);
		if(retVal<0) {
			powerbound_global = watts* (size/procsPerPackage);
			fprintf(stdout, "POWER_CAP_GLOBAL not set. Using default of %dW per socket. Set environment variable!\n",watts);
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
		dump_rapl_limit(&Px_1,stdout);

	} 
	PMPI_Barrier(MPI_COMM_WORLD);
{{endfn}}


{{fn foo MPI_Finalize}}
	PMPI_Barrier(MPI_COMM_WORLD);
	if(rank %procsPerPackage == 0)
	{
		finalize_msr;
	}
	{{callfn}}
{{endfn}}

