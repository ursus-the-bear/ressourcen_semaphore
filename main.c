#include <msp430.h> 
#include "interrupt.h"
#include "scheduler.h"
#include "semaphore.h"
#include "functions.h"
/*
 * main.c
 */
int main (void) {

	// preamble
	setupLEDs ();
	setupTimer ();
	setupButton ();
	setupSemaphoreLED(&blinkGreen);
	setupScheduler ();

	// start our control thread
	schedulerStartThread (&blinkGreen);

	// ok, now enable the interrupts for everything to just magically start
	__enable_interrupt();

	// now just do nothing so the program can run for ever...
	while (1==1) {

		// currently do nothing
		// just loafing around
		// don't like this - am hoging the CPU

	}
}
