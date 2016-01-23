/*
 * functions.h
 *
 *  Created on: 21 Jan 2016
 *      Author: offic
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define BUTTON  	BIT3 // Port 1.3
#define RED_LED   	BIT0 // Port 1.0
#define GREEN_LED  	BIT6 // Port 1.6


void setupLEDs ();
void blinkGreen ();
void redOnForTwoSeconds ();

#endif /* FUNCTIONS_H_ */
