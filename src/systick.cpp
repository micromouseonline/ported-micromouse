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



#include "systick.h"
#include "hardware.h"
#include "ui.h"
#include "sensors.h"
#include "navigator.h"

#ifndef  TCNT3
#error "SYSTICK uses TIMER3"
#endif

static unsigned int timerReload = (F_CPU / 8L / 250);

/***
 * Normally the systick event is triggered at a regular interval by a
 * timer interrupt. In PORTEDmouse, the interval is 250 Hz. For a DC
 * mouse with a faster processor, systick will run at 1kHz or faster
 * and will also process the encoders and update all the PID controllers
 * as well as generate the motor PWM signals
 */
void systickInit(int frequency) {
  timerReload = (F_CPU / 8L / frequency);
  /// Timer 3 is used to generate the 500Hz system update
  TCCR3A = 0x00;  // normal port operation, no auto change of OCnA/B/C pins
  TCNT3 = 0;      // start the timer at 0x0000
  OCR3A = timerReload;
  TCCR3B = 0x02; //  0: off, 1 => FCPU/1, 2 => FCPU/8, 3 => FCPU/64
  TIMSK3 |= (1 << OCIE3A);
}

/***
 * systick is defined as a function so that it may be called from
 * test code when the interrupt is not running
 */

void systick() {
  // enable interrupts for the motor driver
  sei();
  navigatorUpdate();
  debouncePin(BUTTON);
  sensorUpdate();
}

ISR(TIMER3_COMPA_vect) {      // 500Hz system timer
  OCR3A += timerReload;		// do this early to preserve the timing
  systick();
}
