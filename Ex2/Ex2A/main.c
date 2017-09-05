/*
 * main.c
 *
 *  Created on: Sep 5, 2017
 *      Author: student
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int glob = 0;

int main(int argc, char**argv) {
	int local = 0;
	pid_t pid = vfork();
	if (pid == 0) {
		glob++;
		local++;
		printf("Process: %i Global: %i Local: %i\n",pid, glob,local);
	} else if (pid > 0) {
		glob++;
		glob++;
		local++;
		local++;
		printf("Process: %i Global: %i Local: %i\n",pid , glob,local);
		exit(1);
	} else {
		printf("Fail\n");
	}
	return 0;
}
