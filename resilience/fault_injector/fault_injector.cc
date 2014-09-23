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
//////////////////////////////////////////////////////////////////////////////
/*
 * fault_injector.cc
 *
 *  Created on: Sep 20, 2013
 *      Author: Ignacio Laguna
 *     Contact: ilaguna@llnl.gov
 */

#include "fault_injector.h"

#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include <iostream>

using namespace std;

#define NANO_SEC 1000000000.0

/*
 * ----------------------------------------------------------------------------
 * File-scope global variables
 * ----------------------------------------------------------------------------
 */
static long int _faultsCounter_ = 0;
static struct timeval _gtime_; //global time
static double _faultRate_ = 0; // errors/hour
static double _runtime_;
static bool _printNextInjectionTime_ = false;
pthread_t _faultyThreadID_;

/*
 * ----------------------------------------------------------------------------
 * Local helper functions
 * ----------------------------------------------------------------------------
 */
void * GREMLIN_faultInjectorThread_(void *args);
void GREMLIN_initFIEngine_();
void GREMLIN_getTimeFlag_();
void GREMLIN_setSleepTime_(struct timespec *requested_time);
void GREMLIN_setFault_();
void GREMLIN_unsetFault_();

void * GREMLIN_faultInjectorThread_(void *args) {
	struct timespec requested_time;
	struct timespec remaining;

	while(true) {
		(_faultsCounter_ != 0) ? GREMLIN_setFault_() : GREMLIN_unsetFault_();
		_faultsCounter_++;
		GREMLIN_setSleepTime_(&requested_time);

		if (_printNextInjectionTime_)
			cout << "[Next fault in " << requested_time.tv_sec << " sec] ";

		int ret = nanosleep (&requested_time, &remaining);
		if (ret == -1) {
			cout << "Error in nanosleep: " << ret << endl;
			exit(EXIT_FAILURE);
		}
	}

	pthread_exit(NULL);
}

void GREMLIN_setFault_()
{
	kill(getpid(), SIGUSR1);
}

void GREMLIN_unsetFault_()
{
	kill(getpid(), SIGUSR2);
}

// Exponential distribution
static inline
double GREMLIN__expo_(double lambda) {
	double ret = -log(random()/(double)RAND_MAX) / lambda;
	//cout << "\tWait time: " << ret << endl;
	return ret;
}

void GREMLIN_setSleepTime_(struct timespec *requested_time) {

	/* FIXME
	 * For the demo only:
	 * Start first injection in 15 seconds.
	 */
	if (_faultsCounter_ == 1) {
		requested_time->tv_sec = 15;
		requested_time->tv_nsec = 0;
		return;
	}

	double waitTimeHours = GREMLIN__expo_(_faultRate_);
	double totalTime = waitTimeHours * 60.0 * 60.0; // in seconds
	long int timeSec = (long int)(totalTime); // only seconds
	long int timeNano = (long int)((totalTime - (double)timeSec) * NANO_SEC);
	requested_time->tv_sec = timeSec;
	requested_time->tv_nsec = timeNano;
}

void GREMLIN_initFIEngine_() {
	/*
	 * Only crate fault-injection thread if error rate is > 0
	 */
	if (_faultRate_ > 0)
	{
		int ret = pthread_create(&_faultyThreadID_,
				NULL,
				GREMLIN_faultInjectorThread_,
				NULL);
		if (ret != 0) {
			cout << "Error creating fault-injector thread!" << endl;
			exit(EXIT_FAILURE);
		}
	}
}

void GREMLIN_getTimeFlag_()
{
	const char *buff = getenv("PRINT_INJECTION_TIME");
	if (buff != NULL) {
		_printNextInjectionTime_ = true;
	}
}

void GREMLIN_startTimer_()
{
	srandom(time(NULL) + getpid());
	gettimeofday(&_gtime_, NULL);
}

void GREMLIN_stopTimer_()
{
	struct timeval newtime;
	gettimeofday(&newtime, NULL);

    // Get difference in time
    double time1 = (double)(_gtime_.tv_sec) + (double)(_gtime_.tv_usec)/(1.0e6);
    double time2 = (double)(newtime.tv_sec) + (double)(newtime.tv_usec)/(1.0e6);
    _runtime_ = time2 - time1;
}

void GREMLIN_printRuntime_()
{
	printf("\nRUNTIME (sec): %f\n", _runtime_);
}

void GREMLIN_getFaultRate_()
{
	const char *buff = getenv("FAULT_RATE");
	if (buff != NULL) {
		_faultRate_ = atof(buff);
	} else {
		cerr << "Variable FAULT_RATE was not set!" << endl;
		exit(EXIT_FAILURE);
	}

	if (_faultRate_ > 0) {
		cout << "----------------------------------------------" << endl;
		cout << "Injecting at " << _faultRate_ << " errors/hour" << endl;
		cout << "----------------------------------------------\n" << endl;
	}
}


void GREMLIN_initExperiment_()
{
	GREMLIN_startTimer_();
	GREMLIN_getFaultRate_();
	GREMLIN_getTimeFlag_();
	GREMLIN_initFIEngine_();
}
extern "C" void GREMLIN_initExperiment(){
	GREMLIN_initExperiment_();
}


void GREMLIN_endExperiment_()
{
	GREMLIN_stopTimer_();
	GREMLIN_printRuntime_();
}
extern "C" void GREMLIN_endExperiment(){
	GREMLIN_endExperiment_();
}
