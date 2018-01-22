/***********************************************************************
 * Created by Peter Harrison on 30/12/2017.
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


#include "motion.h"
#include "hardware.h"
#include "parameters.h"
#include "motors.h"
#include "mouse.h"
#include "navigator.h"
#include "volatiles.h"



/***
 * The integrated smooth turn simply speeds up the outer wheel and
 * slows down the inner wheel. The speeds are not specified as such
 * because of the way the stepper tables work. Instead, the wheels
 * accelerate to their new speeds over a fixed distance. The actual
 * wheel speeds after this are then held steady for another fixed
 * distance and the wheels are then allowed to return to their initial
 * speeds - again over a fixed distance.
 *
 * In this way, the key turn parameters are just the entry speed and
 * two distances. Only two because the first and last phase are the same.
 *
 * combined with the entry speed, the phase 1 and phase 3 distances will
 * determine the minimum radius of the turn. The phase 2 distance can be
 * adjusted to change the total angle turned by the mouse.
 *
 * This method is simple and effective but there is no obvious analytical
 * solution for a turn of a given speed and radius. It can be worked out
 * but that is left as an exercise to the reader.
 *
 * When setting up the straight and turn speeds, care must be taken to
 * ensure that the mouse can comfortably get down to the speed used when
 * setting up the phase1 and phase2 distances.
 *
 * The turning parameters - speeds, angles and distances - should really
 * be held in a table so that it is easy to look up the values
 * needed for different turn types. For example, rather than do two
 * successive right angle turns, it is better to do one smooth 180
 * degree turn.
 *
 * For this, the path processing will need more work and turnSmooth will
 * probably need to be given a turn type and direction.
 *
 * In a DC mouse, a very similar process is used. There are still the three
 * phases but the desired angular velocity is easily calculated in advance
 * given knowledge of the entry speed and the desired radius. Note that
 * the relation ship V = w x R always holds. R is the radius, V is the
 * tangential speed and w is the angular velocity. Thus, to run a turn
 * faster but with the same radius, w must be increased in proportion.
 */
void turnSmooth(int direction) {
  steeringMode = SM_NONE;
  motorsSetDirection(FORWARD);
  int phase1 = 220;	// adjust for radius
  int phase2 = 500;	// adjust for angle
  noInterrupts();
  if (direction == RIGHT) {
    speedTargetLeft = 800;
    speedTargetRight = 1;
  } else {
    speedTargetLeft = 1;
    speedTargetRight = 800;
  }
  positionCount = 0;
  long targetSteps = phase1;
  interrupts();
  // phase 1 - accelerating phase
  while (getStepCount() < targetSteps) {
    ; // do nothing
  }
  // phase 2 - constant radius
  noInterrupts();
  speedTargetLeft = speedLeft;
  speedTargetRight = speedRight;
  targetSteps += phase2;
  interrupts();
  while (getStepCount() < targetSteps) {
    ; // do nothing
  }
  // phase 3 - decelerating phase
  noInterrupts();
  speedTargetLeft = SPEEDMAX_SMOOTH_TURN;
  speedTargetRight = SPEEDMAX_SMOOTH_TURN;
  targetSteps += phase1;
  interrupts();
  while (getStepCount() < targetSteps) {
    ; // do nothing
  }
  noInterrupts();
  speedLeft = SPEEDMAX_SMOOTH_TURN;
  speedRight = SPEEDMAX_SMOOTH_TURN;
  interrupts();
}



/***
 * General purpose motion profiler where both motors have to move at the same speed.
 * Used for forward/reverse moves and in-place turns;
 *
 * Profiling is a simple question of accelerating the mouse until we need
 * to brake while limiting the maximum speed.
 *
 * For a stepper mouse, this is easy because the number of steps needed
 * to bring a motor to a given speed index is just the difference in index
 * values for the two speeds.
 *
 * Private to this module. Only call from other code that sets up the motor
 * directions correctly
 */
void move(long steps, int maxSpeed, int exitSpeed) {
  noInterrupts();
  speedTargetLeft = maxSpeed;
  speedTargetRight = maxSpeed;
  positionCount = 0;
  interrupts();
  long brakingSteps;
  long remainingSteps;
  // accelerating phase
  do {
    remainingSteps = steps - getVolatile(positionCount);
    brakingSteps = getVolatile(speedRight) + getVolatile(speedLeft);
    // NOTE: do not divide speed by 2 as we want the total number of
    // braking steps here
    brakingSteps -= exitSpeed * 2;
  } while (remainingSteps >= brakingSteps);
  // decelerating phase
  if (exitSpeed > 0) {
    setVolatile(speedTargetRight, exitSpeed);
    setVolatile(speedTargetLeft, exitSpeed);
  } else { // ensure we get to that last step;
    setVolatile(speedTargetRight, 1);
    setVolatile(speedTargetLeft, 1);
  }
  while (getVolatile(positionCount) < steps) {
    ; // do nothing
  }
  // force the current speed to match the set speed
  setVolatile(speedTargetRight, exitSpeed);
  setVolatile(speedTargetLeft, exitSpeed);
  setVolatile(speedRight, exitSpeed);
  setVolatile(speedLeft, exitSpeed);
}

/***
 * Turn in place by the given number of steps.
 * Steps will be the sum of the left and right motor step counts
 * Positive steps is anticlockwise or left, negative is clockwise or right
 * maxSpeed and exitSpeed are indexes into the acceleration tables.
 * Normally, the exit speed would be zero but it need not be.
 */
void spin(long steps, int maxSpeed, int exitSpeed) {
  steeringMode = SM_NONE;
  if (steps >= 0) {
    motorsSetDirection(LEFT);
  } else {
    motorsSetDirection(RIGHT);
    steps = -steps;
  }
  move(steps, maxSpeed, exitSpeed);
}



/***
 * note that the motors may already be moving when this is called
 * All it really does is reset the counters and set a new maximum speed
 * which may be larger or smaller than the current speed
 */
void startForward(int maxSpeed) {
  motorsSetDirection(FORWARD);
  motorsResetCounters();
  noInterrupts();
  speedTargetLeft = maxSpeed;
  speedTargetRight = maxSpeed;
  interrupts();
}

void startReverse(int maxSpeed) {
  motorsSetDirection(REVERSE);
  motorsResetCounters();
  noInterrupts();
  speedTargetLeft = maxSpeed;
  speedTargetRight = maxSpeed;
  interrupts();
}


/***
 * Move forward or backwards by the given number of steps.
 * Steps is the sum of the left and right motor steps
 * maxSpeed and exitSpeed are indexes into the acceleration table.
 *
 */
void forward(long steps, int maxSpeed, int exitSpeed) {
  if (steps >= 0) {
    motorsSetDirection(FORWARD);
  } else {
    motorsSetDirection(REVERSE);
    steps = -steps;
  }
  move(steps, maxSpeed, exitSpeed);
}

void turnIP180() {
  static int direction = 1;
  direction *= -1;	// alternate direction each time it is called
  spin(direction * DEG(180), SPEEDMAX_SPIN_TURN, 0);
  mouse.heading = (mouse.heading + 2) & 0x03;
}

void turnIP90R() {
  spin(-DEG(90), SPEEDMAX_SPIN_TURN, 0);
  mouse.heading = (mouse.heading + 1) & 0x03;
}

void turnIP90L() {
  spin(DEG(90), SPEEDMAX_SPIN_TURN, 0);
  mouse.heading = (mouse.heading + 3) & 0x03;
}

void turnSS90L() {
  turnSmooth(LEFT);
  mouse.heading = (mouse.heading + 3) & 0x03;
}


void turnSS90R() {
  turnSmooth(RIGHT);
  mouse.heading = (mouse.heading + 1) & 0x03;
}

