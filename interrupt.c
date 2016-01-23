/*
 * interrupt.c
 *
 *  Created on: 20 Jan 2016
 *      Author: offic
 */
#include <msp430.h>

#include "interrupt.h"
#include "scheduler.h"
#include "functions.h"

#define TIME_TO_CALL_SCHEDULER  5000 // timer triggers once a second i.e. change threads every 5 seconds
#define BUTTON_IE P1IE
#define BUTTON_IFG P1IFG
#define BUTTON_IES P1IES

int callScheduler;
int iLEDonCount;

void setupButton (void) {

	// Button setup
	P1DIR &= ~BUTTON; 				// button is an input
	P1OUT |= BUTTON; 				// pull-up resistor
	P1REN |= BUTTON; 				// resistor enabled
	BUTTON_IES |= BUTTON; 			// high-to-low transition
	BUTTON_IFG &= ~BUTTON; 			// clear eventual interrupt status (floating gate)
	BUTTON_IE |= BUTTON; 			// enable module interrupt

}

void setupTimer (void) {

	// This sets the MSP430's clock to a factory calibrated 1 MHz,
	// which means that any delays we specify will be more accurate
	DCOCTL  = CALDCO_1MHZ;
	BCSCTL1 = CALBC1_1MHZ;

	// Setup your timer
	TA0CCTL1 = CM_0 | OUTMOD_7;		// no capture (->ignore CCISx, SCS, SCCI), Compare mode, Reset/Set outmode, ignore "rest"
	CCTL0 = CCIE;					// CCRO Interrupt enable
	TA0CTL = 						// Timer TA0
			TASSEL_2 +				// TASSEL_2 selects SMCLK clock
			ID_0 + 					// ???
			MC_1 + 					// ???
			TACLR;					// Clear timer

	TACCR0 = 1000; 					// 1 ms @ 1MHz i.e. timer gets called every second

	// initialise your schedule timer
	callScheduler = 0;
	iLEDonCount = 0;
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer (void) {

	// up your timers
	callScheduler++;
	iLEDonCount++;	// ie. LED has been ON for another ms

	// do we need to call the scheduler
	if(callScheduler >= TIME_TO_CALL_SCHEDULER) {
		callScheduler = 0;
		scheduler_runNextThread ();
	}

}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void process_button_press (void) {

	// don't want to get interrupted
	atomic_start ();

	// remove interrrupt flag so you can process the next one
	BUTTON_IFG &= ~BUTTON;

	if (0 == led_semaphore)
		scheduler_startThread (&redOnForTwoSeconds, WAITING);
	else
		scheduler_startThread (&redOnForTwoSeconds, BLOCKED);
	led_semaphore++;

	// fin
	atomic_end();

}
