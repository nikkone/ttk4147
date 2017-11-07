#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

struct pid_data {
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

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
int main(int argc, char *argv[]) {
	int file = shm_open("/sharedpid", O_RDWR | O_CREAT, S_IRWXU);
	char buf[50];
	pthread_mutexattr_t myattr;
	set_priority(3);
	ftruncate(file, sizeof(struct pid_data));
	struct pid_data * const ptr = (struct pid_data *) mmap(0,
			sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, file,
			0);
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&ptr->pid_mutex, &myattr);
	ptr->pid = getpid();
	int chanId = ChannelCreate(/*_NTO_CHF_FIXED_PRIORITY*/0);
	struct _msg_info * info = malloc(sizeof(struct _msg_info));
	int i = 0;
	while (1) {
		printf("Priority before: %i \n", get_priority());
		int recvId = MsgReceive(chanId, buf, 50, info);
		printf("Job for Client PID: %i TID: %i Priority: %i\n", info->pid, info->tid, get_priority());
		buf[0] = 'h' + info->tid;
		MsgReply(recvId, EOK, buf, 6);
	}
	return EXIT_SUCCESS;
}
