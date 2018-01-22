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

#ifndef VOLATILES_H_
#define VOLATILES_H_

#include <avr/interrupt.h>

/*
 * Some templated functions to handle setting, getting and comparing volatiles
 * Not needed to 8 bit quantities but essential for 16 and 32 bit quantities
 * The technique is simply to wrap the operation with calls to
 * enable and disable interrupts.
 *
 * Note that there is still no absolute guarantee this will prevent errors
 * cause by accessing volatiles.
 *
 * Wherever possible, provide other ways to handle quantities that can be
 * modified inside interrupt service routines.
 *
 * Also note that this is a quick hack rather than a comprehensive and carefully
 * considered solution to a common problem.
 */

template <typename T>
inline void setVolatile(volatile T & a, T const b) {
  uint8_t oldSREG = SREG;
  cli();
  a = b;
  SREG = oldSREG;
}


template <typename T>
inline T getVolatile(volatile T & a) {
  T temp;
  uint8_t oldSREG = SREG;
  cli();
  temp = a;
  SREG = oldSREG;
  return temp;
}

template <typename T>
bool GT(volatile const T & a, volatile const T  & b) {
  bool result;
  uint8_t oldSREG = SREG;
  cli();
  result = a > b;
  SREG = oldSREG;
  return result;
}

template <typename T>
bool LT(volatile const T & a, volatile const T  & b) {
  bool result;
  uint8_t oldSREG = SREG;
  cli();
  result = a < b;
  SREG = oldSREG;
  return result;
}

template <typename T>
bool GTE(volatile const T & a, volatile const T  & b) {
  bool result;
  uint8_t oldSREG = SREG;
  cli();
  result = a >= b;
  SREG = oldSREG;
  return result;
}

template <typename T>
bool LTE(volatile const T & a, volatile const T  & b) {
  bool result;
  uint8_t oldSREG = SREG;
  cli();
  result = a <= b;
  SREG = oldSREG;
  return result;
}


template <typename T>
bool NE(volatile const T & a, volatile const T  & b) {
  bool result;
  uint8_t oldSREG = SREG;
  cli();
  result = a != b;
  SREG = oldSREG;
  return result;
}

template <typename T>
int DIFF(volatile const T & a, volatile const T  & b) {
  int result;
  uint8_t oldSREG = SREG;
  cli();
  result = (a - b);
  SREG = oldSREG;
  return result;
}

template <typename T>
int ABS_DIFF(volatile const T & a, volatile const T  & b) {
  int result;
  uint8_t oldSREG = SREG;
  cli();
  result = abs(a - b);
  SREG = oldSREG;
  return result;
}



#endif /* VOLATILES_H_ */

