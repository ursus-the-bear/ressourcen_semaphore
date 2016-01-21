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

#define LED_ON_TIMER 1500

int led_semaphore;

// setup LEDs
void setupLEDs () {

	// preabmle
	WDTCTL = WDTPW + WDTHOLD; 		// stop using the watchdog

	// LED initial setup
	P1DIR |= RED_LED + GREEN_LED;	// ENABLE GREEN AND RED LED
	P1OUT &= ~GREEN_LED;			// Green off
	P1OUT &= ~RED_LED;				// Red off

	led_semaphore = 0; // nobody is currently using the LED

}

void blinkGreen () {

	// preamble
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT &= ~RED_LED;		// Red off
	int i;

	while (1 == 1) {

		// switch green on
		iLEDonCount = 0;
		P1OUT |= GREEN_LED;
		while (iLEDonCount <= LED_ON_TIMER){
			i = iLEDonCount;  // just do something so the while does not get removed
		}

		// switch green off
		iLEDonCount = 0;
		P1OUT &= ~GREEN_LED;
		while (iLEDonCount <= LED_ON_TIMER){
			i = iLEDonCount;  // just do something so the while does not get removed
		}
	}
}
// turn red on, then off again
void redOnForTwoSeconds () {

	iLEDonCount = 0;

	// switch red on
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT |= RED_LED;		// Red on

	int i;
	while (iLEDonCount <= LED_ON_TIMER){
		i = iLEDonCount;  // just do something so the while does not get removed
	}

	// everything off
	P1OUT &= ~GREEN_LED;	// Green off
	P1OUT &= ~RED_LED;		// Red off

}
