#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
