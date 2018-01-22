/***********************************************************************
 * Created by Peter Harrison on 03/01/2018.
 * Copyright (c) 2018 Peter Harrison
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


#include "test.h"
#include "hardware.h"
#include "parameters.h"
#include "maze.h"
#include "mouse.h"
#include "motors.h"
#include "ui.h"
#include "sensors.h"
#include "motion.h"
#include "navigator.h"
#include "volatiles.h"
#include "streaming.h"

// taken from Repetier 3D printer sources
int getFreeRam() {
  int freeram = 0;
  asm("cli" :: : "memory");
  unsigned char *heapptr, *stackptr;
  heapptr = (unsigned char*)(malloc(4));   // get heap pointer
  free(heapptr); // free up the memory again (sets heapptr to 0)
  stackptr = (unsigned char*)((SP));   // save value of stack pointer
  freeram = (int)(stackptr) - (int)(heapptr);
  asm("sei" :: : "memory");
  return freeram;
}




void testSensors() {
}


void testSteeringErrorSides() {
  steeringMode = SM_STRAIGHT;
  int error = steeringError;//getSteeringError();
  console << F("L:") << _JUSTIFY(sensL, 4) << F(",  ");
  console << F("R:") << _JUSTIFY(sensR, 4) << F(",  ");
  console << F(" error =") << _JUSTIFY(error, 4) << endl;
}

void testSteeringErrorFront() {
  steeringMode = SM_FRONT;
  int error = getSteeringError();
  console << F("FR:") << _JUSTIFY(sensFR, 4) << F(",  ");
  console << F("FL:") << _JUSTIFY(sensFR, 4) << F(",  ");
  console << F(" error =") << _JUSTIFY(error, 4) << endl;
}

/***
 * place the mouse at the back of a cell with walls on either side and
 * no walls in the next cell ahead.
 *
 * Mouse will stop 180 mm past the relevant sensor falling edge.
 *
 * Use this to adjust the side/diagonal sensors so that they both detect
 * the edge as nearly as possible at the same point.
 */
void testSensorEdge(int side) {
  volatile bool & sensor = wallSensorRight;
  if (side == LEFT) {
    sensor = wallSensorLeft;
  }
  motorsEnable();
  forward(MM(30), 120, 120);
  while (sensor) {
    digitalWrite(RED_LED, sensor);
  }
  forward(MM(180), 120, 0);
  motorsDisable();
  digitalWrite(GREEN_LED, 1);
  delay(200);
  digitalWrite(GREEN_LED, 0);
}


void testMove() {
  if (waitForStart() == 0) {
    return ;
  }
  motorsEnable();
  digitalWrite(GREEN_LED, 1);
  digitalWrite(RED_LED, 1);
  steeringMode = SM_NONE;
  startForward(SPEEDMAX_EXPLORE);
  motorsWaitUntil(MM(90));
  digitalWrite(RED_LED, 0);
  motorsStopAt(MM(180));
  digitalWrite(GREEN_LED, 0);
  motorsDisable();
}

void testForward(long distance, int maxSpeed) {
  if (waitForStart() == 0) {
    return ;
  }
  motorsEnable();
  digitalWrite(GREEN_LED, 1);
  digitalWrite(RED_LED, 1);
  steeringMode = SM_NONE;
  startForward(maxSpeed);
  motorsWaitUntil(distance - 2 * maxSpeed);
  digitalWrite(RED_LED, 0);
  motorsStopAt(distance);
  digitalWrite(GREEN_LED, 0);
  motorsDisable();
}

void testSteering() {
  int choice = waitForStart();
  if (choice == LEFT) {
    steeringMode = SM_STRAIGHT;
  } else if (choice == RIGHT) {
    steeringMode = SM_FRONT;
  } else {
    return;
  }
  motorsEnable();
  forward(MM(10 * 180), SPEEDMAX_EXPLORE, 0);
  motorsDisable();
}

void testFollower(int target) {
  if (waitForStart() == 0) {
    return ;
  }
  motorsEnable();
  steeringMode = SM_STRAIGHT;
  mouse.handStart = true;
  mouse.location = 0;
  mouse.heading = NORTH;
  mouseFollowTo(target);
  motorsDisable();
}

void testSearcher(int target) {
  if (waitForStart() == 0) {
    return ;
  }
  motorsEnable();
  mouse.handStart = true;
  steeringMode = SM_STRAIGHT;
  mouse.location = 0;
  mouse.heading = NORTH;
  int result = mouseSearchTo(target);
  debug << F("Arrived at goal: status ") << result << endl;
  if (result != 0) {
    panic();
  }

  digitalWrite(GREEN_LED, 1);
  delay(200);
  result = mouseSearchTo(0);
  motorsDisable();
  if (result != 0) {
    panic();
  }
  debug << F("Arrived at home: status ") << result << endl;
  digitalWrite(RED_LED, 1);

}

void testCalibrateFrontSensors() {
  int sum[127];
  int diff[127];
  waitForClick();
  motorsEnable();
  steeringMode = SM_FRONT;
  int distance = 0;
  startReverse(50);
  while (distance < 127) {
    long steps = getStepCount();
    int mm = (1000L * steps) / STEPS_FOR_ONE_METER;
    if (mm != distance) {
      int left = getVolatile(sensFL);
      int right = getVolatile(sensFR);
      console << _JUSTIFY(mm, 4) << ' ';
      console << _JUSTIFY(left, 4) << ' ';
      console << _JUSTIFY(right, 4) << ' ';
      console << _JUSTIFY(left + right, 4) << ' ';
      console << _JUSTIFY(sensorGetFrontDistance(), 4) << ' ';
      console << _JUSTIFY(sensorGetFrontSteering(mm), 4) << endl;
      sum[distance] = left + right;
      diff[distance] = left - right;
      distance = mm;
    }
  }
  forward(-60, 0, 0);
  motorsDisable();
  for (int i = 0; i < 127; i++) {
    console << _JUSTIFY(sum[i], 5) << ',';
  }
  console.println();
  for (int i = 0; i < 127; i++) {
    console << _JUSTIFY(diff[i], 5) << ',';
  }
  console.println();
}
