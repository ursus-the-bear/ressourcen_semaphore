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
int interrupts_disabled_count;
//
void setupScheduler () {

	// setup your processList
	int i;
	for (i = 0; i < MAX_THREADS; i++)
		threadList [i].state = FREE;
	currThread = NO_THREAD;

	// how many interrupts have there been
	interrupts_disabled_count = 0;

}
//
void atomic_start () {

	if (0 == interrupts_disabled_count)
		__disable_interrupt();

	interrupts_disabled_count++;
}
//
void atomic_end () {

	interrupts_disabled_count--;

//	if (interrupts_disabled_count < 0) {
//		// basically you should never land here!!! you have disabled more than you have enabled!
//		interrupts_disabled_count = 0;
//	}

	if (0 == interrupts_disabled_count)
		__enable_interrupt();

}
//
void schedulerSetThreadStatus (int threadID, states_t state) {

	// didn't your parents tell you not to speak until you are spoken to?
	atomic_start ();

	if (threadID <= MAX_THREADS)
		threadList [threadID].state = state;

	// ok, what is it that you wanted
	atomic_end ();

}
//
int schedulerGetCurrThread () {

	return currThread;

}
//
void * schedulerGetCurrFunction () {

	return threadList [currThread].funcPtr;

}
// where is your next free slot
int schedulerGetFreeThread () {

	// didn't your parents tell you not to speak until you are spoken to?
	atomic_start ();

	// find the spot you want to place the thread
	int RC;
	int i;
	RC = ERROR_THREAD;
	for (i = 0; i < MAX_THREADS; i++) {
		if (threadList [i].state == FREE) {
			RC = i;
			atomic_end();
			return RC;
		}
	}

	// if we get here we haven't found a free thread
	RC = ERROR_THREAD;
	atomic_end ();
	return RC;
}

// start a thread
int schedulerStartThread (void (*funcPtr)()) {

	// do not process interrupts when I am busy
	atomic_start ();

	// get the next thread
	int threadID = schedulerGetFreeThread ();
	if (threadID == NO_THREAD) {
		atomic_end();
		return threadID;
	}

	// setup your thread array
	threadList[threadID].funcPtr = funcPtr;
	threadList[threadID].state = READY;

	// save the current state of the thread and return
	if (setjmp (threadList [threadID].context) == 0) {
		// now that everything is setup nothing left to do but go back to the caller
		atomic_end ();
		return threadID;
	}

	// this is where you land after the setjmp
	threadList [threadID].funcPtr();		// basically run the function you are passing to it

	// once you are finished you kann kill yourself
	schedulerKillThread(threadID);			// once you get here you are finished so stop
	return ERROR_THREAD;

}
// where is your next free spot
int schedulerGetNextThreadToRun () {

	// didn't your parents tell you not to speak until you are spoken to?
	atomic_start ();

	// find the next thread AFTER the current one (looping the array if need be)
	int i;
	int arrayPos;
	// which count are you at
	for (i = 0; i < MAX_THREADS; i++) {
		// find the position you need to check in the loop
		arrayPos = currThread + i; // this is where you are now
		if (arrayPos > MAX_THREADS) // you are over your array so go to the beginning of the loop -> saves you 2 loops :o)
			arrayPos =  (currThread + i - 1) - MAX_THREADS;

		// now check that position
		if (threadList [arrayPos].state == READY) {
			i = MAX_THREADS;  // now you get the next free thread, give it back and start working!
			atomic_end ();
			return arrayPos;
		}
	}

	// couldn't find a READY thread so just return ==> deadlock
	atomic_end ();

	// and return the first space you found
	if (currThread == NO_THREAD)
		return NO_THREAD;
	else
		return currThread;
}
// select the next thread to run and run it
void schedulerRunNextThread() {

	// do not process interrupts when I am busy
	atomic_start ();

	// which thread do we need to run now
	int threadID = schedulerGetNextThreadToRun();
	if (threadID == NO_THREAD) {
		atomic_end();
		return;
	}

	// now switch according to the state of the thread
	switch (threadList[threadID].state) {
	case BLOCKED:
		break ;
	case FREE:
		break ;
	case RUNNING:
		// just carry on with your running thread
		atomic_end();
		longjmp(threadList[currThread].context, 1);
		break;
	case READY:
		if(currThread == -1) {
			currThread = threadID;
			threadList[currThread].state = RUNNING;
			atomic_end();
			longjmp(threadList[currThread].context, 1);
		} else {
			if (setjmp(threadList[currThread].context) == 0) {
				// set your old thread to waiting
				if (threadList[currThread].state == RUNNING)   // This is incase the thread is BLOCKED to not set it to READY
					threadList[currThread].state = READY;

				// and start your new thread
				currThread = threadID;
				threadList[currThread].state = RUNNING;
				atomic_end();
				longjmp(threadList[currThread].context, 1);
			}
		}
	}

	// ok, now we can process interrupts again
	atomic_end ();
}
void schedulerKillThread (int threadNo) {

	// children should be seen and not heard
	atomic_start ();

	// just say this position is empty
	threadList [threadNo].state = FREE;

	// ok, what is it that you wanted
	atomic_end ();
}
