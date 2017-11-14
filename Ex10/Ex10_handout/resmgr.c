#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "fifo.h"
#include <pthread.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

dispatch_t *dpp;
resmgr_attr_t resmgr_attr;
dispatch_context_t *ctp;
resmgr_connect_funcs_t connect_funcs;
resmgr_io_funcs_t io_funcs;
iofunc_attr_t io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

char buf[255] = { 0 };
fifo_t queue;
int rcv = -1;

pthread_mutex_t mQueue;

void error(char *s) {
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");
	init_fifo(&queue);
	pthread_mutex_init(&mQueue, NULL);
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

	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb) {
	int nonblock, status;
	if ((status = iofunc_read_verify(ctp, msg, ocb, &nonblock)) != EOK)
		return (status);

	if (!ocb->offset) {
		// set data to return
		if (!fifo_status(&queue)) {
			if (nonblock) {
				return (EAGAIN);
			} else {
				/*
				 *  The client is willing to be blocked.
				 *  Save at least the ctp->rcvid so that you can
				 *  reply to it later.
				 */
				rcv = ctp->rcvid;
				return (_RESMGR_NOREPLY);
			}
		}
		pthread_mutex_lock(&mQueue);
		fifo_rem_string(&queue, buf);
		pthread_mutex_unlock(&mQueue);
		SETIOV(ctp->iov, buf, strlen(buf));
		_IO_SET_READ_NBYTES(ctp, strlen(buf));

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
	char locBuf[255] = { 0 };

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);
	resmgr_msgread(ctp, locBuf, msg->i.nbytes, sizeof(msg->i));
	locBuf[msg->i.nbytes] = '\0';

	if (rcv == -1) {
		pthread_mutex_lock(&mQueue);
		fifo_add_string(&queue, locBuf);
		pthread_mutex_unlock(&mQueue);
	} else {
		strncpy(buf, locBuf, msg->i.nbytes);
		puts(buf);
		MsgReply(rcv, 0, buf, msg->i.nbytes);
		rcv = -1;
	}

	return (_RESMGR_NPARTS (0));
}
