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


#ifndef MAZE_H
#define MAZE_H


#define GOAL 0x22



// directions for mapping
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

#define VISITED 0xf0

#define INVALID_DIRECTION (0)
#define MAX_COST 255

extern const  unsigned char emptyMaze[];
extern const  unsigned char japan2007[];


// tables give new direction from current heading and next turn
const unsigned char DtoR[] = { 1, 2, 3, 0};
const unsigned char DtoB[] = { 2, 3, 0, 1};
const unsigned char DtoL[] = { 3, 0, 1, 2};

extern unsigned char cost[256];
extern unsigned char walls[256];



inline bool hasExit(unsigned char cell, unsigned char direction) {
  return ((walls[cell] & (1 << direction)) == 0);
}

inline bool hasWall(unsigned char cell, unsigned char direction) {
  return ((walls[cell] & (1 << direction)) != 0);
}

unsigned char cellNorth(unsigned char cell);
unsigned char cellEast(unsigned char cell);
unsigned char cellSouth(unsigned char cell);
unsigned char cellWest(unsigned char cell);
unsigned char neighbour(unsigned char cell, unsigned char direction);
unsigned char neighbourCost(unsigned char cell, unsigned char direction);
unsigned char directionToSmallest(unsigned char cell, unsigned char startDirection);

void mazeCopyWallsFromFlash(const unsigned char *src);
void mazeSetWall(unsigned char cell, unsigned char direction);

void mazeInit(const unsigned char *testMaze);
void mazeFlood(unsigned char target);




#endif //MAZE_H
