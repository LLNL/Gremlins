#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <syscall.h>
#include <errno.h>
#include <papi.h>
#include <signal.h>

#ifdef __cplusplu
extern "C" long long int identity(long long int n_) {

	return n_;

}
#else
long long int identity(long long int n_) {

	return n_;

}
#endif

