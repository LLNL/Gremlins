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

#include <mpi.h>
#include <unistd.h>

#include <sys/time.h>

#include <identity.h>

#include <signal.h>
#include <papi.h>

#include <sys/types.h>
#include <unistd.h>

#include <signal.h>
#include "memory_utils.h"

MPI_Comm new_comm; 


int contaminators_id[16];

//100 nodes maximum!!
long message_times[100];

int quit=0;

void
signal_callback_handler(int signum)
{
	quit=1;
}



//Assigns the contamination threads to the extra MPI tasks
int Contamination( MPI_Comm comm) {

	int comm_size, my_rank;
        PMPI_Comm_size( comm, &comm_size );
        PMPI_Comm_rank( comm, &my_rank );
        int n_nodes = atoi(getenv("N_NODES"));
        long long int buffer_size = atoll(getenv("BUFFER"));
	int n_conts = atoi(getenv("N_CONTS"));
	int n_cores = atoi(getenv("N_CORES"));
	int metric = atoi(getenv("METRIC"));
	int counter, jjj, lll;

        int tasks_per_node = comm_size / n_nodes;

        struct timeval start, end;

        FILE *fp; char path[1035];
	long long int packets=0, previous_packets=0;

	fprintf(stderr, "Cache/memBW interference buffer_size = %lld n_contaminators = %d\n", buffer_size, n_conts);

	if(   n_cores > 1) {
		if( my_rank % (n_cores/2) < (n_conts/2)) {
			//Wait for SIGUSR2 to terminate contamination
			signal(SIGUSR2, signal_callback_handler);
			if ( metric == 1) 
				random_access_nothreading_(buffer_size);
			else if (metric == 2)
				measurement_bandwidth_nothreading_(buffer_size);

			MPI_Finalize();
			exit(0);
        	}
	} else {
		printf("More than 1 task per socket must be mapped.\n");
	}
	return 0;
}

//Cache capacity interference
void random_access_nothreading_(long long int n_) {

	const int n_counters = 1;
	int retval, EventSet = PAPI_NULL, i;
	int Events[] = { PAPI_L3_TCM};

	long long int time_old=0, time_new=0;
	long long int Values[n_counters];
	long long int Average_values[n_counters];
	long long int average_time=0, average_time2=0;

	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if(retval<0){
	        fprintf(stderr, "PAPI_library_init = %i %i\n", retval, PAPI_VER_CURRENT);
	}
        retval = PAPI_create_eventset(&EventSet);
	if(retval<0){
	        fprintf(stderr, "PAPI_create_eventset = %i\n", retval);
	}
        for ( i = 0; i < n_counters; i++){
                retval = PAPI_add_event(EventSet, Events[i]);
		if(retval<0){
	       		fprintf(stderr, "PAPI_add_event = %i\n", retval);
		}
        }


        for( i=0; i<n_counters; i++) {
                Values[i] =  0;
        }
	
	
	int num_obs = 100000000/n_;
	if(num_obs<100)
		num_obs=100;


	fprintf(stderr, "n = %lld num_obs=%d\n", n_, num_obs);


	struct random_data* rand_states = (struct random_data*)calloc(1, sizeof(struct random_data));
	char* rand_statebufs = (char*)calloc(1, 8);
	initstate_r(random(), rand_statebufs, 8, rand_states);

        PAPI_start(EventSet);
        time_old = PAPI_get_real_usec();

	int* vec_ = (int*)malloc(sizeof(int)*n_);

	long long int k=0;	
	long long int k_;
	int r1;

	while(!quit) {
	   	for( i=0; i<num_obs; i++ ) {
	
               		for( k_=0; k_<n_; k_++) {	
				random_r(rand_states, &r1);
				vec_[r1%n_]=vec_[r1%n_]+1;
			}
		}
		if((identity(k) %  num_obs/10) == 0) {
			
			for( i=0; i < n_counters; i++){
				Values[i]=0;
			}		
			retval=PAPI_accum(EventSet, Values);
			time_new = PAPI_get_real_usec();
				
				
			fprintf(stderr, "vec_ = %d k_ = %lld BW-%lld (MB/s)  = %le \n", vec_[r1%n_], n_, n_, (double)sizeof(long long int)*(double)Values[0]/(double)(time_new-time_old));	
			time_old = time_new;	
			
			time_old = time_new;
			time_new = PAPI_get_real_usec();


               	}
		k++;

	}
}

//Mem BW interference
void measurement_bandwidth_nothreading_(long long int n_) {
	
        const int n_counters = 1;
	int retval, EventSet = PAPI_NULL, i;
	int Events[] = { PAPI_L3_TCM};

        long long int time_old=0, time_new=0;
        long long int Values[n_counters];
        long long int Average_values[n_counters];
	long long int average_time=0, average_time2=0;
	long long int k_;

	
	retval = PAPI_library_init(PAPI_VER_CURRENT);  
	fprintf(stderr, "PAPI_library_init = %i %i\n", retval, PAPI_VER_CURRENT);
	retval = PAPI_create_eventset(&EventSet);
	fprintf(stderr, "PAPI_create_eventset = %i\n", retval);
	for ( i = 0; i < n_counters; i++){
		retval = PAPI_add_event(EventSet, Events[i]);
	}


	for( i=0; i<n_counters; i++) { 
	       	Values[i] =  0;
	}


	long long int** vec_ = (long long int**)malloc(sizeof (long long int *) * n_counters );

	for ( i = 0; i < n_counters; i++ ) 
		vec_[i] = (long long int *) malloc ( sizeof (long long int) * n_ );

	
	
	PAPI_start(EventSet);
        time_old = PAPI_get_real_usec();

	k_=0;	
        while(!quit) {
   
		for( i = 0 ; i < n_counters; i++ ) {
			
			(*(volatile long long int *)(&vec_[i][identity(3736503*k_)%n_]))++;
			
		}
		
		if( (identity(k_) %  4000000*n_) == n_ ) {

                                for( i=0; i<n_counters; i++) { 
                                        Values[i] =  0;
                                }
				retval=PAPI_accum(EventSet, Values);
				time_new = PAPI_get_real_usec();
				
				fprintf(stderr, "BW (MB/s)  = %le\n", (double)sizeof(long long int)*(double)Values[0]/(double)(time_new-time_old));	
				time_old = time_new;


                }
		k_++;





	}

	PAPI_stop(EventSet, Values);
}

//An extra communicator the main application will use as MPI_Comm_World is created
int Create_commu( MPI_Comm comm) {


	MPI_Group group_world, new_group;
        int comm_size, my_rank, i=0;
        PMPI_Comm_size( comm, &comm_size );
        PMPI_Comm_rank( comm, &my_rank );
	int n_nodes = atoi(getenv("N_NODES"));
	int n_conts = atoi(getenv("N_CONTS"));
	int n_cores = atoi(getenv("N_CORES"));
	int p_id =  getpid() ; 
	MPI_Request req[n_conts];
	MPI_Status  status[n_conts]; 


	// MPI mapping socket 0 : 0 1 2 3 4 X
	// 	       socket 1 : 5 6 7 8 9 X	
	// It doesn't work for non-symmetric mappings, i. e., if different the
	// number of MPI tasks mapped in socket 0 is not the same as the one mapped
	// in socket 1. 

	int * new_set_ranks = malloc(sizeof(int)*(comm_size-n_nodes*n_conts));
	int j=0;
	
	for(i =0; i<comm_size-n_nodes*n_conts; i++)
		new_set_ranks[i] = 0;


	if( my_rank % (n_cores/2) < (n_conts/2)) {
		PMPI_Send(&p_id, 1, MPI_INT, my_rank/n_cores * n_cores + n_cores/2-1, 123, MPI_COMM_WORLD);
        }
	if ( my_rank%n_cores  == n_cores/2-1 ) {
		for( i = 0; i < comm_size; i++ ) {
			if( i % (n_cores/2) < (n_conts/2) && i/n_cores == my_rank/n_cores) {
				PMPI_Irecv(&contaminators_id[j], 1, MPI_INT, i, 123, MPI_COMM_WORLD, &req[j]);
				j++;
			}
		}
		PMPI_Waitall(n_conts, req, status);

	}
	

	
	for( i = 0; i<comm_size; i++) {
		if(n_cores > 1) {
			if( i%(n_cores/2) >= n_conts/2) {
				new_set_ranks[i-n_conts/2-n_conts/2*(i/(n_cores/2))] = i;
			}

		} else {

			new_set_ranks[i] = i;

		}
		
	}
	



        PMPI_Comm_group(comm, &group_world);
        PMPI_Group_incl(group_world, comm_size-n_nodes*n_conts, new_set_ranks, &new_group);
        PMPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

	return 0;
}


