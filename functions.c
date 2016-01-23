/*
 * functions.c
 *
 *  Created on: 21 Jan 2016
 *      Author: offic
 */
#include <msp430.h>
#include "functions.h"
#include "interrupt.h"
#include "scheduler.h"
#include "semaphore.h"

#define GREEN_LED_ON_TIMER 1500
#define RED_LED_ON_TIMER 1500

// setup LEDs
void setupLEDs () {

	// preabmle
	WDTCTL = WDTPW + WDTHOLD; 		// stop using the watchdog

	// LED initial setup
	P1DIR |= RED_LED + GREEN_LED;	// ENABLE GREEN AND RED LED
	P1OUT &= ~GREEN_LED;			// Green off
	P1OUT &= ~RED_LED;				// Red off

}
//
void blinkGreen () {

	// block the semaphore
	semaphoreLED_P ();

	// preamble
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT &= ~RED_LED;		// Red off
	int i;

	while (1 == 1) {

		// switch green on
		iLEDonCount = 0;
		P1OUT |= GREEN_LED;
		while (iLEDonCount <= GREEN_LED_ON_TIMER){
			i = iLEDonCount;  // just do something so the while does not get removed
		}

		// switch green off
		iLEDonCount = 0;
		P1OUT &= ~GREEN_LED;
		while (iLEDonCount <= GREEN_LED_ON_TIMER){
			i = iLEDonCount;  // just do something so the while does not get removed
		}
	}

	// will never get here because of the while (1==1)
	semaphoreLED_V ();

}
// turn red on, then off again
void redOnForTwoSeconds () {

	// block the semaphore
	semaphoreLED_P ();

	// counter of number of ticks the LED was on
	iLEDonCount = 0;

	// switch red on
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT |= RED_LED;		// Red on

	int i;
	while (iLEDonCount <= GREEN_LED_ON_TIMER){
		i = iLEDonCount;  // just do something so the while does not get removed
	}

	// everything off
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT &= ~RED_LED;		// Red off

	// free the semaphore
	semaphoreLED_V ();
}
