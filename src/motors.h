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
#ifndef MOTORS_H_
#define MOTORS_H_


/***
 * With a set of four NiMH AA cells loaded up, the mouse can just about do
 * speed 1000 with microstepping set to half steps (MICROSTEP_2)
 *
 * Safer is half the speed with quarter stepping (MICROSTEP_4)
 *
 * With the acceleration table supplied, the maximum step frequency is 2.5kHz
 *
 * For this mouse, that corresponds to a top speed, using quarter steps, of about
 *
 * 30*3.14mm * 2500 /(200*4) = 295mm/s
 *
 * Not very quick.
 *
 * Take care before trying for higher frequencies since the ISRs will hve to run
 * 5000 times per second (2 wheels) along with the 4kHz sensor interrupt. The
 * sensor interrupt can be streamlined by directly talking to the ADC rather than
 * use the Arduino calls.
 *
 * Potentially, nearly 1200mm/s could be achieved at this pulse frequency if the
 * motor can be carefully accelerated while changing microstep mode.
 *
 * Alternatively, less weight would reduce the load seen by the motors or a
 * higher voltage could be applied to the motor drivers up to 10.8 Volts for the
 * DRV8834. Really, to get best performance, voltages of twice that will most
 * likely be needed but the DRV8834 could not cope.
 *
 * Live with the lower speed. Experiment with different microstep modes. This is
 * a training platform, not an international contest entry.
 *
 */
#define SET_RIGHT_FD() digitalWriteFast(DIRR, 1)
#define SET_RIGHT_BK() digitalWriteFast(DIRR, 0)
#define SET_LEFT_FD() digitalWriteFast(DIRL, 0)
#define SET_LEFT_BK() digitalWriteFast(DIRL, 1)

extern volatile long offsetCount;
extern volatile long positionCount;

extern int speedTargetLeft;
extern int speedTargetRight;
extern volatile int speedLeft;
extern volatile int speedRight;

extern  bool slowLeftMotor;
extern bool slowRightMotor; // flags indicate whether slowing left or right motor for steering

void motorsInit();
void setMicrostepMode(int mode);
void motorsHalt();
void motorsEnable();
void motorsDisable();
void motorsResetCounters();
long getStepCount();
void motorsSetDirection(int direction);

void motorsStopAt(long distance);
void motorsWaitUntil(long distance);
void motorRightUpdate();
void motorLeftupdate();


#endif /* MOTORS_H_ */
