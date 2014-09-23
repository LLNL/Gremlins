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
// Written by Marc Casas Guix and Matthias Maiterth
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/time.h>

#include <identity.h>

#include <signal.h>

#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

#include "memory_utils.h"

MPI_Comm new_comm; 


int contaminators_id[16];

//100 nodes maximum!!
long message_times[100];

{{fn foo MPI_Init}}
	{{callfn}}
	Create_commu(MPI_COMM_WORLD);
	Contamination(MPI_COMM_WORLD);

{{endfn}}


#define swap_comm(comm) \
int comm_size, world_size;\
\
        PMPI_Comm_size(comm, &comm_size);\
        PMPI_Comm_size(MPI_COMM_WORLD, &world_size);\
\
        if(comm_size == world_size)\
                comm = new_comm


{{fn foo MPI_Comm_size MPI_Comm_rank MPI_Barrier MPI_Bcast MPI_Reduce MPI_Allreduce MPI_Abort MPI_Comm_group MPI_Comm_dup MPI_Comm_create MPI_Comm_split MPI_Alltoall MPI_Alltoallv MPI_Allgather MPI_Allgatherv MPI_Gather MPI_Gatherv MPI_Scatter MPI_Scatterv MPI_Send MPI_Recv MPI_Sendrecv MPI_Isend MPI_Irecv MPI_Send_init MPI_Recv_init MPI_Iprobe MPI_Probe MPI_Irsend MPI_Scan}}
	{{applyToType MPI_Comm swap_comm}}
	{{callfn}}

{{endfn}}

{{fn foo MPI_Finalize}}
	int my_rank, i, errno;
	PMPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	int n_conts = atoi(getenv("N_CONTS"));
	int n_cores = atoi(getenv("N_CORES"));
	
	if( my_rank % n_cores == n_cores/2 - 1){
		for( i = 0; i < n_conts; i++) {
			errno = kill(contaminators_id[i], SIGUSR2);
		}
	}
	{{callfn}}
{{endfn}}
