/***********************************************************************
 * Created by Peter Harrison on 22/12/2017.
 * Copyright (c) 2017 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
#ifndef HARDWARE_H
#define HARDWARE_H



#include "Arduino.h"
#include "digitalwritefast.h"

#define USE_DEBUG
extern HardwareSerial & console;
extern Stream & debug;

// set up all the pins, the timers, the serial port and enable the interrupts.
void hardwareInit();

// pin descriptions
#define SERIAL_RX 					0
#define SERIAL_TX 					1
#define BUTTON 			2	 // a pushbutton in parallel with SW2
#define SEL0 				2  // dip switch input SW2
#define SEL1 				3  // dip switch input SW1
#define LED_TX_RF 		4	 // wall sensor emitter controls
#define LED_TX_LD 		5
#define LED_TX_RD 		6
#define LED_TX_LF 		7
#define DIRR 				8	 // stepper control lines
#define STEPR 				9
#define DIRL 				10
#define STEPL 				11
#define NENABLE 			12
#define RED_LED 			13	 // on-board indicators
#define GREEN_LED		17
#define LEFT_FRONT 	A0	 // wall sensor receivers
#define RIGHT_FRONT 	A1
#define LEFT_DIAG 		A2
#define RIGHT_DIAG 	A3
#define M1 					A4 // microstepping control
#define M0 					A5

enum {
  MICROSTEP_1,
  MICROSTEP_2,
  MICROSTEP_4,
  MICROSTEP_8,
  MICROSTEP_16,
  MICROSTEP_32,
};
/*
 * microstepping modes for DRV8834. Z means pin is left floating
 *
 *   M1  M0  Mode
 *    0   0  Full step (1)
 *    0   1  half step (2)
 *    0   Z  quarter   (4)
 *    1   0   8th      (8)
 *    1   1  16th     (16)
 *    1   Z  32nd     (32)
 */

enum {
  FORWARD, REVERSE, LEFT, RIGHT
};

#define F_CPU 16000000L
#define F_MOTOR_TIMER (F_CPU/8L)
#define F_COUNTER (F_CPU/8L)


#endif //HARDWARE_H
