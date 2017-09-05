/*
 * sem.c
 *
 *  Created on: Sep 5, 2017
 *      Author: student
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>


sem_t sem;
void * func(void * data) {
	sem_wait(&sem);
	printf("Thread %i was here\n", *((int *) data));
	sleep(1);
	sem_post(&sem);
	pthread_exit(NULL);
}

int main(int argc, char**argv) {
	pthread_t threads[5];
	int i;
	int id[5];
	if (sem_init(&sem, 1, 3)) {
		perror("semaphore init");
		exit(0);
	}
	for (i = 0; i < 5; i++) {
		id[i] = i;
		pthread_create(&threads[i],NULL,func,&id[i]);
	}
	for(i = 0; i < 5; i++){
		pthread_join(threads[i],NULL);
	}
	return 0;
}

