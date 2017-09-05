/*
 * main.c
 *
 *  Created on: Sep 5, 2017
 *      Author: student
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob = 0;

static void *func(void *arg){
	int local = 0;
	glob++;
	local++;
	printf("Thread %i Global: %i Local: %i\n",*((int *) arg), glob, local);
	pthread_exit(NULL);
}

int main(int argc, char **argv){
	pthread_t thread1, thread2;
	int int1 = 1, int2 = 2;
	pthread_create(&thread1,NULL,func,(void *) &int1);
	pthread_create(&thread2,NULL,func,(void *) &int2);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0;
}
