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

#include "hardware.h"
#include "parameters.h"
#include "systick.h"
#include "NullSerial.h"
/***
 * On the Leonardo platform, Serial is a virtual com port and so
 * useless without a USB connection.
 *
 * Here we ensure that a 'proper' serial connection is made with the
 * Arduino TX and RX pins.
 *
 */
#if defined(ARDUINO_AVR_UNO)
HardwareSerial & console = Serial;
#elif defined(ARDUINO_AVR_LEONARDO)
HardwareSerial & console = Serial1;
#else
#error Unsupported hardware
#endif

NullSerial nullConsole;

#ifdef USE_DEBUG
Stream & debug = console;
#else
Stream & debug = nullConsole;
#endif

#ifdef USE_PRINTF
// Function that printf and related will use to print
int serial_putchar(char c, FILE* f) {
  if (c == '\n') {
    serial_putchar('\r', f);
  }
  return console.write(c) == 1 ? 0 : 1;
}

FILE serial_stdout;
void redirectPrintf() {
  // Redirect stdout so that we can use printf() with the console
  fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &serial_stdout;
}
#else
void redirectPrintf() {};
#endif



void hardwareInit() { //FIXED
  pinMode(SERIAL_TX, OUTPUT);	// enabling the USART makes this an output anyway
  pinMode(SERIAL_RX, INPUT_PULLUP);
  pinMode(NENABLE, OUTPUT);
  digitalWrite(NENABLE, 1); // disable the motors as soon as possible
  pinMode(LED_TX_LF, OUTPUT);
  pinMode(LED_TX_RF, OUTPUT);
  pinMode(LED_TX_LD, OUTPUT);
  pinMode(LED_TX_RD, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(SEL1, INPUT);
  pinMode(STEPL, OUTPUT);
  pinMode(STEPR, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  /// speed up the ADC to CLK/32 (4 times faster than normal)
  ADCSRA |= (1 << ADPS2);
  ADCSRA &= ~(1 << ADPS1);
  ADCSRA |= (1 << ADPS0);

  noInterrupts();
  /// Timer 1 is used to generate stepper motor pulses
  TCCR1A = 0x00;		// normal port operation, no auto change of OCnA/B/C pins
  TCNT1 = 0;				// start the timer at 0x0000
  OCR1A = MOTOR_IDLE_51Hz;	// mutually prime to reduce interaction
  OCR1B = MOTOR_IDLE_57Hz;
  TCCR1B = 0x02;		//  0: off, 1 => FCPU/1, 2 => FCPU/8, 3 => FCPU/64
  TIMSK1 |= (1 << OCIE1A);	// Timer 1 OC1A interrupt enable (0x10)
  TIMSK1 |= (1 << OCIE1B);	// Timer 1 OC1A interrupt enable (0x10)

  systickInit(SYSTICK_FREQUENCY);	// uses timer 3  
  redirectPrintf();
  // turn it all loose
  interrupts();
}



