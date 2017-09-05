#include <pthread.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "functions.h"


void busy_wait_delay(int seconds)
{
    int i, dummy;
    int tps = sysconf(_SC_CLK_TCK);
    clock_t start;
    struct tms exec_time;
    times(&exec_time);
    start = exec_time.tms_utime;
    while( (exec_time.tms_utime - start) < (seconds * tps)){
        for(i=0; i<1000; i++){
            dummy = i;
        }
        times(&exec_time);
    }
}
void *wait_sleep(void *arg){
    printf("Thread %i first message\n",*(int *)arg);
    sleep(5);
    printf("Thread %i second message\n",*(int *)arg);
    pthread_exit((int *) arg);
}

void *wait_busy(void *arg){
    printf("Thread %i first message\n",*(int *)arg);
    busy_wait_delay(5);
    printf("Thread %i second message\n",*(int *)arg);
    pthread_exit((int *) arg);
}

