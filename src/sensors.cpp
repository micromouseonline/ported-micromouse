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

#include "sensors.h"
#include "hardware.h"
#include "parameters.h"


bool sensorsEnabled;
// sensor values
volatile int sensFR;
volatile int sensFL;
volatile int sensR;
volatile int sensL;

volatile int frontSum;
volatile int frontDiff;

int rawFR;
int rawFL;
int rawR;
int rawL;


volatile int sensorState;


// sensor wall detection
volatile bool wallSensorRight;
volatile bool wallSensorLeft;
volatile bool wallSensorFrontRight;
volatile bool wallSensorFrontLeft;
volatile bool wallSensorFront;

const PROGMEM int frontSumTable[] = {
  1046, 1043, 1040, 1036, 1029, 1026, 1015, 1006, 991, 977, 958, 942, 924, 903, 881, 858, 826, 799, 779, 752, 726, 698, 682, 657, 635, 612, 595, 575, 557, 542, 525, 509, 493, 482, 468, 454, 442, 426, 417, 403, 391, 381, 368, 360, 350, 340, 331, 323, 315, 309, 301, 291, 285, 279, 272, 264, 259, 254, 244, 242, 233, 230, 222, 217, 213, 210, 207, 201, 198, 192, 187, 182, 179, 174, 176, 169, 163, 163, 157, 156, 153, 148, 147, 146, 141, 140, 137, 132, 130, 130, 127, 125, 120, 119, 117, 115, 112, 110, 109, 107, 104, 102, 102, 99, 98, 95, 94, 92, 92, 88, 89, 86, 85, 83, 83, 81, 82, 79, 77, 74, 74, 73, 73, 72, 69, 69, 68,
};

const PROGMEM int frontDiffTable[] = {
  -92, -91, -90, -90, -91, -90, -91, -94, -103, -115, -118, -128, -130, -137, -141, -132, -126, -119, -115, -110, -106, -100, -96, -93, -87, -82, -81, -79, -73, -72, -67, -65, -67, -62, -58, -56, -52, -48, -47, -49, -45, -39, -40, -38, -36, -34, -35, -33, -31, -29, -27, -27, -27, -27, -22, -20, -21, -20, -18, -16, -17, -16, -14, -13, -13, -10, -13, -13, -12, -8, -9, -6, -5, -6, -8, -5, -3, -7, -3, -2, -3, -2, -5, 0, -3, 0, 1, 0, -4, -2, -1, 1, 0, -1, -1, -1, 0, 2, 3, 3, 0, 2, 2, 1, 0, 1, 2, 0, 2, 2, 1, 0, -1, 1, 3, 1, 2, 3, -1, 2, 2, 3, -1, 2, 1, -1, 2,
};

void sensorsInit() {
  sensorState = 0;
}

/***
 * Sensor reading takes 8 states.
 * The initial state of zero is only entered once and ensures all emitters off.
 * Time needed per state is between 15 and 30 us and the update is called at
 * a frequency of 4kHz so the load is between about 5% and 10%. Quite high.
 * It looks like some channels are slower to read than others!
 *
 * Note that the ADC hardware is significantly speeded up in hardwareInit();
 *
 * With this scheme, sensors are read only 500 times per second. That is fine for a
 * slow robot but significant improvements will be needed if speed increases.
 *
 * sensorUpdate takes about 600us to run and it could be made more efficient by
 * use of a state machine or by only calling it when the answers are needed
 *
 * All told, reading the sensors takes about 25% of the available processor time.
 *
 * Also, time wasted here robs the processor of valuable power for maze flooding
 * and control tasks.
 */

void sensorUpdate() {
  // gather all the unlit readings
  int darkL = analogRead(LEFT_DIAG);
  int darkR = analogRead(RIGHT_DIAG);
  int darkFL = analogRead(LEFT_FRONT);
  int darkFR = analogRead(RIGHT_FRONT);
  digitalWrite(LED_TX_RF, 1);	// front LEDs on
  digitalWrite(LED_TX_LF, 1);
  delayMicroseconds(50);
  int litFL = analogRead(LEFT_FRONT);
  int litFR = analogRead(RIGHT_FRONT);
  digitalWrite(LED_TX_RF, 0);	// front LEDs off
  digitalWrite(LED_TX_LF, 0);
  digitalWrite(LED_TX_RD, 1);	// side LEDs on
  digitalWrite(LED_TX_LD, 1);
  delayMicroseconds(50);
  int litL = analogRead(LEFT_DIAG);
  int litR = analogRead(RIGHT_DIAG);
  digitalWrite(LED_TX_RD, 0);	// side LEDs off
  digitalWrite(LED_TX_LD, 0);

  // all the dark and lit values have now been collected so
  // see what we got from reflection but never accept negative readings
  rawFL = max(litFL - darkFL, 0);
  rawFR = max(litFR - darkFR, 0);
  rawL = max(litL - darkL, 0);
  rawR = max(litR - darkR, 0);

  // now calculate the normalised readings
  sensFL = (rawFL * LF_NOMINAL) / LF_CAL;
  sensFR = (rawFR * RF_NOMINAL) / RF_CAL;
  sensL = (rawL * LD_NOMINAL) / LD_CAL;
  sensR = (rawR * RD_NOMINAL) / RD_CAL;

  frontSum = sensFL + sensFR;
  frontDiff = sensFL - sensFR;
  // there is some hysteresis built in to the side sensors to ensure
  // cleaner edges
  // decide whether walls are present - use both sensors at the front

  wallSensorFrontRight = sensFR > FRONT_THRESHOLD;
  wallSensorFrontLeft = sensFL > FRONT_THRESHOLD;
  wallSensorFront = wallSensorFrontRight && wallSensorFrontLeft;

  if (sensL < (DIAG_THRESHOLD)) {
    wallSensorLeft = false;
  } else  if (sensL > (DIAG_THRESHOLD + 5)) {
    wallSensorLeft = true;
  }
  if (sensR < (DIAG_THRESHOLD)) {
    wallSensorRight = false;
  } else if (sensR > (DIAG_THRESHOLD + 5)) {
    wallSensorRight = true;
  }

}



void sensorsEnable() {
  sensorsEnabled = true;
}

void sensorsDisable() {
  sensorsEnabled = false;
}

int sensorGetFrontDistance() {
  int i;
  for (i = 0; i < 127; i++) {
    if (frontSum >= (int)pgm_read_word_near(frontSumTable + i)) {
      break;
    }
  }
  return i;
}

int sensorGetFrontSteering(int distance) {
  int i = 0;
  if (distance >= 0 && distance < 127) {
    i = frontDiff - pgm_read_word_near(frontDiffTable + distance);
  }
  return i;
}
