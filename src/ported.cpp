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
#include "maze.h"
#include "sensors.h"
#include "mouse.h"
#include "motors.h"
#include "test.h"
#include "ui.h"
#include "streaming.h"






unsigned long eventInterval = 125; // in milliseconds
unsigned long eventTrigger;

static unsigned long boot_magic __attribute__((section(".noinit")));
void setup() {
  hardwareInit();
  console.begin(57600);
  digitalWrite(RED_LED, 1);
  sensorsDisable();
  motorsInit();

  if (boot_magic == 0xFEEDBEEF) {    // this was an external reset
    digitalWrite(GREEN_LED, 1);
    console.println(F("\nExternal Reset.\n"));
  } else {                          // This was a power-on reset
    // NOTE: the hardware serial port connection can provide enough power to
    // the processor to prevent it seeing a power down.
    console.println(F("\nPower Up - clearing maze.\n"));
    mazeInit(emptyMaze);
    mouseState = FRESH_START;
    boot_magic = 0xFEEDBEEF;
  }
  mazeFlood(GOAL);
  mouseInit();
  sensorsEnable();
  console << F("Free RAM: ") << getFreeRam() << F(" bytes") << endl;
  console.write(':');
  eventTrigger = millis() + eventInterval;
  digitalWrite(RED_LED, 0);
  digitalWrite(GREEN_LED, 0);
}

int target = 0;
int flip = 0;
int i;


void loop() {

  breathePin(GREEN_LED);	// let the user know we are in standby
  if (millis() > eventTrigger) {
    eventTrigger += eventInterval;
    if (digitalRead(SEL1) == HIGH) {	// the same as the setting for FOLLOW
      //      printSensors();
    }
  }
  if (buttonPressed()) {
    doButton();
    eventTrigger = millis(); // or we do catching up of missed events!
  }
  if (console.available()) {
    doCLI();
    eventTrigger = millis(); // or we do catching up of missed events!
  }
}

