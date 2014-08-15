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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "utils.h"

int get_env_int(const char *name, int *val){
        char *str=NULL;
        str = getenv(name);
        if(str == NULL){
          *val = -1;
          return -1;
        }
        *val = (int) strtol(str, (char **)NULL, 0);
        return 0;
}

void get_cpuinfo_entry(int processor, char key[], char value[]){
        int i;
        FILE *fp;
        int keylen = strlen(key);
        char buf[1024];
        char *tok;
        fp = fopen("/proc/cpuinfo", "r");
        for( i = 0; i <= processor; i++){
                do{
                        fgets(buf, sizeof(buf), fp);
                }while(strncmp(key, buf, keylen) != 0);
        }
        tok = strtok(buf,":");
        tok = strtok(NULL,":"); // only works because the lines are in the format "description : value"
        strcpy(value,tok);
        fclose(fp);
}


/*
 * getfileID(int rank):
 * get a common file pointer for each rank
 * - to aquire a filepointer call with using rank (a new fp will be returned or if another gremlin created it that one will be returned)
 * - to return the previously aquired filepoint, pass -1 
 * - to close that filepointer use -2. If others are still using the pointer it will not be close but flushed. 
 */
FILE *getFileID(int rank){
	static FILE* writeFile;
	static int refcount = 0;

	switch(rank){
		case -1: 	//getID
		{
			if(writeFile == NULL){
				printf("File was not opened or is closed!\n");
			}
			return writeFile;
			break;
		}
	 	case -2:	//close
		{
			if(writeFile!=NULL){
				refcount--;
				fflush(writeFile);
				if(refcount==0){
					fclose(writeFile);
					writeFile=NULL;
				}
			}
			return writeFile;
			break;
		}
		default:	// rank>=0 open
		{
			refcount++;
			if(writeFile != NULL){
				return writeFile; 
			}
			char *filePath=NULL;
			char hname[1025];
		
		        gethostname(hname, 1024);
		
		        filePath = getenv("NODE_OUTPUT_PATH");
		
		        if (filePath != NULL){
		        	char fileName[4096];
		        	sprintf(fileName, "%s%s%s%d%s", filePath, hname,"_",rank, "_gremlin.out");
		        	writeFile = fopen(fileName, "a");
		
		        	if (writeFile == NULL){
		        	fprintf(stderr, "Failure in opening file for %s. Errno: %d. We are just going to use stdout as our output now\n", hname, errno);
		        		if (errno == ENOENT){
		                		fprintf(stderr, "It appears as though the directory path to the file does not exist (%s). Try ensuring it exists, then try again.\n", filePath);
		                	}
		                	writeFile = stdout;
		         	}
		        } else {
		        	fprintf(stderr, "It appears as though the directory to dump per-node power readings isn't set. Please set the environment variable NODE_OUTPUT_PATH to be the correct path. Now we are just going to use stdout as our output\n");
		                writeFile = stdout;
		        }
		
			return writeFile;
			break;		
		}
	}

}
