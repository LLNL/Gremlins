/*
 * utils.h
 *
 */

#ifndef	GREMLINS_UTILS_H_ 
#define GREMLINS_UTILS_H_

int get_env_int(const char *name, int *);
void get_cpuinfo_entry(int processor, char key[], char value[]); 

#endif /* GRMELINS_UTILS_H_ */
