/***********************************************************************
 * Created by Peter Harrison on 23/12/2017.
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

#include "motors.h"
#include "hardware.h"
#include "parameters.h"
#include "acctable.h"
#include "volatiles.h"

volatile long offsetCount;	// position within a cell
volatile long positionCount;	// sum of steps by both motors;

int speedTargetLeft;
int speedTargetRight;
volatile int speedLeft;
volatile int speedRight;

bool slowLeftMotor;
bool slowRightMotor;

void motorsInit() {
  motorsSetDirection(FORWARD);
  setMicrostepMode(MICROSTEP_2);
  slowLeftMotor = false;
  slowRightMotor = false;
  motorsDisable();
  motorsHalt();
}

void motorsSetDirection(int direction) {
  if (direction == FORWARD) {
    SET_RIGHT_FD();
    SET_LEFT_FD();
  } else if (direction == RIGHT) {
    SET_RIGHT_BK();
    SET_LEFT_FD();
  } else if (direction == LEFT) {
    SET_RIGHT_FD();
    SET_LEFT_BK();
  } else if (direction == REVERSE) {
    SET_RIGHT_BK();
    SET_LEFT_BK();
  }
}

void motorRightUpdate() {

  unsigned int timerInterval;

  if (speedRight < speedTargetRight) {
    speedRight++;
  }
  if (speedRight > speedTargetRight) {
    speedRight--;
  }
  speedRight = constrain(speedRight, 0, SPEED_TABLE_END);
  if (speedRight == 0) {
    timerInterval = MOTOR_IDLE_57Hz;
  } else {
    digitalWriteFast(STEPR, 1);
    timerInterval = accTable(speedRight);
    if (slowRightMotor) {
      timerInterval += (timerInterval / 8);
    }
    offsetCount++;
    positionCount++;
    digitalWriteFast(STEPR, 0);
  }
  OCR1B += timerInterval;
}


void motorLeftupdate() {
  unsigned int timerInterval;

  if (speedLeft < speedTargetLeft) {
    speedLeft++;
  }
  if (speedLeft > speedTargetLeft) {
    speedLeft--;
  }
  speedLeft = constrain(speedLeft, 0, SPEED_TABLE_END);
  if (speedLeft == 0) {
    timerInterval = MOTOR_IDLE_51Hz;
  } else {
    digitalWriteFast(STEPL, 1);
    timerInterval = accTable(speedLeft);
    if (slowLeftMotor) {
      timerInterval += (timerInterval / 8);
    }
    offsetCount++;
    positionCount++;
    digitalWriteFast(STEPL, 0);
  }
  ;
  OCR1A += timerInterval;
}


ISR(TIMER1_COMPA_vect) {      // interrupt service routine
  motorLeftupdate();
}

ISR(TIMER1_COMPB_vect) {      // interrupt service routine
  motorRightUpdate();
}

// simply stop them moving. Current may still flow.
void motorsHalt() {
  uint8_t oldSREG = SREG;
  cli();
  speedLeft = 0;
  speedRight = 0;
  speedTargetLeft = 0;
  speedTargetRight = 0;
  SREG = oldSREG;
}

void motorsEnable() {
  digitalWriteFast(NENABLE, 0);
}

void motorsDisable() {
  digitalWriteFast(NENABLE, 1);
}

void motorsResetCounters() {
  uint8_t oldSREG = SREG;
  cli();
  positionCount = 0;
  offsetCount = 0;;
  SREG = oldSREG;
}

long getStepCount() {
  return getVolatile(positionCount);
}

void motorsWaitUntil(long targetSteps) {
  while (getStepCount() < targetSteps) {
    delay(1);
  }
}

void motorsStopAt(long target) {
  bool done = false;
  while (!done) {
    int remaining = target - getStepCount();
    int speed = getVolatile(speedRight) + getVolatile(speedLeft);
    if (remaining < speed) {
      done  = true;
    }
  }
  // hit the brakes
  setVolatile(speedTargetRight, 1);
  setVolatile(speedTargetLeft, 1);
  done = false;
  while (!done) {
    done =  getStepCount() >= target;
  }
  // make sure they stop completely
  setVolatile(speedTargetRight, 0);
  setVolatile(speedTargetLeft, 0);
  setVolatile(speedRight, 0);
  setVolatile(speedLeft, 0);
}



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
void setMicrostepMode(int mode) {
  switch (mode) {
    case MICROSTEP_1:
      pinMode(M1, OUTPUT);
      pinMode(M0, OUTPUT);
      digitalWriteFast(M1, 0);
      digitalWriteFast(M0, 0);
      break;
    case MICROSTEP_2:
      pinMode(M1, OUTPUT);
      pinMode(M0, OUTPUT);
      digitalWriteFast(M1, 0);
      digitalWriteFast(M0, 1);
      break;
    case MICROSTEP_4:
      pinMode(M1, OUTPUT);
      pinMode(M0, INPUT);
      digitalWriteFast(M1, 0);
      digitalWriteFast(M0, 0);
      break;
    case MICROSTEP_8:
      pinMode(M1, OUTPUT);
      pinMode(M0, OUTPUT);
      digitalWriteFast(M1, 1);
      digitalWriteFast(M0, 0);
      break;
    case MICROSTEP_16:
      pinMode(M1, OUTPUT);
      pinMode(M0, OUTPUT);
      digitalWriteFast(M1, 1);
      digitalWriteFast(M0, 1);
      break;
    case MICROSTEP_32:
      pinMode(M1, OUTPUT);
      pinMode(M0, INPUT);
      digitalWriteFast(M1, 1);
      digitalWriteFast(M0, 0);
      break;
    default:
      // TODO: this is an error. We should handle it.
      break;
  }
}
