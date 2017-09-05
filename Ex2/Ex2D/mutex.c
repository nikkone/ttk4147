/*
 * mutex.c
 *
 *  Created on: Sep 5, 2017
 *      Author: student
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * func1(void * arg);
void * func2(void * arg);

int running = 1, var1 = 0, var2 = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char** argv){
	pthread_t thread1, thread2;
	pthread_create(&thread1,NULL,func1,NULL);
	pthread_create(&thread2,NULL,func2,NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}

void * func1(void * arg){
	while (running){
		pthread_mutex_lock(&mutex);
		var1++;
		var2 = var1;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}
void * func2(void * arg){
	int i = 0;
	for(i = 1; i <= 20; i++){
		pthread_mutex_lock(&mutex);
		printf("Number 1 is %i, number 2 is %i\n", var1, var2);
		pthread_mutex_unlock(&mutex);
		usleep(100000);
	}
	running = 0;
	pthread_exit(NULL);
}
