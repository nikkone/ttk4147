#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <pthread.h>

struct pid_data {
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

struct pid_data * ptr;

int set_priority(int priority) {
	int policy;
	struct sched_param param;

	// check priority in range
	if (priority < 1 || priority > 63)
		return -1;
	// set priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = priority;
	return pthread_setschedparam(pthread_self(), policy, &param);
}
int get_priority() {
	int policy;
	struct sched_param param;
	// get priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	return param.sched_curpriority;
}

void * func(void * arg) {
	int pri = *((int *) arg);
	set_priority(pri);
	char buf[] = "Hello";
	char inc[50];
	int chanId = ConnectAttach(0, ptr->pid, 1, 0, 0);
	int status = MsgSend(chanId, buf, 6, inc, 50);
	printf("%s\n", inc);
	ConnectDetach(chanId);
	pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
	set_priority(6);
	int file = shm_open("/sharedpid", O_RDWR, S_IRWXU);
	pthread_t threads[4];
	int pris[] = {1, 2, 4, 5};
	ptr = (struct pid_data *) mmap(0, sizeof(struct pid_data), PROT_READ
			| PROT_WRITE, MAP_SHARED, file, 0);
	int i = 0;
	for (i = 0; i < 4; i++) {
		pthread_create(&threads[i], NULL, func,(void *) &pris[i]);
	}
	for (i = 0; i < 4; i++) {
		pthread_join(threads[i], NULL);
	}
	return EXIT_SUCCESS;
}
