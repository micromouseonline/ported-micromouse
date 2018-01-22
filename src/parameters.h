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
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

// speeds are always given as the index into the acceleration table.
// bigger is always faster
// conveniently this index is also the number of steps needed to come to rest
#define SPEEDMAX_EXPLORE      225		//	 must be slow enough to stop easily in 90mm or less
#define SPEEDMAX_STRAIGHT     225		// must be able to get down to smooth turn speed in 20mm
#define SPEEDMAX_SPIN_TURN    225
#define SPEEDMAX_SMOOTH_TURN   70		// affects turn radius

// the steering error needs to be constrained to keep from over correcting
#define STEERING_ERROR_MAX			32



#define MOTOR_IDLE_51Hz (F_MOTOR_TIMER/51)    // motor idle frequency is 51Hz
#define MOTOR_IDLE_57Hz (F_MOTOR_TIMER/57)    // motor idle frequency is 57Hz
const int SYSTICK_FREQUENCY = 250;
// values are sum of left and right motor steps
#define STEPS_FOR_ONE_METER  (8248L)
#define STEPS_FOR_360DEG   (2303L)	// 360 degrees

// Convert distances in millimeters to step counts
#define MM(X) (((X) * STEPS_FOR_ONE_METER)/1000L)
// convert angles in degrees to step counts
#define DEG(X) (((X) * STEPS_FOR_360DEG )/360L)


// Calibration values are the raw reading from the sensor
// NOTE: side sensors see the front wall when the mouse is centered
#define LD_CAL 150
#define RD_CAL 150
// NOTE: front sensor calibration is with the mouse against the rear wall
#define LF_CAL 190
#define RF_CAL 165

// values that the sensors get normalised to when the mouse is correctly positioned
// defined as longs to prevent overflow when normalising
#define LD_NOMINAL 100L
#define RD_NOMINAL 100L
#define LF_NOMINAL 100L
#define RF_NOMINAL 100L

// when we are within 50mm of any wall ahead,
// the side sensors are unreliable. This is the normalised value seen by
// (sensFL + sensFR) when we are too close
#define FRONT_WALL_INTERFERENCE_THRESHOLD 300

// Thresholds for wall detection are compared to the normalised value
#define DIAG_THRESHOLD 40
#define FRONT_THRESHOLD 20

#define LEFT_FRONT_ERROR_MIN 5
#define RIGHT_FRONT_ERROR_MIN 5

// edge positions in mm
#define LEFT_EDGE_OFFSET MM(185L)
#define RIGHT_EDGE_OFFSET MM(185L)

// the level the sensor must exceed before it sees a finger in front for
// non-contact starting
#define SENSOR_OCCLUDED_LEVEL 100




#endif /* PARAMETERS_H_ */
