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


#include "navigator.h"
#include "hardware.h"
#include "parameters.h"
#include "motion.h"
#include "sensors.h"
#include "motors.h"


volatile STEERING_MODE steeringMode;
volatile int steeringError;


/***
 *
 * getSteeringError() is called by the navigator to allow correction of
 * alignment (heading) errors in the mouse while moving in a straight
 * line.
 *
 * The way the steering error is calculated depends upon the current mode.
 *
 * Normally the side sensors are used and the method is simply a question
 * of collision avoidance. the mouse is prevented from getting too close
 * to a wall. This can be a problem if there is a wall down just one side
 * of a long straight since the mouse may steer away and find no useful
 * correction from the other side.
 *
 * The front sensors can help since, with normal alignment, they cannot
 * see the walls or posts at the sides of the mouse. However, they are
 * aligned so that a heading or offset error will begin to generate a
 * response. That response will override the side sensor readings.
 *
 * Since the responses from posts alone are short lived transients
 * they may not last long enough to significantly affect the steering.
 * A low pass filter on the error signal prevents sudden step responses
 * and effectively prolongs the effect of a single post that is too close.
 * This low pass filter will need tuning by adjusting the value of alpha.
 * Alpha should probably be in the mouse parameter header.
 *
 * When the mouse is too close to a wall ahead, that wall will affect the
 * side sensor readings. To prevent that, a check should be made on the front
 * sensor values and when that condition is met, the steering mode should
 * be switched to SM_FRONT where the difference in the front sensor values
 * can be used for alignment.
 *
 * NOTE that the sensor responses are non-linear and likely to be quite
 * different from side to side unless they are very carefully set up.
 *
 * This will show up in the observation that a lateral error of the same
 * amount left and right will give different error values and so different
 * responses.
 *
 * Don't try and fix this in software. Get the sensor alignment right so
 * that the left and right responses are as close to the same as possible.
 *
 */

int sideSensorError() {
  int left = sensL;
  int right = sensR;
  int error = 0;
  if (left > LD_NOMINAL && right > RD_NOMINAL) {
    error = left - right;
  } else if (left < right  && right > RD_NOMINAL) {
    error = RD_NOMINAL - right;
  } else if (left > right && left > LD_NOMINAL) {
    error = left - LD_NOMINAL;
  }
  return error;
}

int getSteeringError() {
  int error = 0;
  static int errorOld;
  switch (steeringMode) {
    case SM_NONE:
      error = 0;
      break;
    case SM_STRAIGHT:
      error = sideSensorError();
      break;
    case SM_FRONT:
      // in this mode, the mouse should work out the distance to
      // the wall ahead and its orientation.
      // From these, the steering error can be calculated
      if ((sensFL + sensFR) > FRONT_WALL_INTERFERENCE_THRESHOLD) {
        // add code to use the front sensor readings
        error = 0;
      } 	else {
        error = sideSensorError();
      }
      break;
    default:
      error = 0;
      break;
  }
  /***
   * low pass filter the error value to get a smooth response to steps
   * Be sure that there can be no overflow here. It should be safe since
   * the error cannot exceed 900 except if there is a fault.
   * And, yes, there are rounding errors here. They are acceptable.
   */
  const int alpha = 28;
  error = (alpha * errorOld  + (32 - alpha) * error) / 32;
  errorOld = error;
  return error;
}


void doFEC() {
  /***
   * Not implemented in this version.
   *
   * The idea is to track the falling edge of the side sensors and
   * use their apparent position to correct the current mouse position.
   *
   * We know exactly where these edges should occur. If they appear early
   * or late that is an indication of an error. However, the error may be
   * simply that the mouse is running off to one side so the correction
   * is not simple.
   *
   * Since the motor interrupts are running, take care when altering the
   * distance traveled
   */
}

void navigatorUpdate() {
  if (steeringMode == SM_NONE) {
    steeringError = 0;
    return;
  }

  // apply the steering correction
  doAlignment();
  // correct for any forward errors
  // note that the motors are going to interrupt this code so take care
  doFEC();
  /***
   * The mouse maintains its position within a cell when moving forwards.
   * This is used when looking for wall edges and to help determine
   * the correct sensing and turning points.
   */
  if (offsetCount > MM(200)) {
    offsetCount -= MM(180);
  }
}


/***
 * Ensure that the mouse is the correct distance from a wall ahead.
 * Only do this if it is already within 40mm
 *
 * Assumes mouse is stationary and close to perpendicular to the wall.
 */
void adjustFrontDistance() {
  long error = 0;
  do {
    long dist  = sensorGetFrontDistance();
    if (dist < 40) {
      error =  MM(dist - 10);
      forward(error, 50, 0);
    }
  } while (error > 1 || error < -1);
}

/***
 * Ensure that the mouse is the correct angle to a wall ahead.
 * Only do this if it is already within 40mm
 *
 * Assumes mouse is stationary .
 */
void adjustFrontAngle() {
  long error = 0;
  do {
    long dist = sensorGetFrontDistance();
    if (dist < 40) {
      error = sensorGetFrontSteering(dist);
      spin((error / 8), 50, 0);
    }
  } while (error > 10 || error < -10);
}


/***
 * steering alignment is very simple and probably only suited to a
 * stepper design. Ideally it should be proportional and it should
 * work by applying a rotational component to the motors. However,
 * in a stepper mouse, it may simply not be possible to drive one
 * motor any faster so the rotation component would get complicated
 * to manage.
 *
 * Instead, here we just slow down one of the motors to make the
 * mouse turn. A fixed ratio is used for the change and the duration
 * of the correction is proportional to the error. In effect, that
 * makes the correction an angle and alignment errors are assumed
 * to be angular errors.
 *
 * Experiment with the amount and duration of the correction to tune
 * the mouse steering.
 */
void doAlignment() {
  // always update the steering error even if it is not used so that
  // it can be observed and logged
  // positive values mean we need to turn right
  steeringError = constrain(getSteeringError(), -STEERING_ERROR_MAX, STEERING_ERROR_MAX);

  // assume no correction is needed
  slowLeftMotor = 0;
  slowRightMotor = 0;

  if (steeringError > 0) {
    slowRightMotor = 1;
  }
  if (steeringError < 0) {
    slowLeftMotor = 1;
  }
}

