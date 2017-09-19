/*
 * main.c
 *
 *  Created on: Sep 19, 2017
 *      Author: student
 */

#include "io.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

void * test(void * arg);
void * dist(void * arg);

int set_cpu(int cpu_number) {
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);
	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void timespec_add_us(struct timespec *t, long us) {
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us * 1000;
	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000) {
		t->tv_nsec = t->tv_nsec - 1000000000;
		t->tv_sec += 1;
	}
}

int main() {
	int i, args[3] = { 1, 2, 3 };
	pthread_t threads[3], dists[10];
	io_init();
	for (i = 0; i < 3; i++) {
		pthread_create(&threads[i], NULL, test, (void*) &args[i]);
	}
	for (i = 0; i < 10; i++) {
		pthread_create(&dists[i], NULL, dist, NULL);
	}
	for (i = 0; i < 3; i++) {
		pthread_join(threads[i], NULL);
	}
	for (i = 0; i < 10; i++) {
		pthread_join(dists[i], NULL);
	}
	return 0;
}

void * test(void * arg) {
	int channel = *((int *) (arg));
	set_cpu(1);
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	timespec_add_us(&t, 100);

	while (1) {
		if (!io_read(channel)) {
			io_write(channel, 0);
			usleep(5);
			io_write(channel, 1);
		}
		//clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL);
		timespec_add_us(&t, 100);
	}
	pthread_exit(NULL);
}

void * dist(void * arg) {
	set_cpu(1);
	int i = 5;
	while (1) {
		i = 124 * i;
		i = i - 615;
	}
	pthread_exit(NULL);
}

