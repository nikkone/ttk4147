/*
 * phil.c
 *
 *  Created on: Sep 5, 2017
 *      Author: student
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t forks[5];
void * func_dead(void * data);
void * func_working(void * data);
int main(int argc, char**argv) {
	pthread_t phils[5];
	int i;
	int id[5];
	for (i = 0; i < 5; i++) {
		pthread_mutex_init(&forks[i], NULL);
	}
	for (i = 0; i < 5; i++) {
		id[i] = i;
		pthread_create(&phils[i], NULL, func_working, &id[i]);
	}
	for (i = 0; i < 5; i++) {
		pthread_join(phils[i], NULL);
	}
	return 0;
}

void * func_dead(void * data) {
	int id = *((int *) data);
	printf("Philosopher %i is waiting for fork\n", id);
	pthread_mutex_lock(&forks[id]);
	sleep(1);
	pthread_mutex_lock(&forks[(id + 1) % 5]);
	printf("Philosopher %i is eating\n", id);
	pthread_mutex_unlock(&forks[(id + 1) % 5]);
	pthread_mutex_unlock(&forks[id]);
	pthread_exit(NULL);
}
void * func_working(void * data) {
	int id = *((int *) data);
	if (id < ((id + 1) % 5)) {
		printf("Philosopher %i is waiting for fork\n", id);
		pthread_mutex_lock(&forks[id]);
		sleep(1);
		pthread_mutex_lock(&forks[(id + 1) % 5]);
		printf("Philosopher %i is eating\n", id);
		pthread_mutex_unlock(&forks[(id + 1) % 5]);
		pthread_mutex_unlock(&forks[id]);
		pthread_exit(NULL);
	} else {
		printf("Philosopher %i is waiting for fork\n", id);
		pthread_mutex_lock(&forks[(id + 1) % 5]);
		sleep(1);
		pthread_mutex_lock(&forks[id]);
		printf("Philosopher %i is eating\n", id);
		pthread_mutex_unlock(&forks[id]);
		pthread_mutex_unlock(&forks[(id + 1) % 5]);
		pthread_exit(NULL);
	}
}

