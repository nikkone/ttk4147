#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

dispatch_t *dpp;
resmgr_attr_t resmgr_attr;
dispatch_context_t *ctp;
resmgr_connect_funcs_t connect_funcs;
resmgr_io_funcs_t io_funcs;
iofunc_attr_t io_attr;

void * count_func(void * arg);
int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

char buf[100] = "Hello World\n";
int counter = 0;
int countflag = 0;

pthread_mutex_t mBuf;
pthread_mutex_t mCountFlag;
pthread_mutex_t mCount;

void error(char *s) {
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");
	pthread_mutex_init(&mBuf, NULL);
	pthread_mutex_init(&mCountFlag, NULL);
	pthread_mutex_init(&mCount, NULL);
	pthread_t cnt_thr;
	pthread_create(&cnt_thr, NULL, count_func, NULL);
	// create dispatch.
	if (!(dpp = dispatch_create()))
		error("Create dispatch");

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS,
			&io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0,
			&connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);

	// wait forever, handling messages.
	while (1) {
		if (!(ctp = dispatch_block(ctp)))
			error("Dispatch block");
		dispatch_handler(ctp);
	}

	pthread_join(cnt_thr, NULL);
	pthread_mutex_destroy(&mBuf);
	pthread_mutex_destroy(&mCountFlag);
	pthread_mutex_destroy(&mCount);
	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb) {

	if (!ocb->offset) {
		// set data to return
		char locBuf[100] = { 0 };
		pthread_mutex_lock(&mCount);
		sprintf(locBuf, "%i\n", counter);
		pthread_mutex_unlock(&mCount);

		pthread_mutex_lock(&mBuf);
		strncpy(buf, locBuf, 100);
		SETIOV(ctp->iov, buf, 100);
		_IO_SET_READ_NBYTES(ctp, 100);
		pthread_mutex_unlock(&mBuf);

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		return (_RESMGR_NPARTS(1));
	} else {
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb) {
	printf("Test Write\n");
	char locBuf[100] = { 0 };

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);
	resmgr_msgread(ctp, locBuf, msg->i.nbytes, sizeof(msg->i));
	locBuf[msg->i.nbytes] = '\0';

	if (!strncmp(locBuf, "up", 2)) {
		pthread_mutex_lock(&mCountFlag);
		countflag = 1;
		pthread_mutex_unlock(&mCountFlag);
	}
	if (!strncmp(locBuf, "down", 4)) {
		pthread_mutex_lock(&mCountFlag);
		countflag = -1;
		pthread_mutex_unlock(&mCountFlag);
	}
	if (!strncmp(locBuf, "stop", 4)) {
		pthread_mutex_lock(&mCountFlag);
		countflag = 0;
		pthread_mutex_unlock(&mCountFlag);
	}

	return (_RESMGR_NPARTS (0));
}

void * count_func(void * arg) {
	printf("Test Count\n");
	while (1) {
		pthread_mutex_lock(&mCountFlag);
		switch (countflag) {
		case -1:
			pthread_mutex_lock(&mCount);
			counter--;
			pthread_mutex_unlock(&mCount);
			break;
		case 0:
			break;
		case 1:
			pthread_mutex_lock(&mCount);
			counter++;
			pthread_mutex_unlock(&mCount);
			break;
		default:
			printf("Error\n");
		}
		pthread_mutex_unlock(&mCountFlag);
		delay(100);
	}
	pthread_exit(NULL);
}
