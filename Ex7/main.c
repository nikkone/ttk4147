/*
 * main.c
 *
 *  Created on: Sep 26, 2017
 *      Author: student
 */

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <native/task.h>
#include <native/sem.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <native/mutex.h>



#define MUTEX 1
#define TASK_A 0
#if TASK_A

RT_SEM sem;
RT_MUTEX mutex;

void test(void * arg);
void testL(void * arg);
void testM(void * arg);
void testH(void * arg);
void print_pri(RT_TASK *task, char *s);
int rt_task_sleep_ms(unsigned long delay);
void busy_wait_ms(unsigned long delay);

int main(void) {
	RT_TASK L, M, H;
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);
	rt_sem_create(&sem, "Semaphore", 1, S_PRIO);
	rt_mutex_create(&mutex, "Mutex");
	rt_task_shadow(NULL, "main", 99, T_CPU(0));

	rt_task_create(&L, "L", 0, 40, T_JOINABLE | T_CPU(0));
	rt_task_create(&M, "M", 0, 50, T_JOINABLE | T_CPU(0));
	rt_task_create(&H, "H", 0, 60, T_JOINABLE | T_CPU(0));
	rt_task_start(&L, testL, NULL);
	rt_task_start(&M, testM, NULL);
	rt_task_start(&H, testH, NULL);

	usleep(100000);
	rt_sem_broadcast(&sem);
	usleep(100000);
	rt_task_join(&L);
	rt_task_join(&M);
	rt_task_join(&H);
	rt_sem_delete(&sem);
	rt_mutex_delete(&mutex);
	return 0;
}

void test(void * arg) {
	int rank = *((int *) arg);
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("Task %i\n", rank);
	rt_sem_v(&sem);
}

void testL(void * arg) {
#if MUTEX
	rt_mutex_acquire(&mutex, TM_INFINITE);
#else
	rt_sem_p(&sem, TM_INFINITE);
#endif
	busy_wait_ms(3);
	print_pri(rt_task_self(), "\n");
#if MUTEX
	rt_mutex_release(&mutex);
#else
	rt_sem_v(&sem);
#endif

}

void testM(void * arg) {
	rt_task_sleep_ms(1);
	busy_wait_ms(5);
	print_pri(rt_task_self(), "\n");
}

void testH(void * arg) {
	rt_task_sleep_ms(2);
#if MUTEX
	rt_mutex_acquire(&mutex, TM_INFINITE);
#else
	rt_sem_p(&sem, TM_INFINITE);
#endif
	busy_wait_ms(2);
	print_pri(rt_task_self(), "\n");
#if MUTEX
	rt_mutex_release(&mutex);
#else
	rt_sem_v(&sem);
#endif

}

void print_pri(RT_TASK *task, char *s) {
	struct rt_task_info temp;
	rt_task_inquire(task, &temp);
	rt_printf("b:%i c:%i ", temp.bprio, temp.cprio);
	rt_printf(s);
}

int rt_task_sleep_ms(unsigned long delay) {
	return rt_task_sleep(1000 * 1000 * delay);
}

void busy_wait_ms(unsigned long delay) {
	unsigned long count = 0;
	while (count <= delay * 10) {
		rt_timer_spin(1000 * 100);
		count++;
	}
}
#else
RT_MUTEX mutexA, mutexB;

void testL(void * arg);
void testH(void * arg);
void print_pri(RT_TASK *task, char *s);
int rt_task_sleep_ms(unsigned long delay);
void busy_wait_ms(unsigned long delay);

int main(void) {
	RT_TASK L, H;
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	rt_mutex_create(&mutexA, "MutexA");
	rt_mutex_create(&mutexB, "MutexB");

	rt_task_shadow(NULL, "main", 99, T_CPU(0));
	rt_task_create(&L, "L", 0, 40, T_JOINABLE | T_CPU(0));
	rt_task_create(&H, "H", 0, 60, T_JOINABLE | T_CPU(0));

	rt_task_start(&L, testL, NULL);
	rt_task_start(&H, testH, NULL);

	rt_task_join(&L);
	rt_task_join(&H);

	rt_mutex_delete(&mutexA);
	rt_mutex_delete(&mutexB);
	return 0;
}


void testL(void * arg) {
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	rt_task_set_priority(rt_task_self(), 60);
	busy_wait_ms(3);
	print_pri(rt_task_self(), "L after 1st busy_wait\n");
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	busy_wait_ms(3);
	print_pri(rt_task_self(), "L after 2nd busy_wait\n");
	rt_mutex_release(&mutexB);
	rt_mutex_release(&mutexA);
	rt_task_set_priority(rt_task_self(), 40);
	busy_wait_ms(1);
	print_pri(rt_task_self(), "L after 3rd busy_wait\n");
}

void testH(void * arg) {
	rt_task_sleep_ms(1);
	rt_mutex_acquire(&mutexB, TM_INFINITE);
	busy_wait_ms(1);
	print_pri(rt_task_self(), "H after 1st busy_wait\n");
	rt_mutex_acquire(&mutexA, TM_INFINITE);
	busy_wait_ms(2);
	print_pri(rt_task_self(), "H after 2nd busy_wait\n");
	rt_mutex_release(&mutexA);
	rt_mutex_release(&mutexB);
	busy_wait_ms(1);
	print_pri(rt_task_self(), "H after 3rd busy_wait\n");
}

void print_pri(RT_TASK *task, char *s) {
	struct rt_task_info temp;
	rt_task_inquire(task, &temp);
	rt_printf("b:%i c:%i ", temp.bprio, temp.cprio);
	rt_printf(s);
}

int rt_task_sleep_ms(unsigned long delay) {
	return rt_task_sleep(1000 * 1000 * delay);
}

void busy_wait_ms(unsigned long delay) {
	unsigned long count = 0;
	while (count <= delay * 10) {
		rt_timer_spin(1000 * 100);
		count++;
	}
}
#endif
