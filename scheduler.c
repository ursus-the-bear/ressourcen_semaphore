/*
 * scheduler.c
 *
 *  Created on: 14 Jan 2016
 *      Author: offic
 */
#include <msp430.h>
#include "scheduler.h"
#include "setjmp.h"
#include "functions.h"

thread_t threadList [MAX_THREADS];
int currThread;
int add_red_function = 0;
unsigned int interrupts_disabled_count = 0;
//
void atomic_start () {
//	if (0 == interrupts_disabled_count) {
		__disable_interrupt();
//	}
	interrupts_disabled_count++;
}
//
void atomic_end () {
//	if (1 == interrupts_disabled_count) {
		__enable_interrupt();
//	} else
	interrupts_disabled_count--;
}
// where is your next free slot
int scheduler_getFreeThread () {

	// didn't your parents tell you not to speak until you are spoken to?
	atomic_start ();

	// find the spot you want to place the thread
	int RC;
	int i;
	RC = ERROR_THREAD;
	for (i = 0; i < MAX_THREADS; i++) {
		if (threadList [i].state == FREE) {
			RC = i;
			break;
		}
	}

	// ok, what is it that you wanted
	// atomic_end ();    ==> ????

	// and return the first space you found
	return RC;
}

// start a thread
int scheduler_startThread (void (*funcPtr)(), states_t state) {

	// do not process interrupts when I am busy
	atomic_start ();

	// get the next thread
	int threadID = scheduler_getFreeThread ();
	if (threadID == NO_THREAD) return threadID;

	// setup your thread array
	threadList[threadID].funcPtr = funcPtr;
	threadList[threadID].state = state;

	// save the current state of the
	if (setjmp (threadList [threadID].context) == 0) {
		// now that everything is setup nothing left to do but go back to the caller
		atomic_end ();
		return threadID;
	} else {
		// ok, the longjmp has been called
		atomic_end ();
		threadList [threadID].funcPtr();		// basically run the function you are passing to it
		scheduler_killThread(threadID);			// once you get here you are finished so stop
		return ERROR_THREAD;
	}
}
// where is your next free spot
int scheduler_getNextThreadToRun () {

	// didn't your parents tell you not to speak until you are spoken to?
	atomic_start ();

	// find the next thread AFTER the current one (looping the array if need be)
	int RC;
	RC = ERROR_THREAD;
	int i;
	int arrayPos;
	// which count are you at
	for (i = 1; i < MAX_THREADS; i++) {
		// find the position you need to check in the loop
		arrayPos = currThread + i; // this is where you are now
		if (arrayPos > MAX_THREADS) // you are over your array so go to the beginning of the loop -> saves you 2 loops :o)
			arrayPos =  (i + currThread - 1) - MAX_THREADS;

		// now check that position
		if (threadList [arrayPos].state == WAITING) {
			RC = arrayPos;
			i = MAX_THREADS;  // now you get the next free thread, give it back and start working!
			// atomic_end ();

			// and return the first space you found
			return RC;
		}
	}

	// ok, what is it that you wanted
	atomic_end ();

	// and return the first space you found
	return RC;
}
// select the next thread to run and run it
void scheduler_runNextThread() {

	// do I need to add any red functions
	if (add_red_function > 0)
		for (; add_red_function > 0; add_red_function--) {
			if (0 == led_semaphore)
				scheduler_startThread(&redOnForTwoSeconds, WAITING);
			else
				scheduler_startThread(&redOnForTwoSeconds, BLOCKED);
			led_semaphore++;
		}

	// do not process interrupts when I am busy
	atomic_start ();

	// which thread do we need to run now
	int threadID = scheduler_getNextThreadToRun();
	if (threadID == ERROR_THREAD) {
		atomic_end();
		return;
	}

	// now switch according to the state of the thread
	switch (threadList[threadID].state) {
	case RUNNING: break ; // not sure I need this as I only return WAITING threads
	case WAITING:
		if(currThread == -1){
			currThread = threadID;
			threadList[currThread].state = RUNNING;
			longjmp(threadList[currThread].context, 1);
		}else{
			if (setjmp(threadList[currThread].context) == 0) {
				if (threadList[currThread].state == RUNNING)
					threadList[currThread].state = WAITING;
				currThread = threadID;
				threadList[currThread].state = RUNNING;
				longjmp(threadList[currThread].context, 1);
			}
		}
	}

	// ok, now we can process interrupts again
	atomic_end ();
}
void scheduler_killThread (int threadNo) {

	// children should be seen and not heard
	atomic_start ();

	// just say this position is empty
	threadList [threadNo].state = FREE;

	// ok, what is it that you wanted
	atomic_end ();

}
