/*
 * semaphore.c
 *
 *  Created on: 23 Jan 2016
 *      Author: offic
 */
#include "semaphore.h"
#include "scheduler.h"

#define MAX_SEMAPHORES MAX_THREADS + 1

static int semaphoreLED;					 // count of threads waiting for LED
static int threadsWaiting [MAX_SEMAPHORES]; // waiting list
static void (*backgroundFunction) ();		 // which is the function that is the "background" function
//
void setupSemaphoreLED (void (*funcPtr) ()) {

	// nothing running at the moment
	semaphoreLED = -1;

	// init your array
	int i;
	for (i = 0; i < MAX_SEMAPHORES; i++)
		threadsWaiting [i] = NO_THREAD;

	// this is the function you can interrupt
	backgroundFunction = funcPtr;

}
// why is this called P -> process????
void semaphoreLED_P () {

	// make this unbreakable
	atomic_start();

	// we need the LED
	semaphoreLED++;

	// add this function to our functionlist ===> threadsWaiting [0] is allways the currently running thread
	int runningThread = schedulerGetCurrThread ();
	threadsWaiting [semaphoreLED] = runningThread;

	// check if the LED belongs to you
	if (0 == semaphoreLED) {
		// you are the first person to who wants the LED, just run with it
	} else {
		// set any greens to blocked
		int i;
		for (i = 0; i < semaphoreLED; i++) {
			if (threadList [i].funcPtr == backgroundFunction)
				schedulerSetThreadStatus (i, BLOCKED);		// set any green functions to blocked
		}

		// is the running one green
		if (threadList [threadsWaiting [0]].funcPtr == backgroundFunction) {
			// swap the one I am adding with the one at 0
			int threadAtZero;
			threadAtZero = threadsWaiting [0];
			threadsWaiting [0] = runningThread;
			threadsWaiting [semaphoreLED] = threadAtZero;
		} else {
			schedulerSetThreadStatus (runningThread, BLOCKED);
		}

		// NB -> We are going to run another thread because this one cannot run i.e. it is blocked
		atomic_end();
		schedulerRunNextThread();
		return; // need this so I do not, by mistake, run atomic_end twice (from below)
	}

	// finished
	atomic_end();
}

// V is for Vendetta????
void semaphoreLED_V () {

	// leave me alone
	atomic_start();

	// dequeue
	int i;
	for (i = MAX_SEMAPHORES; i > 0; i--) {
		threadsWaiting [i - 1] = threadsWaiting [i];
		threadsWaiting [i] = NO_THREAD;
	}

	// the current thread no longer needs the LED
	semaphoreLED--;

	// if there are any threads waiting set the top of the list to READY
	if (threadsWaiting [0] != NO_THREAD) {
		// are we trying to start a green thread
		if (threadsWaiting [0] == backgroundFunction) {
			// shit, this one is the background thread - move it to the end of the queue
			void * ptr = threadsWaiting [semaphoreLED];
			threadsWaiting [semaphoreLED] = threadsWaiting [0];
			threadsWaiting [0] = ptr;
		}
		schedulerSetThreadStatus (threadsWaiting [0], READY);
	}

	// finished
	atomic_end();
}
