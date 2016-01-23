/*
 * scheduler.h
 *
 *  Created on: 14 Jan 2016
 *      Author: offic
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <msp430.h>
#include <inttypes.h>
#include <setjmp.h>

// the states my thread can be in
typedef enum {
	RUNNING,
	READY,
	BLOCKED,
	FREE
} states_t;

// what info am I keeping per thread
typedef struct _thread {

	states_t state;
	void (*funcPtr)();
	jmp_buf context;

} thread_t;

#define MAX_THREADS 10
#define ERROR_THREAD -1
#define NO_THREAD -2

extern thread_t threadList [MAX_THREADS];
extern int currThread;
extern int interrupts_disabled_count;

void atomic_start ();
void atomic_end ();

int schedulerGetCurrThread ();
void schedulerSetThreadStatus (int threadID, states_t state);
int schedulerStartThread (void (*funcPtr)());
void schedulerRunNextThread ();
void schedulerKillThread (int threadNo);
void setupScheduler ();
void * schedulerGetCurrFunction ();

#endif /* SCHEDULER_H_ */
