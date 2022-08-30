#ifndef _THREADS_H_
#define _THREADS_H_

#include <pthread.h>
#include <unistd.h>
#include "types.h"

typedef pthread_t thread_t;

typedef pthread_mutex_t mutex_t;

typedef pthread_cond_t cond_t;

static inline int thread_create(thread_t *thread, void *(*start_routine)(void*), void *arg)
{
	return pthread_create(thread, NULL, start_routine, arg);
}

static inline int thread_join(thread_t thread, void **value)
{
	return pthread_join(thread, value);
}

static inline void thread_condInit(cond_t *cond)
{
	*cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}

static inline int thread_wait(cond_t *cond, mutex_t *lock)
{
	return pthread_cond_wait(cond, (pthread_mutex_t *)lock);
}

static inline int thread_signal(cond_t *cond)
{
	return pthread_cond_signal(cond);
}

static inline void thread_exit(void *arg)
{
	pthread_exit(arg);
}

static inline void mutex_init(mutex_t *mutex)
{
	*mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
}

static inline int lock(mutex_t *mutex)
{
	return pthread_mutex_lock(mutex);
}

static inline int mutex_trylock(mutex_t *mutex)
{
	return pthread_mutex_trylock(mutex);
}

static inline int unlock(mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}

static inline void thread_sleep(u32 us)
{
	usleep(us);
}

#endif
