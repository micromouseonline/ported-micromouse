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


#ifndef SENSORS_H
#define SENSORS_H



extern bool sensorsEnabled;

// normalised sensor values
extern volatile int sensFR;
extern volatile int sensFL;
extern volatile int sensR;
extern volatile int sensL;

extern volatile int frontSum;
extern volatile int frontDiff;
// raw values
extern int rawFR;
extern int rawFL;
extern int rawR;
extern int rawL;


extern volatile int sensorState;

// sensor wall detection
extern volatile bool wallSensorRight;
extern volatile bool wallSensorLeft;
extern volatile bool wallSensorFrontRight;
extern volatile bool wallSensorFrontLeft;
extern volatile bool wallSensorFront;

void sensorsInit();
int sensorGetFrontDistance();
int sensorGetFrontSteering(int distance);
void sensorUpdate();
void sensorsEnable();
void sensorsDisable();


class sensors {

};

#endif //SENSORS_H
