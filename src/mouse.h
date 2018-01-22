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


#ifndef MOUSE_H
#define MOUSE_H


enum {
  FRESH_START,
  SEARCHING,
  INPLACE_RUN,
  SMOOTH_RUN,
  FINISHED
};

class Mouse {
public:
  unsigned char heading;
  unsigned char location;
  bool leftWall;
  bool frontWall;
  bool rightWall;
  bool handStart;
};

extern char mouseState;

extern Mouse mouse;
extern  char path[];
extern  char commands[];

void mouseInit();
void mouseCheckWallSensors();
void mouseTurnToFace(unsigned char newHeading);
void mouseFollowTo(int target);
int mouseSearchTo(int target);
void mouseRunInplaceTurns(int topSpeed);
void mouseRunSmoothTurns(int topSpeed);
void mouseUpdateMapFromSensors();

int mouseSearchMaze();
int mouseRunMaze();

bool pathGenerate(unsigned char startCell);
void pathExpand(char * pathString);


#endif //MOUSE_H
