/*
 * main.c
 *
 *  Created on: Oct 10, 2017
 *      Author: root
 */
#include <stdio.h>
#include <pthread.h>
#include "miniproject.h"
#include <stdlib.h>

void *udp_listener(void * arg);
void *pi_controller(void * arg);
void *signal_responder(void * arg);

pthread_mutex_t udp_m, y_m, signal_m; // Mutexes for UDP sending, locally stored y value and the signal from part2
struct udp_conn con;
double y = 0, u; // u always set in the pid_controller before its used, no need to initialize
int running = 1, signaled = 0;  /* Running is a flag for stopping the threads after a given time,
signaled gets set by the udp_listener and responded to and reset in signal_responder*/

int main(void) {
	pthread_t udp_thread, pi_thread, signal_thread;

	pthread_mutex_init(&udp_m, NULL);
	pthread_mutex_init(&y_m, NULL);
	pthread_mutex_init(&signal_m, NULL);

	struct timespec time;
	udp_init_client(&con, 9999, "192.168.0.1");

	pthread_create(&udp_thread, NULL, udp_listener, NULL);
	pthread_create(&pi_thread, NULL, pi_controller, NULL);
	pthread_create(&signal_thread, NULL, signal_responder, NULL);

	udp_send(&con, "START", 6);
	clock_gettime(CLOCK_REALTIME, &time);
	timespec_add_us(&time, 500000);
	clock_nanosleep(&time);
	running = 0;

	pthread_mutex_lock(&udp_m);
	udp_send(&con, "GET", 4);
	pthread_mutex_unlock(&udp_m);

	pthread_join(udp_thread, NULL);
	pthread_join(pi_thread, NULL);
	pthread_join(signal_thread, NULL);

	udp_send(&con, "STOP", 5); // Mutex not needed, only one thread from this point
	udp_close(&con);

	pthread_mutex_destroy(&udp_m);
	pthread_mutex_destroy(&y_m);
	pthread_mutex_destroy(&signal_m);

	return 0;
}

void *udp_listener(void * arg) {
	char recv_buf[50];
	int mesg_len = 0;

	while (running) {
		mesg_len = udp_receive(&con, recv_buf, 50);
		if (mesg_len > 7) { // "SIGNAL" is of length 6, bigger messages are new y values
			pthread_mutex_lock(&y_m);
			y = atof(recv_buf + 8);
			pthread_mutex_unlock(&y_m);
		} else {
			pthread_mutex_lock(&signal_m);
			signaled = 1;
			pthread_mutex_unlock(&signal_m);
		}
	}
	pthread_exit(NULL);
}

void *pi_controller(void * arg) {
	double error, integral = 0, reference = 1;
	char send_buf[50];  // This number could be decreased for memory efficiency
	long period = 2000;
	double Kp = 10, Ki = 800;
	int count_send;
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	while (running) { // Runs periodically with a period of 2000µs
		timespec_add_us(&time, period);
		pthread_mutex_lock(&udp_m);
		udp_send(&con, "GET", 4);
		pthread_mutex_unlock(&udp_m);
		pthread_mutex_lock(&y_m);
		error = reference - y;
		pthread_mutex_unlock(&y_m);
		integral += ((error * period) / 1000000);
		u = Kp * error + Ki * integral;
		count_send = sprintf(send_buf, "SET:%.3f", u);
		pthread_mutex_lock(&udp_m);
		udp_send(&con, send_buf, count_send + 1);
		pthread_mutex_unlock(&udp_m);
		clock_nanosleep(&time);
	}
	pthread_exit(NULL);
}

void *signal_responder(void * arg) {
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	long period = 7000;

	while(running){
		timespec_add_us(&time, period);
		pthread_mutex_lock(&signal_m);
		if(signaled){
			pthread_mutex_lock(&udp_m);
			udp_send(&con, "SIGNAL_ACK", 11);
			pthread_mutex_unlock(&udp_m);
			signaled = 0;
		}
		pthread_mutex_unlock(&signal_m);
		clock_nanosleep(&time);
	}
	pthread_exit(NULL);
}
